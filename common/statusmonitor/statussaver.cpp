#include "statussaver.h"
#include "statusmanager.h"
#include "devicemanager.h"
#include "status.h"
#include "commonapi.h"
#include "smconfig.h"
extern int usb_error_printing;

void StatusObject::update_status(QList<Printer_struct> printers)
{
    if(printers.isEmpty())
        return;
    PrinterInfo_struct printerinfo;
    int result;
    DeviceIO* device;
    bool printing = false;

    device = DeviceManager::new_device(&printers[0]);
    result = -1;
    printerinfo.printer = printers[0];
    if(device){
#if 0
        if(device->type() ==DeviceIO::Type_usb){
            foreach (Printer_struct printer, printers) {
                if(printer_is_printing(printer.name)){
                    result = usb_error_printing;
                    printing = true;
                    break;
                }
            }
        }
        if(!printing){
            result = StatusMonitor::getDeviceStatus(device ,&printerinfo);
        }
#else
        result = StatusMonitor::getDeviceStatus(device ,&printerinfo);
        if(result == usb_error_printing)
            printing = true;
#endif
        delete device;
    }
    if(printing){
        LOGLOG("device %s is printing ,update status via filter" ,printerinfo.printer.deviceUri);
        return;//get status via filter
    }
    foreach (Printer_struct printer, printers) {
        LOGLOG("update %s result:%d ,status:%d ,url:%s" ,printer.name ,result
               ,printerinfo.status.PrinterStatus
               ,printer.deviceUri);
        printerinfo.printer = printer;
        printerinfo.printer.isConnected = !result;
        printerinfo.printer.status = result;
        StatusManager().savePrinterInfoToFile(printer.name ,&printerinfo);
    }
}

SingelStatusThread::SingelStatusThread(QObject *parent)
    : QThread(parent)
{
    qRegisterMetaType<QList<Printer_struct> >("QList<Printer_struct>");
    connect(this ,SIGNAL(update_status(QList<Printer_struct>)) ,&statusObject ,SLOT(update_status(QList<Printer_struct>)));
    statusObject.moveToThread(this);
}

void SingelStatusThread::get_status()
{
    QList<Printer_struct> printer_list;
    mutex.lock();
    printer_list = printerlist;
    mutex.unlock();
    if(!printer_list.isEmpty())
        update_status(printer_list);
}

bool SingelStatusThread::update_printerlist(QList<Printer_struct> printer_list)
{
    bool same = false;
    QString url = printer_list[0].deviceUri;
    mutex.lock();
    if(!url.compare(printerlist[0].deviceUri)){
        same = true;
        printerlist = printer_list;
    }
    mutex.unlock();
    return same;
}
StatusSaverThread::StatusSaverThread(QObject *parent)
    : QThread(parent)
    , abort(false)
    ,delay_start(false)
{
    StatusManager().clearFile();
}

StatusSaverThread::~StatusSaverThread()
{
    abort = true;
    while(abort)usleep(1000);
}

void StatusSaverThread::run()
{
    StatusSaver* monitor = qobject_cast<StatusSaver* >(parent());
    QList<Printer_struct> printers;
    PrinterInfo_struct printerinfo;
    int result;
//    PRINTER_STATUS status;
    DeviceIO* device;

    if(!monitor){
        return;
    }
    if(delay_start)
        sleep(6);
    forever {
        if (abort)
            break;

        monitor->mutex.lock();
        printers = monitor->current_printerlist;
        monitor->mutex.unlock();

        for(int i = 0 ;i < printers.count() ;i++) {
            if (abort){
                break;
            }
//            if(!printers[i].isConnected)
//                continue;
            device = DeviceManager::new_device(&printers[i]);
            result = -1;
            printerinfo.printer = printers[i];
            if(device){
//                result = getStatusFromDevice(device ,&printers[i] ,&status);
                result = StatusMonitor::getDeviceStatus(device ,&printerinfo);
                if((result == usb_error_printing)
                        && (device->type() ==DeviceIO::Type_usb)){
                    delete device;
                    continue;
                }
                delete device;
            }
            printerinfo.printer.isConnected = !result;
            printerinfo.printer.status = result;
            StatusManager().savePrinterInfoToFile(printerinfo.printer.name ,&printerinfo);
//            LOGLOG("update printer %s, status:0x%02x" ,printerinfo.printer.name ,printerinfo.status.PrinterStatus);
        }
        sleep(6);
    }
    StatusManager().clearFile();
    abort = false;
}

static int callback_getPrinters(void* para,Printer_struct* ps)
{
    StatusSaver* watcher = (StatusSaver*)para;
    if(isDeviceSupported && isDeviceSupported(ps)){
        watcher->add_printer(ps);
    }
    return watcher->abort ?0 :1;
}

