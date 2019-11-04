#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include "statusmonitor.h"
#include "serverthread.h"
#include "devicemanager.h"
#include "watcher.h"
class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);
    ~Worker();
    void cancel();
    
signals:
    void cmdResult(int cmd,int result ,QVariant data=QVariant());
    
public slots:
    void cmdFromUi(int cmd ,const QString& printer_name = QString() ,QVariant data = QVariant());
    void update_printerlist();

private:
    int cmd_status;
    PrinterInfo_struct current_printer_info;
    Watcher* watcher;
};

#endif // WORKER_H
