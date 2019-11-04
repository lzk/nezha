#include "uinterface.h"
#include "worker.h"
UInterface::UInterface(QObject *parent) :
    QObject(parent)
  ,timeval(0)
{
    worker = new Worker();
    worker->moveToThread(&thread);
    connect(&thread ,SIGNAL(finished()) ,worker ,SLOT(deleteLater()));

    connect(this ,SIGNAL(cmdToWorker(int,QString ,QVariant)) ,worker ,SLOT(cmdFromUi(int,QString ,QVariant)));
    connect(worker ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SIGNAL(cmdResult(int,int,QVariant)));
    connect(worker ,SIGNAL(signal_update_scan_progress(int)) ,this ,SIGNAL(signal_update_scan_progress(int)));
    connect(worker ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult_slot(int,int,QVariant)));

    thread.start();

    connect(&timer ,SIGNAL(timeout()) ,this ,SLOT(timerOut()));
    timer.setSingleShot(true);

}

UInterface::~UInterface()
{
    cancel_work();
    thread.quit();
    thread.wait();
}

void UInterface::setCmd(int cmd ,const QString& printer_name ,QVariant data)
{
    cmdToWorker(cmd ,printer_name ,data);
}

void UInterface::setCurrentPrinterCmd(int cmd ,QVariant data)
{
    setCmd(cmd ,current_printer ,data);
}

void UInterface::setTimer(int timeVal)
{
//    this->timeval = timeVal;
//    if(timeVal < 1)
//        timer.stop();
//    else{
//        timer.start(timeVal * 1000);
//    }
}

void UInterface::timerOut()
{
    cmdToWorker(UIConfig::CMD_GetStatus ,current_printer);
}

void UInterface::cmdResult_slot(int cmd,int result ,QVariant data)
{
    (void)result;
    (void)data;
    switch (cmd) {
    case UIConfig::CMD_GetStatus:
        if(timeval > 0){
            timer.start(timeval * 1000);
        }
        break;
    default:
        break;
    }
}

void UInterface::setcurrentPrinter(const QString& str)
{
    current_printer = str;
//    emit set_current_printer(current_printer);
    cmdToWorker(UIConfig::CMD_SetCurrentPrinter ,current_printer);
}

void UInterface::cancel_work()
{
    worker->cancel();
}

void UInterface::setDeviceMsgFrmUI(const QString &str, int result)
{
    emit setDeviceMsg(str,result);
}

void UInterface::emitStartScanSignal()
{
    emit startScan();
}

void UInterface::emitStopScanSignal()
{
    emit stopScan();
}

void UInterface::emitEnableCycleAnimation(bool enabled)
{
    emit signalEnabledCycleAnimation(enabled);
}


