#include "statusmonitor.h"
#include "log.h"
#include "statusmanager.h"
#include "status.h"
#include "lld.h"

int parsePrinterStatus(PRINTER_STATUS* pStatus ,PrinterStatus_struct* ps)
{
    ps->PrinterStatus = pStatus->PrinterStatus;
//    LOGLOG("get device status correct:%#.02x" ,pStatus->PrinterStatus);

    if(pStatus->TonelStatusLevelC & 0x80){
//        LOGLOG("Tonel not installed");
        ps->TonelStatusLevelC = -1;
    }else{
//        LOGLOG("Tonel has been installed,TonelStatusLevelC level:%d" ,pStatus->TonelStatusLevelC);
        ps->TonelStatusLevelC = pStatus->TonelStatusLevelC;
    }

    if(pStatus->TonelStatusLevelY & 0x80){
//        LOGLOG("Tonel not installed");
        ps->TonelStatusLevelY = -1;
    }else{
//        LOGLOG("Tonel has been installed,TonelStatusLevelY level:%d" ,pStatus->TonelStatusLevelY);
        ps->TonelStatusLevelY = pStatus->TonelStatusLevelY;
    }

    if(pStatus->TonelStatusLevelM & 0x80){
//        LOGLOG("Tonel not installed");
        ps->TonelStatusLevelM = -1;
    }else{
//        LOGLOG("Tonel has been installed,TonelStatusLevelM level:%d" ,pStatus->TonelStatusLevelM);
        ps->TonelStatusLevelM = pStatus->TonelStatusLevelM;
    }

    if(pStatus->TonelStatusLevelK & 0x80){
//        LOGLOG("Tonel not installed");
        ps->TonelStatusLevelK = -1;
    }else{
//        LOGLOG("Tonel has been installed,TonelStatusLevelK level:%d" ,pStatus->TonelStatusLevelK);
        ps->TonelStatusLevelK = pStatus->TonelStatusLevelK;
    }

    if(pStatus->DrumStatusLifeRemain & 0x80){
//        LOGLOG("Tonel not installed");
        ps->drum = -1;
    }else{
//        LOGLOG("Tonel has been installed,TonelStatusLevelK level:%d" ,pStatus->TonelStatusLevelK);
        ps->drum = pStatus->DrumStatusLifeRemain;
    }
    switch(pStatus->PaperTrayStatus){
    case 0xff:
//        LOGLOG("paper tray status: tray no installed");
        ps->trayPaperTrayStatus = 1 ;break;
    case 0xfe:
//        LOGLOG("paper tray status: out of page");
        ps->trayPaperTrayStatus = 2 ;break;
    case 0:
    default:
//        LOGLOG("paper tray status: ready");
        ps->trayPaperTrayStatus = 0 ;break;
    }
//    switch(pStatus->job){
//    case 0:            LOGLOG("executing job: unknow job");            break;
//    case 1:            LOGLOG("executing job: print job");            break;
//    case 2:            LOGLOG("executing job: normal copy job");            break;
//    case 3:            LOGLOG("executing job: scan job");            break;
//    case 4:            LOGLOG("executing job: fax job");            break;
//    case 5:            LOGLOG("executing job: ifax job");            break;
//    case 6:            LOGLOG("executing job: report job");            break;
//    case 7:            LOGLOG("executing job: n in 1 copy job");            break;
//    case 8:            LOGLOG("executing job: id card copy job");            break;
//    case 9:            LOGLOG("executing job: id card copy mode");            break;
//    default:            LOGLOG("executing job: print job");            break;
//    }

//            QString owner_name;
//            owner_name.setRawData((const QChar*)pStatus->OwnerName ,16);
//            LOGLOG("owner name:" + owner_name);
//            QString docu_name;
//            docu_name.setRawData((const QChar*)pStatus->DocuName ,16);
//            LOGLOG("docu name:" + docu_name);
    ps->ErrorCodeGroup = pStatus->ErrorCodeGroup;
    ps->ErrorCodeID = pStatus->ErrorCodeID;
    ps->PaperSize = pStatus->PaperSize;
    ps->PaperType = pStatus->PaperType;
    ps->LowTonerAlert = pStatus->LowTonerAlert;
    ps->NonDellTonerMode = pStatus->NonDellTonerMode;
    ps->job = pStatus->bPhyPrinterStatus;
    return 0;
}

StatusMonitor::StatusMonitor()
{
}

