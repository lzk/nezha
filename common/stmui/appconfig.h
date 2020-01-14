#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "lld.h"

//#define SERVER_PATH SERVICE_TRANS_PATH

#define LOCKER_EXE LOCKER_UI

#define EXE_NAME EXE_UI_NAME

//#define FILTERUIEXE_DIR APP_PATH

#define LOG_FILE_NAME LOG_UI_FILE

#include "statusmonitor.h"
#include <QObject>
#include <QMetaType>

extern bool use_status_thread;
extern bool testmode;

Q_DECLARE_METATYPE(Printer_struct)
Q_DECLARE_METATYPE(QList<Printer_struct>)
Q_DECLARE_METATYPE(PrinterInfo_struct)
Q_DECLARE_METATYPE(QList<PrinterInfo_struct>)

class AppConfig : public QObject{
    Q_OBJECT
public:
    explicit AppConfig(QObject *parent = 0);

    static int initConfig();
    static void exit_app();
};

#endif // APPCONFIG_H
