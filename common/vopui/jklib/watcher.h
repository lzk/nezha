#ifndef WATCHER_H
#define WATCHER_H

#include <statusreader.h>
class Watcher : public CurrentStatusReader
{
    Q_OBJECT
public:
    explicit Watcher(QObject *parent = 0);
    ~Watcher();


signals:
    void server_restart();

protected:
    virtual void watcher_job();


private:

};

#endif // WATCHER_H
