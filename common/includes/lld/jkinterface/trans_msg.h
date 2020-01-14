#ifndef TRANS_MSG_H
#define TRANS_MSG_H
#include "trans_virtual.h"

#define TEXT_SIZE 1024
struct my_msgbuf
{
    long mtype;
    char mtext[TEXT_SIZE];
};

class Trans_Msg_Server : public Trans_Virtual_Server
{
public:
    Trans_Msg_Server();
    ~Trans_Msg_Server();
    int createServer(const char* server_path = 0);
    int readThenWrite(int fd ,CALLBACK_Server callback ,void* para);
    int any_client_connected();

private:
    int running;
    int msgid;
    bool hasmsg;
    struct my_msgbuf msg;
};

class Trans_Msg_Client : public Trans_Virtual_Client
{
public:
    Trans_Msg_Client(const char* server_path = 0);
    ~Trans_Msg_Client();
    int writeThenRead(char* buffer ,int bufsize);
    int tryConnectToServer();
private:
    int msgid;
    struct my_msgbuf msg;
};
#endif // TRANS_MSG_H
