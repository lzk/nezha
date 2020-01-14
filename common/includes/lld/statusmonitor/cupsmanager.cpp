#include "cupsmanager.h"

CupsManager::CupsManager()
{

}

struct PrinterList_para_struct
{
    CALLBACK_getPrinter callback;
    void* para;
};

static int getPrinterList(void* para ,Printer_struct* ps)
{
    int ret = 1;
    struct PrinterList_para_struct* pl_para = (struct PrinterList_para_struct*) para;

    if(pl_para->callback){
        ret = pl_para->callback(pl_para->para ,ps);
    }
    return ret;
}

int CupsManager::getPrinters(CALLBACK_getPrinter callback,void* para)
{
    struct PrinterList_para_struct pl_para;
    pl_para.callback = callback;
    pl_para.para = para;
    return cups_get_printers(getPrinterList ,(void*)&pl_para);
//    return cups_get_printers(callback ,para);
}

int CupsManager::getJobs(CALLBACK_getJob callback,void* para,const char* printer ,const char* which ,const char* users)
{
//    return cups_get_printers(getPrinterList ,(void*)this);
    return cups_get_job(callback ,para ,printer ,which ,users);
}

const char* CupsManager::getDefaultPrinter()
{
    return cups_get_default_printer();
}
