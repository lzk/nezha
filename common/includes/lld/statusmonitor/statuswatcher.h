#ifndef STATUSWATCHER_H
#define STATUSWATCHER_H

#include <QThread>
#include "statusmonitor.h"
#include "devicemanager.h"
#include <QMutex>
#include <QStringList>
class DeviceManager;
class WatcherStatusThread : public QThread
{
    Q_OBJECT
public:
    explicit WatcherStatusThread(const QString& printer ,QObject *parent = NULL);
    ~WatcherStatusThread();

    void run();
    void work(PrinterInfo_struct* printer);

private:
    bool abort;
    DeviceIO* device;
    QString current_printer;
    bool delay_start;

    friend class StatusWatcher;
};

class StatusWatcher : public QThread
{
    Q_OBJECT
public:
    explicit StatusWatcher(QObject *parent = 0);
    ~StatusWatcher();
    void run();

    int get_printer_list(QList<Printer_struct>&);
//    int get_printer_info(const QString& ,PrinterInfo_struct &);
//    int get_printerinfo_list(QList<PrinterInfo_struct>&);
    int get_currentprinter_info(PrinterInfo_struct&);
    int get_printer_from_current_list(const QString& printer_name ,Printer_struct&);
    void setPrinters(Printer_struct* ps);

signals:
//    void update_printer_status(const QString&);
    void update_current_printer_status();
    void update_printerlist();

public slots:
    void set_current_printer(const QString& printer);
    void set_device_id(const QString& printer ,const QString& device_id);

protected:
    virtual void watcher_job();

private:
    void getPrinters();
    void get_printerlist_from_file();
    int printerlist_compare(QList<Printer_struct> & ,QList<Printer_struct> &);
    int printer_compare(const Printer_struct* ,const Printer_struct*);

protected:
    bool abort;
    QMutex mutex;
    int count;

private:
    WatcherStatusThread* statusThread;
    DeviceManager* device_manager;

//    QStringList printers;
    QList<Printer_struct> printerlist;
//    QList<PrinterInfo_struct> printerinfo_list;

    QString current_printer;
    QStringList current_printers;
    PrinterInfo_struct current_printer_info;
    QList<Printer_struct> current_printerlist;
//    QList<PrinterInfo_struct> current_printerinfo_list;

    void get_printer_list();
    void get_printer_status();
    void set_current_printer_info(PrinterInfo_struct* pis);
    friend class WatcherStatusThread;
};

#endif // STATUSWATCHER_H