StatusSaver::StatusSaver(QObject *parent)
    : QThread(parent)
    ,abort(false)
    ,count(0)
{
//    statusThread = new StatusSaverThread(this);
}

StatusSaver::~StatusSaver()
{
    abort = true;
//    if(statusThread)
//        delete statusThread;
    foreach (SingelStatusThread* thread, threads) {
        thread->quit();
        thread->wait();
        delete thread;
    }
    while(abort)usleep(1000);
}

void StatusSaver::add_printer(Printer_struct* ps)
{
    printerlist << *ps;
}

void StatusSaver::run()
{
//    statusThread->start();
    forever{
        if (abort){
            break;
        }
        //update printer list
        if(count % 6 == 0){
            printerlist.clear();
            cups_get_printers(callback_getPrinters ,this);
//            for(int i = 0 ;i < printerlist.count() ;i++) {
//                if (abort){
//                    break;
//                }
//                if(DeviceManager::get_device_connect_status(&printerlist[i])){

//                }
//            }
            mutex.lock();
            current_printerlist = printerlist;
            mutex.unlock();
            StatusManager().savePrintersToFile(printerlist);

            //get the new printers list
            QList<QList<Printer_struct > > printers_list;
            bool same_url;
            QString str;
            for(int i = 0 ;i < printerlist.count() ;i++) {
                same_url = false;
                for(int j = 0 ;j <printers_list.count() ;j++){
                    if(!printers_list[j].isEmpty()){
                        str = printers_list[j][0].deviceUri;
                        if(!str.compare(printerlist[i].deviceUri)){
                            printers_list[j] << printerlist[i];
                            same_url = true;
                            break;
                        }
                    }
                }

                if(!same_url){
                    QList<Printer_struct > printers;
                    printers << printerlist[i];
                    printers_list << printers;
                }
            }

            SingelStatusThread* thread;
#if 1
            QList<SingelStatusThread* > new_threads;
            QList<SingelStatusThread* > rest_threads = threads;
            //find the same url and update printer list of current threads
            for(int i = 0 ;i < threads.count() ;i++){
                thread = threads[i];
                for(int j = 0 ;j <printers_list.count() ;j++){
                    if(thread->update_printerlist(printers_list[j])){
                        printers_list.removeAt(j);
                        rest_threads.removeOne(thread);
                        new_threads << thread;
                        break;
                    }
                }
            }
            //create threads of rest new printer list
            foreach(QList<Printer_struct > printer_list ,printers_list){
                thread = new SingelStatusThread;
                thread->printerlist = printer_list;
                new_threads << thread;
                thread->start();
            }
            //get status of all new printer list
            threads = new_threads;
            foreach (SingelStatusThread* thread , new_threads) {
                thread->get_status();
            }
            //delete rest threads
            foreach (SingelStatusThread* thread, rest_threads) {
                thread->quit();
                thread->wait();
                thread->deleteLater();
            }
#else
            int num = printers_list.count() - threads.count();
            if(num > 0){
                for(int i = 0 ;i < num ;i++){
                    thread = new SingelStatusThread;
                    threads << thread;
                    thread->start();
                }
            }

//            QList<SingelStatusThread* > tmp_threads = threads;
            for(int i = 0 ;i < threads.count() ;i++){
                thread = threads[i];
                if(i < printers_list.count()){
                    thread->printerlist = printers_list[i];
                    thread->get_status();
                }else{
                    thread->quit();
                    thread->wait();
                    thread->deleteLater();
                    tmp_threads.removeOne(thread);
                }
            }
            threads = tmp_threads;
#endif
        }

        watcher_job();
        if(60 == count++)
            count = 0;
        usleep(1000*1000);
    }
    abort = false;
}

void StatusSaver::watcher_job()
{
}

int parsePrinterStatus(PRINTER_STATUS* pStatus ,PrinterStatus_struct* ps);
void StatusSaver::set_device_id(const QString& printer ,const QString& device_id)
{
//    QMutexLocker locker(&mutex);
    PrinterInfo_struct printerinfo;
    PRINTER_STATUS printer_status;
    if(!DecodeStatusFromDeviceID(device_id.toLatin1().data() ,&printer_status)){
        parsePrinterStatus(&printer_status ,&printerinfo.status);
        foreach(Printer_struct ps ,printerlist){
            if(!printer.compare(ps.name)){
                printerinfo.printer = ps;
                break;
            }
        }
        printerinfo.printer.isConnected = true;
        printerinfo.printer.status = 0;

        StatusManager().savePrinterInfoToFile(printer.toLatin1().constData() ,&printerinfo);
        LOGLOG("filter update %s result:%d ,status:%d" ,printer.toLatin1().constData()
               ,0 ,printerinfo.status.PrinterStatus);

    }
}
