#include <QSettings>
#include "log.h"
#include "trans.h"
#include "lld.h"
bool appSettings(const QString& key ,QVariant& value ,const QVariant& defaultValue ,bool set)
{
    bool result = true;
    QSettings settings(g_config_file ,QSettings::NativeFormat);
    if(set){
        settings.setValue(key ,value);
        settings.sync();
        LOGLOG("set key %s value:%s" ,key.toLatin1().constData() ,settings.value(key ,defaultValue).toString().toLatin1().constData());
    }else{
        value = settings.value(key ,defaultValue);
        LOGLOG("get key %s value:%s" ,key.toLatin1().constData() ,value.toString().toLatin1().constData());
    }
    return result;
}
#if 1
#include <QCoreApplication>
#include <QLocalSocket>
 #include <QLocalServer>
#include <QFile>
QLocalServer* m_localServer;
bool is_app_running(const char* serverName)
{
    bool running = true;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (!socket.waitForConnected()) {
//        LOGLOG(socket.errorString());
        if(QFile::exists(serverName))
            QFile::remove(serverName);

        m_localServer = new QLocalServer(qApp);
        if (!m_localServer->listen(serverName)) {
            if (m_localServer->serverError() == QAbstractSocket::AddressInUseError
                    && QFile::exists(serverName)) { //make sure listening success
                QFile::remove(serverName);
                m_localServer->listen(serverName);
            }
        }
        if(!system((QString("chmod a+w ") + serverName+ " 2>/dev/null").toLatin1())){
            ;
        }
        running = false;
    }
    return running;
}
#else
bool is_app_running(const char* server_path)
{
    bool running = true;
    Trans_Client tc(server_path);
    int ret = tc.tryConnectToServer();
    switch (ret) {
    case 0:
//        LOGLOG("There has been a same app running!");
        break;
    case -2:
        running = false;
        break;
    default:
//        LOGLOG("There is something error!");
        break;
    }
    return running;
}
#endif

#if 0
#include <QMutex>
#include <QFile>
#include <QTextStream>
//static QMutex mutex;
//static const QString tmp_file = "/tmp/lnttmp";
#include <QDateTime>
QString get_string_from_shell_cmd(const QString& cmd ,int mode)
{
//    QMutexLocker locker(&mutex);
    QString tmp_file = QString("/tmp/lnttmp_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QString str;
    QString _cmd(cmd);
    _cmd += " > ";
    _cmd += tmp_file;
//    _cmd += "&&chmod 666 ";
//    _cmd += tmp_file;
//    _cmd += " 2>>";
//    _cmd += "/dev/null";
    if(!system(_cmd.toLatin1().constData())){
    }
    QFile fl(tmp_file);
    if(fl.open(QFile::ReadOnly)){
        QTextStream in(&fl);
        if(mode)
            str = in.readAll();
        else
            str = in.readLine();
        fl.close();
        fl.remove();
    }
    return str;
}
#else
#include <stdio.h>
QString get_string_from_shell_cmd(const QString& cmd ,int mode)
{
    QString str;
    QString _cmd(cmd);

    FILE *fp;
    char buf[512];

    if((fp = popen(_cmd.toLatin1().constData(), "r")) == NULL) {
        LOGLOG("command %s error" ,_cmd.toLatin1().constData());
        return "";
    }

    if(mode){
        while(fgets(buf, 512, fp) != NULL) {
            str += buf;
        }
    }else{
        if((fgets(buf, 512, fp) != NULL)){
            str += buf;
        }
    }

    pclose(fp);
    return str;
}
#endif
bool printer_is_printing(const QString& printer_name)
{
    QString str("LANG=en lpstat -l -o ");
    str += printer_name;
    str += " 2>>/dev/null";
    str += "|grep -w ";
    str += printer_name;
    QString printer_jobs = get_string_from_shell_cmd(str ,0);
    return !printer_jobs.isEmpty();
}
