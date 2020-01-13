#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "lld.h"

#define SERVER_PATH FILTER_TRANS_PATH

#define LOCKER_EXE LOCKER_SERVICE

#define EXE_NAME EXE_SERVICE_NAME

#define FILTERUIEXE_DIR APP_PATH

#define LOG_FILE_NAME LOG_SERVICE_FILE

#include <QObject>
class AppConfig : public QObject{
    Q_OBJECT
public:
    explicit AppConfig(QObject *parent = 0);

    static int initConfig();
    static void exit_app();
};
#endif // APPCONFIG_H
