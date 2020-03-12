#include "appconfig.h"
#include "commonapi.h"
#include "filelocker.h"
#include "appserver.h"
#include <QFile>
#include <QDir>
#include "lld.h"
#include <unistd.h>
#include <QDateTime>

bool testmode = false;
//const QString app_name = QString::fromUtf8("打印机状态监视器");
FileLocker app_file_locker;
AppServer* app_server;

QString AppConfig::tmp_scan_dir;
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

    if(app_file_locker.trylock(LOCKER_EXE)){
        LOGLOG("%s had been locked!" ,EXE_NAME);
        return -1;
    }
//    if(is_app_running(DOMAIN_UIEXE)){
//        LOGLOG("socket working!");
//        return -2;
//    }

    LOGLOG("--------%s - %d - v%s-------" ,EXE_NAME ,getpid() ,app_version);

    QDateTime time = QDateTime::currentDateTime();
    QString str_time = time.toString("yyyy-MM-dd_hh-mm-ss-zzz");
    tmp_scan_dir = QString("/tmp/tmpscan_") + str_time;
    QDir().mkdir(tmp_scan_dir);

//    QDir dir(TMP_SCAN_DIR);
//    QDir *path = &dir;
//    if(path->exists(TMP_SCAN_DIR)){
//        path->remove(TMP_SCAN_DIR);
//    }
//    path->mkdir(TMP_SCAN_DIR);

    app_server = new AppServer(DOMAIN_VOPEXE);
    return 0;
}

void AppConfig::exit_app()
{
//    QFile::remove(filepath);
//    QFile::remove(lockfile);
    app_file_locker.unlock();
    delete app_server;
//    QDir dir(TMP_SCAN_DIR);
//    QDir *path = &dir;
//    if(path->exists(TMP_SCAN_DIR)){
//        path->remove(TMP_SCAN_DIR);
//    }
    QDir().remove(tmp_scan_dir);
}
