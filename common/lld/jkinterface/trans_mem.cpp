#include "trans_mem.h"
#include "log.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>

#define SERVER_TYPE 1
#define CLIENT_TYPE 10

static key_t getkey(const char* path)
{
    return ftok(path ,0);
}

static int getmsgid(const char* path)
{
    int msgid = msgget(getkey(path) ,0666 | IPC_CREAT);// duilie
    if(msgid < 0){
        LOGLOG("Trans_Msg:can not create msg");
        return -1;
    }
    return 0;
}

static int creatmsgid(const char* path)
{
    return getmsgid(path);
    int msgid = msgget(getkey(path) ,0666 | IPC_CREAT | IPC_EXCL);// duilie
    if(msgid < 0){
        LOGLOG("Trans_Msg:can not create msg");
        return -1;
    }
    return msgid;
}

static int msgsend(int msgid ,struct my_msgbuf* msg ,int bufsize)
{
    int ret = msgsnd(msgid ,(void*)msg ,bufsize ,0);
    if(ret < 0){
        LOGLOG("Trans_Msg:can not send");
        return -1;
    }
    return 0;
}

static int msgreceive(int msgid ,struct my_msgbuf* msg ,int bufsize)
{
    ssize_t size = msgrcv(msgid ,(void*)msg ,bufsize ,msg->mtype ,0);
    if(size <= 0){
        LOGLOG("Trans_Msg:can not receive");
        return -1;
    }
    return size;
}

static int destroymsg(int msgid)
{
    int ret = msgctl(msgid ,IPC_RMID ,NULL);
    if(ret < 0){
        LOGLOG("Trans_Msg:can not destroy");
        return -1;
    }
    return 0;
}

Trans_Mem_Server::Trans_Mem_Server()
{
    running = 1;
    hasmsg = false;
}

Trans_Mem_Server::~Trans_Mem_Server()
{
    running = 0;
    while(!running){
        usleep(10*1000);
    }
    if(msgid >= 0)
        destroymsg(msgid);
}

int Trans_Mem_Server::createServer(const char* server_path)
{
    if(!server_path)
        return -1;

    strcpy(path ,server_path);
    msgid = getmsgid(path);
    return msgid >= 0 ?0 :-1;
}

int Trans_Mem_Server::any_client_connected()
{
    int ret;
    while(running){
        if(hasmsg){
            usleep(10*1000);
            continue;
        }
        if(msgid < 0){

        }else{
            msg.mtype = CLIENT_TYPE;
            ret = msgreceive(msgid ,&msg ,TEXT_SIZE);
            if(ret == TEXT_SIZE){
                hasmsg = true;
                return 0;
            }
        }
        usleep(10*1000);
    }
    running = 1;
    return -1;
}

int Trans_Mem_Server::readThenWrite(int ,CALLBACK_Server callback ,void* para)
{
    int ret = 0;
    if(msgid < 0){
        return -1;
    }
    if(!callback(para ,msg.mtext ,TEXT_SIZE)){
        msg.mtype = SERVER_TYPE;
        ret = msgsend(msgid ,&msg ,TEXT_SIZE);
    }
    hasmsg = false;
    return ret;
}

Trans_Mem_Client::Trans_Mem_Client(const char* server_path)
    :Trans_Virtual_Client(server_path)
{
    msgid = creatmsgid(path);
}

Trans_Mem_Client::~Trans_Mem_Client()
{
    if(msgid >= 0)
        destroymsg(msgid);
}

int Trans_Mem_Client::tryConnectToServer()
{
    return 0;
}

int Trans_Mem_Client::writeThenRead(char* buffer ,int bufsize)
{

    if(msgid < 0){
        return -1;
    }
    int ret;
    msg.mtype = CLIENT_TYPE;
    strcpy(msg.mtext ,buffer);
    ret = msgsend(msgid ,&msg ,TEXT_SIZE);
    if(!ret){
        msg.mtype = SERVER_TYPE;
        ret = msgreceive(msgid ,&msg ,TEXT_SIZE);
        if(ret == TEXT_SIZE){
            strcpy(buffer ,msg.mtext);
        }
    }
    return 0;
}


