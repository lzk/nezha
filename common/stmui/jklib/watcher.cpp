#include "watcher.h"
#include "lshell.h"
#include "statusmanager.h"
Watcher::Watcher(QObject *parent)
    : StatusReader(parent)
{
    PrinterInfo_struct pis;
    QList<PrinterInfo_struct> printerinfo_list;
    for(int i = 0 ;i < printerlist.count() ;i++) {
        pis.printer = printerlist[i];
        pis.printer.status = -1;
        pis.printer.toner = -1;
        pis.printer.drum = -1;
        printerinfo_list << pis;
    }
    mutex.lock();
    this->printerinfo_list = printerinfo_list;
    mutex.unlock();
}

Watcher::~Watcher()
{
}

void Watcher::watcher_job()
{
    //get drum of printer
    if((count % 6)){
        return;
    }
    int ret;
    PrinterInfo_struct pis;
//    DeviceManager dm ,*device_manager = &dm;
//    LShell lshell(device_manager);
//    Consumabale_supply_info para;

    QList<PrinterInfo_struct> printerinfo_list;
    for(int i = 0 ;i < printerlist.count() ;i++) {
        pis.printer = printerlist[i];
        if(pis.printer.name[0]){
            ret = StatusManager().getPrinterInfoFromFile(pis.printer.name ,&pis);
    //            StatusMonitor::getPrinterStatus(pis.printer.name ,&pis.status);

            pis.printer.toner = -1;
            pis.printer.drum = -1;
            if(ret){
                pis.printer.status = -1;
                printerlist[i] = pis.printer;
                printerinfo_list << pis;
                continue;
            }

            if(pis.printer.isConnected){
                pis.printer.status = pis.status.PrinterStatus;
                pis.printer.toner = pis.status.TonelStatusLevelK;
                pis.printer.drum = pis.status.drum;
#ifndef DEBUG_DEBUG
//                if(QString(pis.printer.deviceUri).startsWith("usb://")){
//                    ret = lshell.open(&pis.printer);
//                    if(!ret){
//                        ret = lshell.get_consumable_supply_info(&para);
//                        lshell.close();
//                    }
//                    if(!ret){
//                        pis.printer.toner = para.RemainingTonerCounter;
//                        pis.printer.drum = para.RemainingOPCCounter;
//                    }
//                }
#endif
            }

            printerlist[i] = pis.printer;
            printerinfo_list << pis;
        }
    }
    mutex.lock();
    this->printerinfo_list = printerinfo_list;
    mutex.unlock();
    update_printerinfo_list();
}

int Watcher::get_printerinfo_list(QList<PrinterInfo_struct>& ps)
{
    QMutexLocker locker(&mutex);
    ps = printerinfo_list;
    return 0;
}
