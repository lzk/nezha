#include "scanner.h"
#include "scannerapi.h"
#include "scannerapp.h"

Scanner::Scanner(ScannerApp* app)
    :scanner_app(app)
    ,scannner_api(new ScannerApi(app->device_manager))
{

}

Scanner::~Scanner()
{
    delete scannner_api;
}

extern int usb_error_scanning;
int Scanner::flat_scan(Printer_struct* printer ,ScanSettings* settings)
{
    int ret;
    if(m_cancel)
        return ScannerApp::STATUS_Cancel;

    ret = scannner_api->open(printer);
    if(ret){
        if(ret == usb_error_scanning)
            return ScannerApp::STATUS_Error_busy;
        else{
            LOGLOG("scanning...error:open");
            return ScannerApp::STATUS_Error_App;
        }
    }
    ret = scannner_api->lock();
    if(ret){
        LOGLOG("scanning...error:lock");
        scannner_api->close();
        if(ret == ScannerApp::STATUS_USEWITHOUTLOCK)
            return ScannerApp::STATUS_Error_busy;
        else
            return ScannerApp::STATUS_Error_lock;
    }

    ret = scannner_api->set_parameters(settings);
    if(ret){
        LOGLOG("scanning...error:set parameters");
        exit_scan();
        ret = ScannerApp::STATUS_Error_machine;
        return ret;
    }

    ret = scannner_api->start();
    if(ret){
        exit_scan();
        ret = ScannerApp::STATUS_Error_machine;
        return ret;
    }
    scanner_app->start_scan(settings);

    ScanStatus status;
    int size;
    char* buffer;
    int buf_size;
    Calc_Data *pCalc = &settings->calc_data;
    buffer = pCalc->scan_buffer;
    buf_size = pCalc->scan_buffer_size;
    while(!m_cancel){
        ret = scannner_api->get_scan_status(&status);
        if(ret){
            //maybe communication error
            break;
        }
        if(status.status == ScanStatus_Scanning){
            if(buf_size < status.data_size){
                LOGLOG("scanning...not enough buffer");
                ret = ScannerApp::STATUS_Error_App;
                break;
            }
            size = scannner_api->get_scan_data(buffer ,status.data_size);
//            LOGLOG("scanning...read:%d real:%d" ,status.data_size ,size);
            if(size != status.data_size){
                LOGLOG("scanning...error:get scan para");
                ret = ScannerApp::STATUS_Error_machine;
                if(size < 0){
                    //communication error
                    ret = -1;
                }
                break;
            }else{
                scanner_app->save_scan_data(settings ,buffer ,size);
            }
        }else if(status.status == ScanStatus_End){
            ret = status.error_code;
            break;
        }
    }
    if(m_cancel){
        if(ret >= 0)
            scannner_api->abort();
    }

    if(ret >= 0)
        scannner_api->stop();

    exit_scan();
    if(ret < 0)
        ret = ScannerApp::STATUS_Error_machine;
    if(m_cancel || (ret == ScannerApp::STATUS_SCANSTOPPED)){
        ret = ScannerApp::STATUS_Cancel;//cancel
    }
    return ret;
}

void Scanner::exit_scan()
{
    int aa = scannner_api->unlock();
    if(aa){
        LOGLOG("scanner unlock error:%d" ,aa);
    }
    scannner_api->close();
}

int Scanner::adf_scan(Printer_struct*  ,ScanSettings*)
{
    return 0;
}
