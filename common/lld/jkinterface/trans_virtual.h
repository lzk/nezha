#ifndef TRANS_VIRTUAL_H
#define TRANS_VIRTUAL_H

typedef int (* CALLBACK_Server)(void* ,char* ,int);

class Trans_Virtual_Server
{
public:
    Trans_Virtual_Server();
    virtual ~Trans_Virtual_Server(){}
    virtual int createServer(const char* server_path = 0) = 0;
    virtual int readThenWrite(int fd ,CALLBACK_Server callback ,void* para) = 0;
    virtual int any_client_connected() = 0;
    const char* get_server_path(){return path;}

protected:
    char path[256];
};

class Trans_Virtual_Client
{
public:
    Trans_Virtual_Client(const char* server_path = 0);
    virtual ~Trans_Virtual_Client(){}
    virtual int writeThenRead(char* buffer ,int bufsize) = 0;
    virtual int tryConnectToServer() = 0;
protected:
    char path[256];
};

#endif // TRANS_VIRTUAL_H
