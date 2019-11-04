#include "statuswatcher.h"
//#include "statusthread.h"
#include "statusmanager.h"
#include "devicemanager.h"
#include "status.h"
#include <QMutexLocker>
#include "commonapi.h"

WatcherStatusThread::WatcherStatusThread(const QString& printername ,QObject *parent)
    : QThread(parent)
    , abort(false)
    ,current_printer(printername)
    ,delay_start(false)
{
}

WatcherStatusThread::~WatcherStatusThread()
{
    abort = true;
//    LOGLOG("printer %s abort" ,current_printer.toLatin1().constData());
    while(abort)usleep(1000);
    if(device)
        delete device;
//    LOGLOG("printer %s exit" ,current_printer.toLatin1().constData());
}

void WatcherStatusThread::run()
{
//    LOGLOG("printer %s start" ,current_printer.toLatin1().constData());
    StatusWatcher* monitor = qobject_cast<StatusWatcher* >(parent());
    PrinterInfo_struct printerinfo;
    int index;

    if(delay_start)
        sleep(6);
    forever {
        if (abort)
            break;
        if(monitor){
            QString cp;
            monitor->mutex.lock();
            if(current_printer.compare(monitor->current_printer)){
                monitor->mutex.unlock();
                LOGLOG("current printer has changed to %s ,not %s"
                       ,monitor->current_printer.toLatin1().constData()
                       ,current_printer.toLatin1().constData());
                sleep(6);
                continue;
            }
            cp = current_printer;
            monitor->mutex.unlock();
            index = monitor->get_printer_from_current_list(cp ,printerinfo.printer);
            if(index >= 0){
                work(&printerinfo);
            }
        }
        sleep(6);
    }
    abort = false;
}

extern int usb_error_printing;
void WatcherStatusThread::work(PrinterInfo_struct* printerinfo)
{
    int result;
//    LOGLOG("printer %s get device status" ,printerinfo->printer.name);
    result = StatusMonitor::getDeviceStatus(device ,&printerinfo->printer ,&printerinfo->status);

    StatusWatcher* monitor = qobject_cast<StatusWatcher* >(parent());
    if(monitor){
        monitor->set_current_printer_info(printerinfo);
    }
}

bool use_status_thread = true;
static int callback_getPrinters(void* para,Printer_struct* ps)
{
    StatusWatcher* watcher = (StatusWatcher*)para;
    watcher->setPrinters(ps);
    return 1;
}

StatusWatcher::StatusWatcher(QObject *parent)
    : QThread(parent)
    ,abort(false)
    ,count(0)
    ,statusThread(NULL)
    ,device_manager(new DeviceManager)
{
    //init printer list
    get_printer_list();

    memset(&current_printer_info.printer ,0 ,sizeof(Printer_struct));
    memset(&current_printer_info.status ,-1 ,sizeof(PrinterStatus_struct));

}

StatusWatcher::~StatusWatcher()
{
    abort = true;
    if(statusThread)
        delete statusThread;
    while(abort)usleep(1000);
    delete device_manager;
}

//#include "lshell.h"
void StatusWatcher::get_printer_list()
{
//    printers.clear();
    printerlist.clear();
    if(use_status_thread){
        StatusMonitor::getPrinters(callback_getPrinters ,this);
#if 0
        for(int i = 0 ;i < printerlist.count() ;i++){
//            device_manager->get_device_connect_status(&printerlist[i]);

            DeviceIO* device = device_manager->new_device(&printerlist[i]);
            if(device){
                int ret = StatusMonitor::getDeviceStatus(device ,&printerlist[i]);
                printerlist[i].drum = -1;
                if(!ret){
                    //get drum of printer
                    LShell lshell(device_manager);
                    ret = lshell.open(&printerlist[i]);
                    if(!ret){
                        Consumabale_supply_info para;
                        ret = lshell.get_consumable_supply_info(&para);
                        if(!ret)
                            printerlist[i].drum = para.DrumCounter;
                        lshell.close();
                    }
                }
                delete device;
            }
            if (abort)
                return;
        }
#endif
    }else{
        StatusMonitor::getPrintersFromFile(callback_getPrinters ,this);
    }
    mutex.lock();
    if(printerlist_compare(current_printerlist ,printerlist)){
//        current_printers = printers;
        current_printerlist = printerlist;
//        current_printerinfo_list = printerinfo_list;
        mutex.unlock();
        emit update_printerlist();//not update auto,update when ui request
    }else
        mutex.unlock();
}

void StatusWatcher::get_printer_status()
{

}

void StatusWatcher::run()
{
    forever{
        if (abort){
            break;
        }
        //update printer list
        if(count % 6 == 0){
            get_printer_list();
        }
        if(!use_status_thread){
            PrinterInfo_struct pis;
            mutex.lock();
            pis= current_printer_info;
            mutex.unlock();
            StatusMonitor::getPrinterStatus(pis.printer.name ,&pis.status);
            pis.printer.status = 0;
            set_current_printer_info(&pis);
        }
        watcher_job();
        if(60 == count++)
            count = 0;
        usleep(1000*1000);
    }
    abort = false;
}

