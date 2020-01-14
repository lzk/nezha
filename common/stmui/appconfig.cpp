#include "appconfig.h"
#include "commonapi.h"
#include "filelocker.h"
#include "appserver.h"
#include <QFile>
#include <QDir>
#include "lld.h"
#include <unistd.h>

bool testmode = false;
//const QString app_name = QString::fromUtf8("打印机状态监视器");
FileLocker app_file_locker;
AppServer* app_server;

AppConfig::AppConfig(QObject *parent) :
    QObject(parent)
{
}

//const char* log_file;
//const char* log_app_name;
int AppConfig::initConfig()
{
//    log_app_name = EXE_NAME;
////    app_version = APP_VERSION;
//    log_file = LOG_FILE_NAME;
//    log_init();

//    const char* app_locker_file = LOCKER_UI;
    if(app_file_locker.trylock(LOCKER_UI)){
        LOGLOG("%s had been locked!" ,EXE_NAME);
        return -1;
    }
//    if(is_app_running(DOMAIN_UIEXE)){
//        LOGLOG("socket working!");
//        return -2;
//    }

    LOGLOG("--------%s - %d - v%s-------" ,EXE_NAME ,getpid() ,app_version);

    app_server = new AppServer(DOMAIN_UIEXE);
    return 0;
}

void AppConfig::exit_app()
{
//    QFile::remove(filepath);
//    QFile::remove(lockfile);
    app_file_locker.unlock();
    delete app_server;
}
