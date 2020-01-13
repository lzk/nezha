#include "appconfig.h"
#include "appserver.h"
//#include "watcher.h"
#include <unistd.h>
#include "lld.h"

//const QString app_name = QString::fromUtf8("打印机状态监视器");
FileLocker app_file_locker;
AppServer* app_server;
//Watcher* watcher;
AppConfig::AppConfig(QObject *parent) :
    QObject(parent)
{
}

//extern const char* log_file;
int AppConfig::initConfig()
{
//    log_app_name = EXE_NAME;
//    app_version = APP_VERSION;
//    log_file = LOG_FILE_NAME;
//    log_init();

    if(app_file_locker.trylock(LOCKER_EXE)){
        LOGLOG("%s had been locked!" ,EXE_NAME);
        return -1;
    }
//    if(is_app_running(SERVER_PATH)){
//        LOGLOG("socket working!");
//        return -2;
//    }

    LOGLOG("--------%s - %d - v%s-------" ,EXE_NAME ,getpid() ,app_version);
#ifndef DEBUG_DEBUG
    //release as deaemon
    int result = daemon(0 ,0);
    if(!result){
        LOGLOG("daemon success!");
    }
#endif

//    ui_server_path = SERVER_PATH;
    app_server = new AppServer(SERVER_PATH);

//    watcher = new Watcher;
//    watcher->start();

    return 0;
}

#include <QFile>
void AppConfig::exit_app()
{
//    QFile::remove(status_file);
//    QFile::remove(status_lock_file);

//    delete watcher;
    delete app_server;
    app_file_locker.unlock();
}
