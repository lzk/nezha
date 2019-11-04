#ifndef TRANS_H
#define TRANS_H

typedef int (* CALLBACK_Server)(void* ,char* ,int);

class Trans_Server
{
public:
    Trans_Server();
    ~Trans_Server();
    int createServer(const char* server_path = 0);
    int readThenWrite(int fd ,CALLBACK_Server callback ,void* para);
    int any_client_connected();
    const char* get_server_path(){return path;}

private:
    int listen_fd;
    char path[256];
};

class Trans_Client
{
public:
    Trans_Client(const char* server_path = 0);
    int writeThenRead(char* buffer ,int bufsize);
    int tryConnectToServer();
private:
    char path[256];
};
#endif // TRANS_H
