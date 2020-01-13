#ifndef STATUSPASER_H
#define STATUSPASER_H

#include <QObject>

class StatusPaser : public QObject{
    Q_OBJECT
public:
    explicit StatusPaser(QObject *parent = 0);

public:
    static QString GetStatusTypeString(int type);
    static int GetStatusTypeForUI(int status);
    static QString getErrorMsg(int status,int job,bool isAbcPlusModel);
    static QString getTrayMsg(int status);
    static bool isAutoShow(int status);
};

#endif // STATUSPASER_H
