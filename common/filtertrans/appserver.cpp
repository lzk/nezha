#include "appserver.h"
#include "commonapi.h"
#include "jkinterface.h"
#include "appconfig.h"
#include <unistd.h>
#include <QUrl>
#if QT_VERSION > 0x050000
#include <QUrlQuery>
#endif
#include "filterlib.h"
#include "filtercommon.h"
#include "filterjobhistory.h"

AppServer::AppServer(const char* server_path ,QObject *parent)
    : QObject(parent)
    ,server_path(server_path)
{
    thread_server = new ServerThread(server_path);
    connect(thread_server ,SIGNAL(client_connect(int)) ,this ,SLOT(client_connect(int)));
    connect(thread_server ,SIGNAL(client_cmd(QString ,void*)) ,this ,SLOT(client_cmd(QString ,void*)));
    thread_server->start();
}

AppServer::~AppServer()
{
    delete thread_server;
}

void AppServer::restart_server()
{
    delete thread_server;

    LOGLOG("restart server:%s" ,server_path.toLatin1().constData());
    thread_server = new ServerThread(server_path.toLatin1().constData());
    connect(thread_server ,SIGNAL(client_connect(int)) ,this ,SLOT(client_connect(int)));
    connect(thread_server ,SIGNAL(client_cmd(QString ,void*)) ,this ,SLOT(client_cmd(QString ,void*)));
    thread_server->start();
}

void AppServer::set_device_id(const QString& printer ,const QString& device_id)
{
    signal_set_device_id(printer ,device_id);
}

static int callback_Server(void* para ,char* buffer,int bufsize)
{
    LOGLOG("filter cmd is:%s" ,buffer);
    AppServer* app_server = (AppServer*)para;
    QUrl url(buffer);
    int jobid;
#if QT_VERSION > 0x050000
    jobid = QUrlQuery(QUrl(url)).queryItemValue("jobid").toInt();
#else
    jobid = QUrl(url).queryItemValue("jobid").toInt();
#endif
    QString cmd;
//    cmd = url.scheme();
//    QString printer = url.host(QUrl::PrettyDecoded);
    QString str(buffer);
    int index;
    index = str.indexOf("://");
    cmd = str.left(index);
//    LOGLOG("cmd is:%s" ,cmd.toLatin1().constData());
    QString printer = QString(buffer).mid(index + strlen("://"));
    index = printer.indexOf('?');
    if(index > 0)
        printer = printer.left(index);
    LOGLOG("printer is:%s" ,printer.toLatin1().constData());
    LOGLOG("jobid is:%d" ,jobid);

    if(!cmd.compare("dvid")){
    QString device_id;
//#if QT_VERSION > 0x050000
//    device_id = QUrlQuery(QUrl(url)).queryItemValue("deviceid");
//#else
//    device_id = QUrl(url).queryItemValue("deviceid");
//#endif
    index = str.indexOf("deviceid=");
    device_id = str.mid(index + strlen("deviceid="));
    LOGLOG("filterlib get device id is:%s" ,device_id.toLatin1().constData());
    app_server->set_device_id(printer ,device_id);
    strcpy(buffer ,"didok");
    return 0;

    }else if(!cmd.compare("start")){
        filter_update_status(jobid ,Filterlib_Status_checking);

        int time_val = 30;
        sprintf(buffer ,"startok:%d" ,time_val);

        QString tmp_cmd(str);
        tmp_cmd += QString("&time_val=%1").arg(time_val);
        LOGLOG("tjgd1zfmui cmd : %s" ,tmp_cmd.toLatin1().constData());
        pid_t pid = fork();
        switch(pid)
        {
        case -1:
            LOGLOG("fork %s failed:" ,EXE_FILTERUI_NAME);
            exit(1);
            break;
        case 0:{
            QString filteruiexe;
#ifdef DEBUG_DEBUG
            filteruiexe = QString("/tmp/") + EXE_FILTERUI_NAME;
#else
            filteruiexe = QString(FILTERUIEXE_DIR) + EXE_FILTERUI_NAME;
#endif
//            execlp("tjgd1zfmui", "tjgd1zfmui" ,tmp_cmd.toLatin1().constData() ,0);
            execl(filteruiexe.toLatin1().constData(), EXE_FILTERUI_NAME ,tmp_cmd.toLatin1().constData() ,0);
            exit(0);
            break;
        }
        default:
            break;
        }

    }else if(!cmd.compare("check")){
        int status = filter_get_status(jobid);
        switch(status){
        case Filterlib_Status_Cancel:
            strcpy(buffer ,"cancel");
            break;
        case Filterlib_Status_timeout:
            strcpy(buffer ,"timeout");
            break;
        case Filterlib_Status_checking:
            strcpy(buffer ,"checking");
            break;
        case Filterlib_Status_OK:
            strcpy(buffer ,"ok");
            break;
        case Filterlib_Status_authorize_fail:
            strcpy(buffer ,"fail");
            break;
        case Filterlib_Status_invalidJobid:
        default:
            strcpy(buffer ,"invalid");
            break;
        }
        return 0;
    }else if(!cmd.compare("result")){
        strcpy(buffer ,"resultok");

        int result;
        QString username,filename;
#if QT_VERSION > 0x050000
        result = QUrlQuery(QUrl(url)).queryItemValue("status").toInt();
        username = QUrlQuery(QUrl(url)).queryItemValue("username");
        filename = QUrlQuery(QUrl(url)).queryItemValue("filename");
#else
        result = QUrl(url).queryItemValue("status").toInt();
        username = QUrl(url).queryItemValue("username");
        filename = QUrl(url).queryItemValue("filename");
#endif
        LOGLOG("result is:%d" ,result);
        LOGLOG("username is:%s" ,username.toLatin1().constData());
        LOGLOG("filename is:%s" ,filename.toLatin1().constData());

        filterlib_struct fls;
        fls.jobid = jobid;
        fls.status = result;
        strcpy(fls.username ,username.toUtf8().constData());
        strcpy(fls.filename ,filename.toUtf8().constData());
        FilterJobHistory::save_job_history(&fls);
    }

    //start tjgd1zsmui
//    const char* app_locker_file = LOCKER_UI;
    FileLocker app_file_locker;
    if(!app_file_locker.trylock(LOCKER_UI)){
        app_file_locker.unlock();
//        LOGLOG("uiexe not locked!");
        pid_t pid = fork();
        switch(pid)
        {
        case -1:
            LOGLOG("fork %s failed:" ,EXE_UI_NAME);
            exit(1);
            break;
        case 0:
#ifdef DEBUG_DEBUG
            execl(QString("/tmp/%1").arg(EXE_UI_NAME).toLatin1().constData() ,EXE_UI_NAME ,"-hide" ,0);
#else
            execlp(EXE_UI_NAME ,EXE_UI_NAME ,"-hide" ,0);
#endif
            exit(0);
            break;
        default:
            break;
        }
    }
    return 0;
}

void AppServer::client_connect(int fd)
{
    thread_server->get_trans_server()->readThenWrite(fd ,callback_Server ,this);
}

void AppServer::client_cmd(const QString & ,void* )
{

}

