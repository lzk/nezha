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

static void start_uiexe()
{
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
}

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

static int callback_Server(void* para ,char* buffer,int bufsize)
{
    LOGLOG("filter cmd is:%s" ,buffer);
//    AppServer* app_server = (AppServer*)para;
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
    LOGLOG("printer is:%s" ,printer.toUtf8().constData());
    LOGLOG("jobid is:%d" ,jobid);

	if(!cmd.compare("start")){
        strcpy(buffer ,"startok:30");
        start_uiexe();
    }else if(!cmd.compare("result")){
        strcpy(buffer ,"resultok");
        start_uiexe();
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

