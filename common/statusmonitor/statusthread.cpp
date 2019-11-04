#include "statusthread.h"
#include "commonapi.h"
#include "status.h"
#include "smconfig.h"
#include "statusmonitor.h"

static int callback_getPrinters(void* para,Printer_struct* ps)
{
    StatusThread* st = static_cast<StatusThread*>(para);
    if(isDeviceSupported && isDeviceSupported(ps)){
        st->printers << *ps;
        st->printerlist << ps->name;
    }
    return st->abort ?0 :1;
}

StatusThread::StatusThread(QObject *parent)
    : QThread(parent)
    ,b_refresh_connect_to(false)
    ,locker_get_status(false)
    ,devicemanager(new DeviceManager)
{
    abort = false;
}

StatusThread::~StatusThread()
{
    while(abort)usleep(1000);
    delete devicemanager;
}

void StatusThread::set_abort()
{
    abort = true;
}

void StatusThread::run()
{
    PRINTER_STATUS printer_status;
    int result;
    forever {
        if (abort){
            break;
        }
        printers.clear();
        printerlist.clear();
//        statusmanager.clearPrintersOfFile();
        cupsmanager.getPrinters(callback_getPrinters ,this);
//        if(b_refresh_connect_to){
//            Printer_struct* printer;
//            DeviceIO* dio;
//            for(int i = 0 ;i < printers.count() ;i++){
//                printer = &printers[i];
//                dio = devicemanager->getDevice(printer);
//                if(dio)
//                    strcpy(printer->connectTo
//                       ,dio->getDeviceAddress(printer));
//            }
//        }
//        statusmanager.savePrintersToFile(printers);

        foreach (Printer_struct printer, printers) {
            if (abort){
                break;
            }
            mutex.lock();
            if(current_printer.isEmpty()){
                mutex.unlock();
                break;
            }
            if(current_printer.compare(printer.name)){
//            if(!current_printer.isEmpty() && current_printer.compare(printer.name)){
                mutex.unlock();
                continue;
            }
            mutex.unlock();
            result = getStatusFromDevice(devicemanager ,&printer ,&printer_status);
            if (abort){
                break;
            }
            mutex.lock();
            if(result){
                LOGLOG("get status from device %s:fail!result:0x%02x" ,printer.name ,result);
                status.PrinterStatus = result;
            }else{
                status = printer_status;
            }
            statusmanager.saveStatusToFile(printer.name ,&status);
            locker_get_status = false;
            mutex.unlock();
        }
        sleep(6);
    }
    abort = false;
}

void StatusThread::set_current_printer(const QString& printer)
{
    QMutexLocker locker(&mutex);
    current_printer = printer;
    memset(&status ,0xff ,sizeof(status));
    locker_get_status = true;
}

bool StatusThread::is_locked_get_status()
{
    QMutexLocker locker(&mutex);
    return locker_get_status;
}

