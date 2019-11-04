#ifndef STATUSSAVER_H
#define STATUSSAVER_H

#include <QThread>
#include "statusmonitor.h"
#include "devicemanager.h"
#include <QMutex>
#include <QStringList>
class DeviceManager;

class StatusObject : public QObject
{
    Q_OBJECT
public:
    explicit StatusObject(QObject *parent = NULL)
        : QObject(parent)
    {}
    ~StatusObject(){}
public slots:
    void update_status(QList<Printer_struct> printerlist);
};

class SingelStatusThread : public QThread
{
    Q_OBJECT
public:
    explicit SingelStatusThread(QObject *parent = NULL);
    void get_status();

signals:
    void update_status(QList<Printer_struct> printerlist);

private:
    StatusObject statusObject;
    QList<Printer_struct> printerlist;

    friend class StatusSaver;
};

class StatusSaverThread : public QThread
{
    Q_OBJECT
public:
    explicit StatusSaverThread(QObject *parent = NULL);
    ~StatusSaverThread();

    void run();

private:
    bool abort;
    bool delay_start;

    friend class StatusSaver;
};

class StatusSaver : public QThread
{
    Q_OBJECT
public:
    explicit StatusSaver(QObject *parent = 0);
    ~StatusSaver();
    void run();

    void add_printer(Printer_struct*);

    bool abort;
protected:
    virtual void watcher_job();

public slots:
    void set_device_id(const QString& printer ,const QString& device_id);

protected:
    QMutex mutex;
    int count;

private:
//    StatusSaverThread* statusThread;

    QList<Printer_struct> printerlist;
//    QList<PrinterInfo_struct> printerinfo_list;

    QList<Printer_struct> current_printerlist;
//    QList<PrinterInfo_struct> current_printerinfo_list;

    QList<SingelStatusThread* > threads;

    friend class SingelStatusThread;
    friend class StatusSaverThread;
};

#endif // STATUSSAVER_H
