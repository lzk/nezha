#include "serverthread.h"
#include <QFile>
ServerThread::ServerThread(const char* server_path ,QObject *parent)
    : QThread(parent)
    ,m_server_path(server_path)
{
    abort = false;
    trans_server.createServer(server_path);
    LOGLOG("create server:%s" ,server_path);
}

ServerThread::~ServerThread()
{
    abort = true;
    LOGLOG("delete server:%s" ,m_server_path.toLatin1().constData());
    while(abort){
        Trans_Client tc(m_server_path.toLatin1().constData());
        tc.tryConnectToServer();
        usleep(100 * 1000);
    }
    if(QFile::exists(m_server_path))
        QFile::remove(m_server_path);
}

void ServerThread::run()
{
    forever {
        if (abort){
            break;
        }
        int fd = trans_server.any_client_connected();
        if(fd >= 0){
            client_connect(fd);
        }
    }
    abort = false;
}

