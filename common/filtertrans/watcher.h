#ifndef WATCHER_H
#define WATCHER_H

#include "statussaver.h"
class Watcher : public StatusSaver
{
    Q_OBJECT
public:
    explicit Watcher(QObject *parent = 0);
    ~Watcher();
    void watcher_job();

signals:
    void server_restart();

};

#endif // WATCHER_H