int StatusMonitor::getPrinterStatus(const char* printer ,PrinterStatus_struct* ps)
{
    PRINTER_STATUS ps_static;
    StatusManager statusmanager;
    int ret = statusmanager.getStatusFromFile(printer ,&ps_static);
    if(!ret){
        ret = parsePrinterStatus(&ps_static ,ps);
    }
    return ret;
}

//int StatusMonitor::getDeviceStatus(DeviceIO* device ,PrinterStatus_struct* ps)
//{
//    PRINTER_STATUS status;
//    int ret = getStatusFromDevice(device ,&status);
//    if(ret){
//        memset(&status ,-1 ,sizeof(status));
////        status.PrinterStatus = PS_ERROR_POWER_OFF;
//        status.PrinterStatus = PS_UNKNOWN;
//    }else{
////        if(IsStatusAbnormal(status.PrinterStatus)){
////            status.PrinterStatus = PS_OFFLINE;
////            status.PrinterStatus = PS_PAUSED;
////        }
//    }
//    parsePrinterStatus(&status ,ps);
//    return 0;
////    return ret;
//}

int StatusMonitor::getDeviceStatus(DeviceIO* device ,PrinterInfo_struct* printer)
{
    return getDeviceStatus(device ,&printer->printer ,&printer->status);
}

int StatusMonitor::getDeviceStatus(DeviceIO* device ,Printer_struct* printer ,PrinterStatus_struct* ps)
{
    if(!device || !printer || !ps){
        return -1;
    }
    int ret = -1;
    PRINTER_STATUS status;
//    if(device->isConnected(printer) || (device->type() == DeviceIO::Type_usb)){
        ret = getStatusFromDevice(device ,printer ,&status);
//    }
    if(ret){
        memset(&status ,-1 ,sizeof(status));
    }
    parsePrinterStatus(&status ,ps);
    return ret;
}

int StatusMonitor::getDeviceStatus(DeviceIOManager* device_manager ,Printer_struct* printer ,PrinterStatus_struct* ps)
{
    if(!device_manager || !printer || !ps){
        return -1;
    }
    DeviceIO* device = device_manager->getDevice(printer);
    return getDeviceStatus(device ,printer ,ps);
}

//struct PrinterList_para_struct
//{
//    CALLBACK_getPrinterInfo callback;
//    void* para;
//};

//static int getPrinterList(void* para ,Printer_struct* ps)
//{
//    int ret = 1;
//    if(isDeviceSupported && isDeviceSupported(ps)){

//        struct PrinterList_para_struct* pl_para = (struct PrinterList_para_struct*) para;

//        struct PrinterInfo_struct pis;
//        pis.printer = *ps;
//        memset((void*)&pis.status ,-1 ,sizeof(pis.status));
//    //    if(!StatusMonitor::getPrinterStatus(ps->name ,&pis.status)){
//    //        LOGLOG("can not get printer status");
//    //    }

//        if(pl_para->callback){
//            ret = pl_para->callback(pl_para->para ,&pis);
//        }
//    }
//    return ret;
//}

//int StatusMonitor::getPrintersFromFile(CALLBACK_getPrinterInfo callback,void* para)
//{
//    struct PrinterList_para_struct pl_para;
//    pl_para.callback = callback;
//    pl_para.para = para;
//    return StatusManager().getPrintersFromFile(getPrinterList ,(void*)&pl_para);
//}

//int StatusMonitor::getPrinters(CALLBACK_getPrinterInfo callback,void* para)
//{
//    struct PrinterList_para_struct pl_para;
//    pl_para.callback = callback;
//    pl_para.para = para;
//    return cups_get_printers(getPrinterList ,(void*)&pl_para);
//}

struct PrinterList_para_struct
{
    CALLBACK_getPrinters callback;
    void* para;
};

static int getPrinterList(void* para ,Printer_struct* ps)
{
    int ret = 1;
    if(isDeviceSupported(ps)){

        struct PrinterList_para_struct* pl_para = (struct PrinterList_para_struct*) para;

        if(pl_para->callback){
            ret = pl_para->callback(pl_para->para ,ps);
        }
    }
    return ret;
}

int StatusMonitor::getPrinters(CALLBACK_getPrinters callback,void* para)
{
    struct PrinterList_para_struct pl_para;
    pl_para.callback = callback;
    pl_para.para = para;
    return cups_get_printers(getPrinterList ,(void*)&pl_para);
}

int StatusMonitor::getPrintersFromFile(CALLBACK_getPrinters callback,void* para)
{
    struct PrinterList_para_struct pl_para;
    pl_para.callback = callback;
    pl_para.para = para;
    return StatusManager().getPrintersFromFile(getPrinterList ,(void*)&pl_para);
}

