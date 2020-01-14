#include <QSettings>
#include "filterlib.h"
#include "pro_config.h"

int filter_get_status(int jobid)
{
    bool ok;
    QSettings settings(Trans_Conf_file ,QSettings::NativeFormat);
    settings.beginGroup(QString("%1").arg(jobid));
    int value = settings.value("status").toInt(&ok);
    settings.endGroup();
    if(!ok)
        value = Filterlib_Status_checking;
    return value;
}

void filter_update_status(int jobid ,int status)
{
    QSettings settings(Trans_Conf_file ,QSettings::NativeFormat);
    settings.beginGroup(QString("%1").arg(jobid));
    settings.setValue("status" ,QString("%1").arg(status));
    settings.endGroup();
    settings.sync();
}

void filter_update_result(int jobid ,int result)
{
    QSettings settings(Trans_Conf_file ,QSettings::NativeFormat);
    settings.beginGroup(QString("%1").arg(jobid));
    settings.setValue("result" ,QString("%1").arg(result));
    settings.endGroup();
    settings.sync();
}

int filter_get_result(int jobid)
{
    bool ok;
    QSettings settings(Trans_Conf_file ,QSettings::NativeFormat);
    settings.beginGroup(QString("%1").arg(jobid));
    int value = settings.value("result").toInt(&ok);
    settings.endGroup();
    if(!ok)
        value = Filterlib_Status_checking;
    return value;
}
