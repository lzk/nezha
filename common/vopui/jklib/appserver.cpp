#include "appserver.h"
#include "commonapi.h"
#include "jkinterface.h"
#include <unistd.h>
#include <QUrl>
#if QT_VERSION > 0x050000
#include <QUrlQuery>
#endif

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
    if(index > 0){
        cmd = str.left(index);
    //    LOGLOG("cmd is:%s" ,cmd.toLatin1().constData());
        QString printer = QString(buffer).mid(index + strlen("://"));
        index = printer.indexOf('?');
        if(index > 0)
            printer = printer.left(index);
        LOGLOG("printer is:%s" ,printer.toUtf8().constData());
        LOGLOG("jobid is:%d" ,jobid);
    }else{
        cmd = str;
    }

    if(!cmd.compare("startexe")){
        app_server->startexe();
        LOGLOG("start exe now");
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

void AppServer::startexe()
{
    signal_startexe();
}
