#ifndef STATUSREADER_H
#define STATUSREADER_H

#include <QThread>
#include "statusmonitor.h"
#include "devicemanager.h"
#include <QMutex>
#include <QStringList>
class StatusReader : public QThread
{
    Q_OBJECT
public:
    explicit StatusReader(QObject *parent = 0);
    ~StatusReader();
    void run();
    void setPrinters(Printer_struct* ps);

    int get_printer_list(QList<Printer_struct>&);
    int get_printer_from_current_list(const QString& printer_name ,Printer_struct&);
protected:
    virtual void watcher_job();

protected:
    bool abort;
    QMutex mutex;
    int count;

protected:
    QList<Printer_struct> printerlist;
    QList<Printer_struct> current_printerlist;

private:
    void get_printer_list();
    int printer_compare(const Printer_struct* ,const Printer_struct*);
    int printerlist_compare(QList<Printer_struct> & ,QList<Printer_struct> &);

    friend class WatcherStatusThread;
};

class CurrentStatusReader : public StatusReader
{
    Q_OBJECT
public:
    explicit CurrentStatusReader(QObject *parent = 0);
    ~CurrentStatusReader();

//    int get_printer_info(const QString& ,PrinterInfo_struct &);
//    int get_printerinfo_list(QList<PrinterInfo_struct>&);
    int get_currentprinter_info(PrinterInfo_struct&);

protected:
    virtual void watcher_job();
    virtual void get_printer_info_else(PrinterInfo_struct* pis);

signals:
//    void update_printer_status(const QString&);
    void update_current_printer_status();
    void update_printerlist();

public slots:
    void set_current_printer(const QString& printer);


private:
    void getPrinters();
    void get_printerlist_from_file();

private:

//    QStringList printers;
//    QList<PrinterInfo_struct> printerinfo_list;

    QString current_printer;
    QStringList current_printers;
    PrinterInfo_struct current_printer_info;
//    QList<PrinterInfo_struct> current_printerinfo_list;

    void get_printer_status();
    void set_current_printer_info(PrinterInfo_struct* pis);
};

#endif // STATUSREADER_H
