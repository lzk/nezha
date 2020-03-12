#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "statusmonitor.h"
#include <QObject>
#include <QMetaType>
#include "lshellstruct.h"
#include "scannerapp.h"

extern bool use_status_thread;
extern bool testmode;

#include "lld.h"

//#define SERVER_PATH SERVICE_TRANS_PATH

#define LOCKER_EXE LOCKER_VOP

#define EXE_NAME EXE_VOP_NAME

//#define FILTERUIEXE_DIR APP_PATH

#define LOG_FILE_NAME LOG_VOP_FILE

Q_DECLARE_METATYPE(Printer_struct)
Q_DECLARE_METATYPE(QList<Printer_struct>)
Q_DECLARE_METATYPE(PrinterInfo_struct)
Q_DECLARE_METATYPE(QList<PrinterInfo_struct>)
Q_DECLARE_METATYPE(copycmdset)
Q_DECLARE_METATYPE(cmdst_wifi_get)
Q_DECLARE_METATYPE(cmdst_aplist_get)
Q_DECLARE_METATYPE(cmdst_passwd)
Q_DECLARE_METATYPE(net_info_st)
Q_DECLARE_METATYPE(net_ipv6_st)
Q_DECLARE_METATYPE(cmdst_userconfig)
Q_DECLARE_METATYPE(cmdst_softap)

//Q_DECLARE_METATYPE(cmdst_wifi_status)
//Q_DECLARE_METATYPE(cmdst_tonerEnd)
//Q_DECLARE_METATYPE(cmdst_PSave_time)
//Q_DECLARE_METATYPE(cmdst_powerOff_time)
//Q_DECLARE_METATYPE(cmdst_fusingScReset)

Q_DECLARE_METATYPE(ScanSettings)

typedef struct struct_wifi_refresh_info
{
    struct cmdst_wifi_get wifi_para;
    bool wifi_status;
    struct cmdst_aplist_get wifi_aplist;
}
    struct_wifi_refresh_info;
Q_DECLARE_METATYPE(struct_wifi_refresh_info)

//Q_DECLARE_METATYPE(cmdst_tonerReset)
//Q_DECLARE_METATYPE(cmdst_drumReset)

Q_DECLARE_METATYPE(cmdst_user_center)

typedef struct struct_user_center
{
    QString strPrinterModel;
    struct _cmdst_user_center user_center;
    Counter_printer_scan counter_printer_scan;
    PrinterInfo_struct printer_info;
}
    struct_user_center;
Q_DECLARE_METATYPE(struct_user_center)


Q_DECLARE_METATYPE(Counter_printer_scan)

class AppConfig : public QObject{
    Q_OBJECT
public:

    explicit AppConfig(QObject *parent = 0);

    static int initConfig();
    static void exit_app();

public:
    static QString tmp_scan_dir;
};
#endif // APPCONFIG_H
