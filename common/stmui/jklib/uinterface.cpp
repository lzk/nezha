#include "uinterface.h"
#include "worker.h"
#include "uiconfig.h"
UInterface::UInterface(QObject *parent) :
    QObject(parent)
{
    worker = new Worker();
    worker->moveToThread(&thread);
    connect(&thread ,SIGNAL(finished()) ,worker ,SLOT(deleteLater()));

    connect(this ,SIGNAL(cmdToWorker(int,QString ,QVariant)) ,worker ,SLOT(cmdFromUi(int,QString ,QVariant)));
    connect(worker ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SIGNAL(cmdResult(int,int,QVariant)));

    thread.start();

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


