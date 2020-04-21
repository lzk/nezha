#ifndef TRANS_FILE_H
#define TRANS_FILE_H
#include "trans_virtual.h"

#define TEXT_SIZE 1024

class Trans_File_Server : public Trans_Virtual_Server
{
public:
    Trans_File_Server();
    ~Trans_File_Server();
    int createServer(const char* server_path = 0);
    int readThenWrite(int fd ,CALLBACK_Server callback ,void* para);
    int any_client_connected();

private:
    int running;
    bool hasmsg;
    char mtext[TEXT_SIZE + 1];
};

class Trans_File_Client : public Trans_Virtual_Client
{
public:
    Trans_File_Client(const char* server_path = 0);
    ~Trans_File_Client();
    int writeThenRead(char* buffer ,int bufsize);
    int tryConnectToServer();
private:
    char mtext[TEXT_SIZE + 1];
};
#endif // TRANS_FILE_H
