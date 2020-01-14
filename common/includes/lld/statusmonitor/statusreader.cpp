#include "statusreader.h"
//#include "statusthread.h"
#include "statusmanager.h"
#include "devicemanager.h"
#include "status.h"
#include <QMutexLocker>
#include "commonapi.h"

static int callback_getPrinters(void* para,Printer_struct* ps)
{
    StatusReader* watcher = (StatusReader*)para;
    watcher->setPrinters(ps);
    return 1;
}

StatusReader::StatusReader(QObject *parent)
    : QThread(parent)
    ,abort(false)
    ,count(0)
{
    get_printer_list();
}

StatusReader::~StatusReader()
{
    abort = true;
    while(abort)usleep(1000);
}

void StatusReader::setPrinters(Printer_struct* ps)
{
    printerlist << *ps;
}

void StatusReader::get_printer_list()
{
    printerlist.clear();
    StatusMonitor::getPrintersFromFile(callback_getPrinters ,this);
#if 1
    mutex.lock();
    current_printerlist = printerlist;
    mutex.unlock();
#else
    mutex.lock();
    if(printerlist_compare(current_printerlist ,printerlist)){
        current_printerlist = printerlist;
    }
    mutex.unlock();
#endif
}

int StatusReader::printer_compare(const Printer_struct* pps1,const Printer_struct* pps2)
{
    int ret = 0;
    if(strcmp(pps1->connectTo ,pps2->connectTo)){
        ret = -1;
    }else if(strcmp(pps1->name ,pps2->name)){
        ret = -1;
    }else if(strcmp(pps1->deviceUri ,pps2->deviceUri)){
        ret = -1;
    }else if(strcmp(pps1->makeAndModel ,pps2->makeAndModel)){
        ret = -1;
    }else if(pps1->isConnected != pps2->isConnected){
        ret = -1;
    }else if(pps1->isDefault != pps2->isDefault){
        ret = -1;
    }else if(pps1->status != pps2->status){
        ret = -1;
    }
    return ret;
}

int StatusReader::printerlist_compare(QList<Printer_struct> & ps1,QList<Printer_struct> & ps2)
{
    if(ps1.count() != ps2.count())
        return -1;
    int ret = 0;
    const Printer_struct* pps1,*pps2;
    for(int i = 0 ;i < ps1.count() ;i++){
        pps1 = &ps1.at(i);
        pps2 = &ps2.at(i);
        ret = printer_compare(pps1 ,pps2);
        if(ret)
            break;
    }
    return ret;
}

void StatusReader::watcher_job()
{
}

void StatusReader::run()
{
    forever{
        if (abort){
            break;
        }
        if(count % 6 == 0)
            get_printer_list();
        watcher_job();
        if(60 == count++)
            count = 0;
        usleep(1000*1000);
    }
    abort = false;
}

int StatusReader::get_printer_from_current_list(const QString& printer_name ,Printer_struct& printer)
{
    int ret = -1;
    QMutexLocker locker(&mutex);

    memset(&printer ,0 ,sizeof(Printer_struct));

    Printer_struct* this_printer = NULL;
    for(int i = 0 ;i < current_printerlist.count() ;i++){
        if(!printer_name.compare(current_printerlist[i].name)){
            this_printer = &current_printerlist[i];
            ret = i;
            break;
        }
    }
    if(this_printer){
        printer = *this_printer;
    }
    return ret;
}

int StatusReader::get_printer_list(QList<Printer_struct>& pslist)
{
    QMutexLocker locker(&mutex);
    pslist = current_printerlist;
    return 0;
}

CurrentStatusReader::CurrentStatusReader(QObject *parent)
    : StatusReader(parent)
{
    memset(&current_printer_info.printer ,0 ,sizeof(Printer_struct));
    memset(&current_printer_info.status ,-1 ,sizeof(PrinterStatus_struct));
//    foreach (Printer_struct printer, printerlist) {
//        if(printer.isConnected){
//            current_printer_info.printer = printer;
//            current_printer = printer.name;
//        }
//    }
}

CurrentStatusReader::~CurrentStatusReader()
{
}

