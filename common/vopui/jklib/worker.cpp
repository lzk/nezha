#include "worker.h"
#include <QVariant>
#include "uiconfig.h"

#include <QDebug>
#include <QString>
#include <QDir>
#include <QDateTime>
#include "lshell.h"
#include <unistd.h>
#include <QImage>
Worker::Worker(QObject *parent) :
    QObject(parent)
  ,cmd_status(0)
  ,deviceManager(new DeviceManager)
  ,lshell(new LShell(deviceManager))
  ,scanner(new ScannerApp(deviceManager))
{
    watcher = new Watcher(this);
//    connect(watcher ,SIGNAL(update_printer_status()) ,this ,SLOT(update_printer_status(PrinterInfo_struct)));
    connect(watcher ,SIGNAL(update_current_printer_status()) ,this ,SLOT(update_current_printer_status()) ,Qt::DirectConnection);
    connect(watcher ,SIGNAL(update_printerlist()) ,this ,SLOT(update_printerlist()) ,Qt::DirectConnection);
    watcher->start();
}

Worker::~Worker()
{
    delete watcher;
    delete deviceManager;
    delete lshell;
    delete scanner;
}


static void scan_callback(void* para);
void Worker::cmdFromUi(int cmd ,const QString& printer_name ,QVariant data)
{
    if(cmd_status)
        return;
    cmd_status = 1;

    QVariant value;
    Printer_struct* printer;
    Printer_struct ps;
    int index  = watcher->get_printer_from_current_list(printer_name ,ps);
    if(index<0){
        printer = NULL;
    }else{
        printer = &ps;
    }
    int result = -1;

    switch (cmd) {
    case UIConfig::CMD_SetCurrentPrinter:
        watcher->set_current_printer(printer_name);
        value = 1;
        result = 0;
        cmdResult(cmd ,result ,value);
        break;

    case UIConfig::CMD_GetPrinters:
        update_printerlist();
        break;

    case UIConfig::CMD_GetStatus:
        update_current_printer_status();
//        if(printer){
//            PrinterInfo_struct printer_info;
//            printer_info.printer = *printer;
//            PrinterStatus_struct* status = &printer_info.status;
//            result = StatusMonitor::getDeviceStatus(deviceManager ,printer ,status);
//            value.setValue(printer_info);
//            LOGLOG("get current status:0x%02x" ,printer_info.status.PrinterStatus);
//        }
//        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::CMD_Scan:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                ScanSettings device_data = data.value<ScanSettings>();
                const char* imagePath = TMP_SCAN_DIR;
//                const char* imagePath = "/tmp/vop_scan";
//                QDir *path = new QDir;
//                if(!(path->exists(imagePath)))
//                {
//                    path->mkdir(imagePath);
//                }
//                delete path;
                switch (device_data.info->source->format) {
                case ImageTransFormat_jpg:
                    sprintf(device_data.tmpfilename, "%s/tmpscan.jpg", TMP_SCAN_DIR);
                    break;
                case ImageTransFormat_raw:
                    sprintf(device_data.tmpfilename, "%s/tmpscan.raw", TMP_SCAN_DIR);
                    break;
                default:
                    memset(device_data.tmpfilename ,0 ,sizeof(device_data.tmpfilename));
                    break;
                }
                QDateTime time = QDateTime::currentDateTime();
                QString str_time = time.toString("yyyy-MM-dd_hh-mm-ss-zzz");
                QByteArray t_ba = str_time.toLatin1();
                sprintf(device_data.filename, "%s/%s.bmp", imagePath, t_ba.constData());

//                device_data.settings.scan_type = Hight_Quality;
                device_data.callback_para = this;
                device_data.callback = scan_callback;
                result = scanner->scan(printer ,&device_data);
                if(!result){
                    QFileInfo info(device_data.filename);
                    QString previewFileName = info.absolutePath() + "/prvw_" + info.fileName();
                    QImage image(device_data.filename);
                    if(image.depth() != 1){
                        QSize size = image.size();
                        while(size.width() * size.height() > 30 * 1024 * 1024){
                            size /= 2;
                        }
                        image.scaled(size).save(previewFileName);
                    }else{
                        QFile::copy(device_data.filename ,previewFileName);
                    }
                }
                value.setValue(device_data);
            }

        }
        cmdResult(cmd ,result ,value);
        break;

    case UIConfig::LS_CMD_GetCopy:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    copycmdset device_data;
                    result = lshell->copy_get_para(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_COPY:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    copycmdset device_data = data.value<copycmdset>();
                    result = lshell->copy(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_apply:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                cmdst_wifi_get device_data = data.value<cmdst_wifi_get>();
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        result = lshell->wifi_apply(&device_data);
                        lshell->close();
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
                value.setValue(device_data);
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_get:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_wifi_get device_data;
                    result = lshell->wifi_get_para(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;

    case UIConfig::CMD_WIFI_refresh_plus:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                struct_wifi_refresh_info wifi_refresh_data;
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        result = lshell->wifi_get_para(&wifi_refresh_data.wifi_para);

                        cmdst_wifi_status status_data = 0;
                        wifi_refresh_data.wifi_status = false;
                        if(!result){
                            result = lshell->wifi_get_status(&status_data);
                        }
                        if(status_data != 0 && !result){
                            wifi_refresh_data.wifi_status = true;
                            result = lshell->wifi_get_aplist(&wifi_refresh_data.wifi_aplist);
                        }
                        lshell->close();
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
                value.setValue(wifi_refresh_data);
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_getAplist:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_aplist_get device_data;
                    result = lshell->wifi_get_aplist(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PASSWD_set:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_passwd device_data = data.value<cmdst_passwd>();
                        result = lshell->password_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PASSWD_get:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_passwd device_data;
                    result = lshell->password_get(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PASSWD_confirm:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_passwd device_data = data.value<cmdst_passwd>();
                        result = lshell->password_confirm(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_GetWifiStatus:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_wifi_status device_data;
                    result = lshell->wifi_get_status(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_TonerEnd_Get:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_tonerEnd device_data;
                    result = lshell->tonerend_get(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_TonerEnd_Set:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_tonerEnd device_data = data.value<cmdst_tonerEnd>();
                    result = lshell->tonerend_set(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_PSaveTime_Get:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_PSave_time device_data;
                        result = lshell->savetime_get(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_PSaveTime_Set:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_PSave_time device_data = data.value<cmdst_PSave_time>();
                        result = lshell->savetime_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_PowerOff_Get:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_powerOff_time device_data;
                    result = lshell->poweroff_get(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_PowerOff_Set:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    cmdst_powerOff_time device_data = data.value<cmdst_powerOff_time>();
                    result = lshell->poweroff_set(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }

        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_NET_GetV4:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        net_info_st device_data;
                        result = lshell->ipv4_get(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_NET_SetV4:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        net_info_st device_data = data.value<net_info_st>();
                        result = lshell->ipv4_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_NET_GetV6:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        net_ipv6_st device_data;
                        result = lshell->ipv6_get(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_NET_SetV6:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        net_ipv6_st device_data = data.value<net_ipv6_st>();
                        result = lshell->ipv6_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_Set_UserConfig:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_userconfig device_data = data.value<cmdst_userconfig>();
                        result = lshell->userconfig_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_Get_UserConfig:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_userconfig device_data;
                        result = lshell->userconfig_get(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_Set_SoftAp:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_softap device_data = data.value<cmdst_softap>();
                        result = lshell->wifi_softap_set(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_WIFI_Get_SoftAp:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        cmdst_softap device_data;
                        result = lshell->wifi_softap_get(&device_data);
                        lshell->close();
                        value.setValue(device_data);
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }

        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_FusingScReset:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        result = lshell->fusingsc_reset();
                        lshell->close();
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_TonerReset:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        result = lshell->toner_reset();
                        lshell->close();
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_DrumReset:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        result = lshell->drum_reset();
                        lshell->close();
                    }

                    if(!result)
                        break;
                    else
                        usleep(100 *1000);
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_Get_UserCenterInfo:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                cmdst_user_center user_center;
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        memset(&user_center ,0 ,sizeof(user_center));
                        result = lshell->usercenterinfo_get(&user_center);
                        lshell->close();
                    }
                    if(!result){
                        value.setValue(user_center);
                        break;
                    }else
                        usleep(100 *1000);
                }
            }

        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_getCRMInfo:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                struct_user_center user_center;
                for(int i = 0 ;i < 3 ;i++){
                    result = lshell->open(printer);
                    if(!result){
                        memset(&user_center.user_center ,0 ,sizeof(user_center.user_center));
                        result = lshell->usercenterinfo_get(&user_center.user_center);
                        if(!result){
                            result = lshell->get_counter_print_scan(&user_center.counter_printer_scan);
                        }
                        lshell->close();
                    }

                    if(!result){
                        user_center.strPrinterModel = QString(printer->makeAndModel);
                        watcher->get_currentprinter_info(user_center.printer_info);
                        value.setValue(user_center);
                        break;
                    }else
                        usleep(100 *1000);
                }
            }

        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_poweroff:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    result = lshell->poweroff();
                    lshell->close();
                }
            }
        }
        cmdResult(cmd ,result ,value);
        break;
    case UIConfig::LS_CMD_PRN_getCounterPrintScan:
        if(printer){
            if(cmd_status_validate(printer ,cmd)){
                result = lshell->open(printer);
                if(!result){
                    Counter_printer_scan device_data;
                    result = lshell->get_counter_print_scan(&device_data);
                    lshell->close();
                    value.setValue(device_data);
                }
            }

        }
        cmdResult(cmd ,result ,value);
        break;
    default:
        break;
    }
    cmd_status = 0;
}

void Worker::cancel()
{
    scanner->set_cancel(true);
}

extern int usb_error_scanning;
void Worker::update_scan_progress(Printer_struct* printer ,int progress ,int is_jpg_mode)
{
    if(progress == -1){//start
        current_printer_info.printer = *printer;
        printer->status = usb_error_scanning;
        current_printer_info.status.PrinterStatus = usb_error_scanning;
        QVariant value;
        value.setValue<PrinterInfo_struct>(current_printer_info);
        cmdResult(UIConfig::CMD_GetStatus ,0 ,value);
        emit signal_update_scan_progress(0);
    }else if(progress == -2){
        emit signal_update_scan_progress(100);
        printer->status = 0;
        current_printer_info.status.PrinterStatus = 0;
//        update_current_printer_status();
    }else if(progress == -3){
        printer->status = 0;
        current_printer_info.status.PrinterStatus = 0;
//        update_current_printer_status();
    }else{
        if(is_jpg_mode){
            progress *= 20;
        }
        if(progress > 100)
            progress = 100;
        emit signal_update_scan_progress(progress);
    }
}

static void scan_callback(void* para)
{
    ScanSettings* settings = (ScanSettings*)para;
    Worker* worker = (Worker*) settings->callback_para;
    worker->update_scan_progress(settings->printer ,settings->progress ,(settings->settings.scan_type == Hight_Speed) ?1 :0);
}

void Worker::update_current_printer_status()
{
    PrinterInfo_struct ps;
    watcher->get_currentprinter_info(ps);
    if(current_printer_info.status.PrinterStatus == usb_error_scanning)//vop scanning
        return;
    QVariant value;
    value.setValue<PrinterInfo_struct>(ps);
    cmdResult(UIConfig::CMD_GetStatus ,0 ,value);
}

void Worker::update_printerlist()
{
    QList<Printer_struct> printer_list;
    QVariant value;
    watcher->get_printer_list(printer_list);
    value.setValue(printer_list);
    usleep(1000 * 1000);//last 1 second for refresh button -_-
    cmdResult(UIConfig::CMD_GetPrinters ,0 ,value);
}

bool Worker::cmd_status_validate(Printer_struct* printer ,int cmd)
{
    return true;
//    PrinterInfo_struct ps;
//    watcher->get_currentprinter_info(ps);
//    if(strcmp(printer->name ,ps.printer.name))
//        return false;
//    PrinterStatus_struct* status = &ps.status;
////    int result = StatusMonitor::getDeviceStatus(deviceManager ,printer ,status);
////    if(result < 0){
////        return false;
////    }
//    bool valid = true;

//    switch(status->PrinterStatus){
//    case UIConfig::Printing:
//    case UIConfig::PrintCanceling:
//    case UIConfig::CopyScanning:
//    case UIConfig::CopyPrinting:
//    case UIConfig::CopyCanceling:
//    case UIConfig::ScanScanning:
//    case UIConfig::ScanSending:
//    case UIConfig::ScanCanceling:
//    case UIConfig::ScannerBusy:
//    case UIConfig::CopyScanNextPage:
//    case UIConfig::InitializeJam:
//    case UIConfig::NofeedJam:
//    case UIConfig::JamAtRegistStayOn:
//    case UIConfig::JamAtExitNotReach:
//    case UIConfig::JamAtExitStayOn:
//        valid = false;
//        break;
//    case UIConfig::TonerEnd1:
//    case UIConfig::TonerEnd2:
//    case UIConfig::WasteTonerFull:
//        if(cmd == UIConfig::LS_CMD_COPY)
//            valid = false;
//        break;
//    case UIConfig::TonerNearEnd:
//    default:
//        break;
//    }
//    LOGLOG("cmd_status_validate:%d,%d",valid,status->PrinterStatus);
//    return valid;
}
