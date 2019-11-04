#include "watcher.h"
#include "appserver.h"
#include "appconfig.h"
#include "commonapi.h"
extern AppServer* app_server;
Watcher::Watcher(QObject *parent)
    : StatusSaver(parent)
{
    connect(this ,SIGNAL(server_restart()) ,app_server ,SLOT(restart_server()));
    connect(app_server ,SIGNAL(signal_set_device_id(QString,QString)) ,this ,SLOT(set_device_id(QString,QString)));
}

Watcher::~Watcher()
{
}

void Watcher::watcher_job()
{
    static int count = 0;
    count ++;
    if(count == 100)
        count = 0;
    if((count % 10 == 0) && !is_app_running(SERVER_PATH)){
        server_restart();
    }
}
