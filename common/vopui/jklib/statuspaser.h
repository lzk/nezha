#ifndef STATUSPASER_H
#define STATUSPASER_H

#include <QObject>

class StatusPaser : public QObject{
    Q_OBJECT
public:
    explicit StatusPaser(QObject *parent = 0);

    static bool is_enable_scan(int status);
    static bool is_enable_copy(int status);
    static bool is_enable_setting(int status);

    static int GetStatusTypeForUI(int status);
    static QString getErrorMsg(int status,int job,bool isAbcPlusModel);
};

#endif // STATUSPASER_H
