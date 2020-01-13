#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "lld.h"

#define SERVER_PATH SERVICE_TRANS_PATH

#define LOCKER_EXE LOCKER_TRANS

#define EXE_NAME EXE_TRANS_NAME

#define FILTERUIEXE_DIR APP_PATH

#define LOG_FILE_NAME LOG_TRANS_FILE

#include <QObject>
class AppConfig : public QObject{
    Q_OBJECT
public:
    explicit AppConfig(QObject *parent = 0);

    static int initConfig();
    static void exit_app();
};
#endif // APPCONFIG_H
