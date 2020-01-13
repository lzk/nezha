#include "worker.h"
#include <QVariant>
#include "appconfig.h"
#include <QDebug>
#include <QString>
#include <QDir>
#include <QDateTime>
#include <unistd.h>
#include <QImage>
Worker::Worker(QObject *parent) :
    QObject(parent)
  ,cmd_status(0)
{
    watcher = new Watcher(this);
    connect(watcher ,SIGNAL(update_printerinfo_list()) ,this ,SLOT(update_printerlist()) ,Qt::DirectConnection);
    watcher->start();
}

Worker::~Worker()
{
    delete watcher;
}

void Worker::cmdFromUi(int cmd ,const QString& printer_name ,QVariant data)
{
    if(cmd_status)
        return;
    cmd_status = 1;

    Printer_struct* printer;
    Printer_struct ps;
    int index  = watcher->get_printer_from_current_list(printer_name ,ps);
    if(index<0){
        printer = NULL;
    }else{
        printer = &ps;
    }

    switch (cmd) {

    case UIConfig::CMD_GetPrinters:
        update_printerlist();
        break;

    default:
        break;
    }
    cmd_status = 0;
}

void Worker::update_printerlist()
{
    QList<PrinterInfo_struct> printerinfo_list;
    QVariant value;
    watcher->get_printerinfo_list(printerinfo_list);
    value.setValue(printerinfo_list);
    cmdResult(UIConfig::CMD_GetPrinters ,0 ,value);
}

void Worker::cancel()
{

}
