#include "appconfig.h"
#include "appserver.h"
#include "commonapi.h"
#include "watcher.h"
#include <unistd.h>

//const QString app_name = QString::fromUtf8("打印机状态监视器");
FileLocker app_file_locker;
AppServer* app_server;
Watcher* watcher;

extern
int (* getpidvid)(const QString& makeAndModel ,int& pid ,int& vid ,int& interface);
//log file var
extern const char* log_app_name;
extern const char* app_version;
//usb error control var
extern int usb_error_printing;
extern int usb_error_scanning;
extern int usb_error_usb_locked;
//scan control var
extern const char* lock_scan_file;
extern const char* lock_scan_info_file;

static bool _isDeviceSupported(Printer_struct* ps)
{
    QString makeAndModel(ps->makeAndModel);
    if(QString(ps->deviceUri).toLower().contains("://[fe80"))
        return false;
    if(0){
#ifdef DEBUG_DEBUG
    }else if(makeAndModel.startsWith("Lenovo L100DW")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo L100W")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo L100D")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo L100")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M100W")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M100D")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M100")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M101DW")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M101W")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M101")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M102W")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M102")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo M7268W")){
        return true;
#endif
    }else if(makeAndModel.startsWith("Lenovo Image LJ2310N")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo Image LJ2320DN")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo Image G262DN")){
        return true;
    }else if(makeAndModel.startsWith("Lenovo Image G336DN")){
        return true;
//    }else if(makeAndModel.startsWith("Lenovo Image GM265DN")){
//        return true;
//    }else if(makeAndModel.startsWith("Lenovo Image GM337DN")){
//        return true;
    }
    return false;
}

static int _getpidvid(const QString& makeAndModel ,int& pid ,int& vid ,int& interface)
{
    vid = 0x17ef;
    pid = -1;
    if(0){
#ifdef DEBUG_DEBUG
    }else if(makeAndModel.contains("L100DW")){
        pid = 0x5445;
        interface = 0;
    }else if(makeAndModel.contains("L100W")){
        pid = 0x5443;
        interface = 0;
    }else if(makeAndModel.contains("L100D")){
        pid = 0x5444;
        interface = 0;
    }else if(makeAndModel.contains("L100")){
        pid = 0x5442;
        interface = 0;
    }else if(makeAndModel.contains("M100W")){
        pid = 0x5641;
        interface = 1;
    }else if(makeAndModel.contains("M100D")){
        pid = 0x5644;
        interface = 1;
    }else if(makeAndModel.contains("M100")){
        pid = 0x563e;
        interface = 1;
    }else if(makeAndModel.contains("M101DW")){
        pid = 0x5645;
        interface = 1;
    }else if(makeAndModel.contains("M101W")){
        pid = 0x5642;
        interface = 1;
    }else if(makeAndModel.contains("M101")){
        pid = 0x563f;
        interface = 1;
    }else if(makeAndModel.contains("M102W")){
        pid = 0x5643;
        interface = 1;
    }else if(makeAndModel.contains("M102")){
        pid = 0x5640;
        interface = 1;
    }else if(makeAndModel.contains("M7268W")){
        pid = 0x563a;
        interface = 1;
    }else if(makeAndModel.contains("M7268")){
        pid = 0x5639;
        interface = 1;
//    }else if(makeAndModel.contains("G262DNT")){
//        pid = 0x5461;
//        interface = 1;
//    }else if(makeAndModel.contains("G336DNT")){
//        pid = 0x5462;
//        interface = 1;
#endif
    }else if(makeAndModel.contains("LJ2310N")){
        pid = 0x5459;
        interface = 1;
    }else if(makeAndModel.contains("LJ2320DN")){
        pid = 0x5454;
        interface = 1;
    }else if(makeAndModel.contains("G262DN")){
        pid = 0x544D;
        interface = 1;
    }else if(makeAndModel.contains("G336DN")){
        pid = 0x5466;
        interface = 1;
//    }else if(makeAndModel.contains("GM265DN")){
//        pid = 0x5663;
//        interface = 1;
//    }else if(makeAndModel.contains("GM337DN")){
//        pid = 0x5671;
//        interface = 1;
    }
    return (pid == -1) ?-1 :0;
}

AppConfig::AppConfig(QObject *parent) :
    QObject(parent)
{
}

extern const char* log_file;
int AppConfig::initConfig()
{
    log_app_name = EXE_NAME;
    app_version = APP_VERSION;
    log_file = LOG_FILE_NAME;
    log_init();

    if(app_file_locker.trylock(LOCKER_EXE)){
        LOGLOG("app had been locked!");
        return -1;
    }
//    if(is_app_running(SERVER_PATH)){
//        LOGLOG("socket working!");
//        return -2;
//    }

    LOGLOG("--------%s v%s-------" ,log_app_name ,app_version);
#ifndef DEBUG_DEBUG
    //release as deaemon
    int result = daemon(0 ,0);
    if(!result){
        LOGLOG("daemon success!");
    }
#endif

    g_config_file =  CONFIG_FILE;

    //config status server thread
#ifdef DEBUG_DEBUG
//    if(testmode){
        status_file = TEST_STATUS_FILE;
        status_lock_file = TEST_STATUS_LOCKER_FILE;
#else
//    }else{
        status_file = STATUS_FILE;
        status_lock_file = STATUS_LOCKER_FILE;
//    }
#endif

    //usb special config
    lock_scan_file = LOCKER_SANE;
    lock_scan_info_file = SANE_INFO_FILE;
    usb_error_printing = Usb_Printing;
    usb_error_scanning = Usb_Scanning;
    usb_error_usb_locked = Usb_Locked;

//    ui_server_path = SERVER_PATH;

    //config tomcat supported printer model
    isDeviceSupported = _isDeviceSupported;
    getpidvid = _getpidvid;

    app_server = new AppServer(SERVER_PATH);

    watcher = new Watcher;
    watcher->start();

    return 0;
}

#include <QFile>
void AppConfig::exit_app()
{
//    QFile::remove(status_file);
//    QFile::remove(status_lock_file);

    delete watcher;
    delete app_server;
    app_file_locker.unlock();
}
