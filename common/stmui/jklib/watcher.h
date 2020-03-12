#ifndef WATCHER_H
#define WATCHER_H

#include "statusreader.h"
class Watcher : public StatusReader
{
    Q_OBJECT
public:
    explicit Watcher(QObject *parent = 0);
    ~Watcher();
    int get_printerinfo_list(QList<PrinterInfo_struct>&);

signals:
    void server_restart();
    void update_printerinfo_list();

protected:
    virtual void watcher_job();

private:
    QList<PrinterInfo_struct> printerinfo_list;
};

#endif // WATCHER_H