void CurrentStatusReader::watcher_job()
{
    PrinterInfo_struct pis;
    mutex.lock();
    pis = current_printer_info;
    mutex.unlock();
    if(pis.printer.name[0]){
        get_printer_info_else(&pis);
        StatusManager().getPrinterInfoFromFile(pis.printer.name ,&pis);
//            StatusMonitor::getPrinterStatus(pis.printer.name ,&pis.status);
        set_current_printer_info(&pis);
    }
}

void CurrentStatusReader::get_printer_info_else(PrinterInfo_struct*)
{

}

void CurrentStatusReader::get_printer_status()
{

}

void CurrentStatusReader::set_current_printer(const QString& printer)
{
    mutex.lock();
    if(!current_printer.compare(printer)){
        mutex.unlock();
        return;
    }
    mutex.unlock();
    PrinterInfo_struct printerinfo;
    int index = get_printer_from_current_list(printer ,printerinfo.printer);
    if(index < 0){
        LOGLOG("not valid printer");
        return;
    }
    LOGLOG("set current printer:%s " ,printer.toLatin1().constData());
    mutex.lock();
    current_printer = printer;
    current_printer_info.printer = printerinfo.printer;
//    memset(&current_printer_info.status ,-1 ,sizeof(PrinterStatus_struct));
    mutex.unlock();
//    update_current_printer_status();
}


void CurrentStatusReader::set_current_printer_info(PrinterInfo_struct* pis)
{
//    QMutexLocker locker(&mutex);
    mutex.lock();
//    LOGLOG("%s will update status" ,pis->printer.name);
    if(current_printer.compare(pis->printer.name)){
        mutex.unlock();
        return;
    }
#if 1
    current_printer_info = *pis;
    mutex.unlock();
    update_current_printer_status();
#else
    if(printer_compare(&pis->printer ,&current_printer_info.printer)){
//    if(pis->printer.status != current_printer_info.printer.status){
        current_printer_info = *pis;
        mutex.unlock();
        update_current_printer_status();
//        LOGLOG("%s update status" ,pis->printer.name);
        return;
    }
    PrinterStatus_struct* status = &pis->status;
    PrinterStatus_struct* p_current_printer_status = &current_printer_info.status;
    if(     (status->PrinterStatus != p_current_printer_status->PrinterStatus)
            ||(status->trayPaperTrayStatus != p_current_printer_status->trayPaperTrayStatus)
            ||(status->TonelStatusLevelC != p_current_printer_status->TonelStatusLevelC)
            ||(status->TonelStatusLevelM != p_current_printer_status->TonelStatusLevelM)
            ||(status->TonelStatusLevelY != p_current_printer_status->TonelStatusLevelY)
            ||(status->TonelStatusLevelK != p_current_printer_status->TonelStatusLevelK)
            ||(status->ErrorCodeGroup != p_current_printer_status->ErrorCodeGroup)
            ||(status->ErrorCodeID != p_current_printer_status->ErrorCodeID)
            ||(status->PaperSize != p_current_printer_status->PaperSize)
            ||(status->PaperType != p_current_printer_status->PaperType)
            ||(status->NonDellTonerMode != p_current_printer_status->NonDellTonerMode)
            ||(status->LowTonerAlert != p_current_printer_status->LowTonerAlert)
//                    ||(status.job != p_current_printer_status->job)
            ){
        current_printer_info = *pis;
        mutex.unlock();
        update_current_printer_status();
//        LOGLOG("%s update status" ,pis->printer.name);
    }else{
        mutex.unlock();
    }
#endif
}

//int CurrentStatusReader::get_printerinfo_list(QList<PrinterInfo_struct>& pslist)
//{
//    QMutexLocker locker(&mutex);
//    pslist = current_printerinfo_list;
//    return 0;
//}


int CurrentStatusReader::get_currentprinter_info(PrinterInfo_struct& ps)
{
    QMutexLocker locker(&mutex);
    ps = current_printer_info;
    return 0;
}

//int CurrentStatusReader::get_printer_info(const QString& printer_name ,PrinterInfo_struct & ps)
//{
//    Printer_struct printer;
//    int index = get_printer_from_current_list(printer_name ,printer);
//    if(index < 0){
//        strcpy(ps.printer.name ,printer_name.toLatin1().constData());
//        memset(&ps.status ,-1 ,sizeof(PrinterStatus_struct));
//        return -1;
//    }
//    QMutexLocker locker(&mutex);
//    ps = current_printerinfo_list[index];
//    return 0;
//}

