#ifndef TRANS_SOCKET_H
#define TRANS_SOCKET_H
#include "trans_virtual.h"

class Trans_Socket_Server : public Trans_Virtual_Server
{
public:
    Trans_Socket_Server();
    ~Trans_Socket_Server();
    int createServer(const char* server_path = 0);
    int readThenWrite(int fd ,CALLBACK_Server callback ,void* para);
    int any_client_connected();

private:
    int listen_fd;
};

class Trans_Socket_Client : public Trans_Virtual_Client
{
public:
    Trans_Socket_Client(const char* server_path = 0);
//    ~Trans_Msg_Client(){}
    int writeThenRead(char* buffer ,int bufsize);
    int tryConnectToServer();
};
#endif // TRANS_SOCKET_H