void StatusWatcher::set_current_printer(const QString& printer)
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
    memset(&current_printer_info.printer ,0 ,sizeof(Printer_struct));
//    memset(&current_printer_info.status ,-1 ,sizeof(PrinterStatus_struct));
    mutex.unlock();
//    update_current_printer_status();
#if 0
    if(printer.isEmpty()){
        if(statusThread){
            delete statusThread;
            statusThread = NULL;
        }
    }else if(use_status_thread){
        if(statusThread){
            //same uri ,not printer name
            if(statusThread->device && statusThread->device->is_the_same_device(&printerinfo.printer)){
                mutex.lock();
                statusThread->current_printer = printer;
                mutex.unlock();
//                LOGLOG("same device,only change printer name:%s" ,printer.toLatin1().constData());
                statusThread->work(&printerinfo);
                return;
            }
            statusThread->deleteLater();
        }
//        LOGLOG("%s will start"  ,printer.toLatin1().constData());
        statusThread = new WatcherStatusThread(printer ,this);
        statusThread->device = DeviceManager::new_device(&printerinfo.printer);
        if(statusThread->device){
            statusThread->work(&printerinfo);
            statusThread->delay_start = true;
            statusThread->start();
        }
    }
#endif
}

int StatusWatcher::printer_compare(const Printer_struct* pps1,const Printer_struct* pps2)
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
int StatusWatcher::printerlist_compare(QList<Printer_struct> & ps1,QList<Printer_struct> & ps2)
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

void StatusWatcher::watcher_job()
{
}

void StatusWatcher::set_current_printer_info(PrinterInfo_struct* pis)
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

void StatusWatcher::setPrinters(Printer_struct* ps)
{
//    printers << ps->name;
    printerlist << *ps;
//    printerinfo_list << *ps;
}

void StatusWatcher::getPrinters()
{
//    printers.clear();
    printerlist.clear();
//    printerinfo_list.clear();
    StatusMonitor::getPrinters(callback_getPrinters ,(void*)this);
}

void StatusWatcher::get_printerlist_from_file()
{
//    printers.clear();
    printerlist.clear();
//    printerinfo_list.clear();
//    StatusMonitor::getPrintersFromFile(callback_getPrinters ,(void*)this);
    if(statusThread){
//        printerlist = statusThread->printers;
    }else{
        StatusMonitor::getPrintersFromFile(callback_getPrinters ,(void*)this);
    }
//    foreach (Printer_struct printer, printerlist) {
//        printers << printer.name;
//    }
}

int StatusWatcher::get_printer_from_current_list(const QString& printer_name ,Printer_struct& printer_list)
{
    int ret = -1;
    QMutexLocker locker(&mutex);

    memset(&printer_list ,0 ,sizeof(Printer_struct));

    Printer_struct* this_printer_list = NULL;
    for(int i = 0 ;i < current_printerlist.count() ;i++){
        if(!printer_name.compare(current_printerlist[i].name)){
            this_printer_list = &current_printerlist[i];
            ret = i;
            break;
        }
    }
    if(this_printer_list){
        printer_list = *this_printer_list;
    }
    return ret;
}

//int StatusWatcher::get_printerinfo_list(QList<PrinterInfo_struct>& pslist)
//{
//    QMutexLocker locker(&mutex);
//    pslist = current_printerinfo_list;
//    return 0;
//}

int StatusWatcher::get_printer_list(QList<Printer_struct>& pslist)
{
    QMutexLocker locker(&mutex);
    pslist = current_printerlist;
    return 0;
}

int StatusWatcher::get_currentprinter_info(PrinterInfo_struct& ps)
{
    QMutexLocker locker(&mutex);
    ps = current_printer_info;
    return 0;
}

//int StatusWatcher::get_printer_info(const QString& printer_name ,PrinterInfo_struct & ps)
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

int parsePrinterStatus(PRINTER_STATUS* pStatus ,PrinterStatus_struct* ps);
void StatusWatcher::set_device_id(const QString& printer ,const QString& device_id)
{
    {
        QMutexLocker locker(&mutex);
        if(printer.compare(current_printer)){
            return;
        }
    }
    PrinterInfo_struct pis;
    int index = get_printer_from_current_list(printer ,pis.printer);
    if(index < 0){
        return;
    }
    pis.printer.status = usb_error_printing;
    PRINTER_STATUS status;
    if(!DecodeStatusFromDeviceID(device_id.toLatin1().data() ,&status)){
        parsePrinterStatus(&status ,&pis.status);
        set_current_printer_info(&pis);
//        statusmanager.saveStatusToFile(printer.toLatin1().constData() ,&printer_status);
    }
}
