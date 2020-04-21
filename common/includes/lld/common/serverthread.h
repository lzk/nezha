#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include "jkinterface.h"

class ClientThread;
class AppServer;
class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(const char* server_path ,QObject *parent = NULL);
    ~ServerThread();

    void run();

    Trans_Server* get_trans_server(){return &trans_server;}
signals:
    void client_connect(int fd);
    void client_cmd(const QString &s ,void* para);

private:
    Trans_Server trans_server;
    bool abort;
    QString m_server_path;

    friend class ClientThread;
    friend class AppServer;
};

#endif // SERVERTHREAD_H
