#include "trans_msg.h"
#include "log.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_TYPE 1
#define CLIENT_TYPE 10

static key_t getkey(const char* path)
{
    return ftok(path ,0);
}

static int getmsgid(const char* path)
{
    int msgid = msgget(getkey(path) ,0666 | IPC_CREAT);
    if(msgid <= 0){
//        LOGLOG("Trans_Msg:can not create msg");
        return -1;
    }
    return msgid;
}

static int creatmsgid(const char* path)
{
    return getmsgid(path);
    int msgid = msgget(getkey(path) ,0666 | IPC_CREAT | IPC_EXCL);// duilie
    if(msgid <= 0){
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

Trans_Msg_Server::Trans_Msg_Server()
{
    running = 1;
    hasmsg = false;
}

Trans_Msg_Server::~Trans_Msg_Server()
{
    running = 0;
    while(!running){
        usleep(10*1000);
    }
    if(msgid >= 0)
        destroymsg(msgid);
}

int Trans_Msg_Server::createServer(const char* server_path)
{
    if(!server_path)
        return -1;

    strcpy(path ,server_path);
    return 0;
}

int Trans_Msg_Server::any_client_connected()
{
    int ret;
    int once = 1;
    while(running){

        if(hasmsg){
            if(once){
                LOGLOG("Trans_Msg:has msg ,waiting for read");
                once = 0;
            }
            usleep(10*1000);
            continue;
        }
        msgid = getmsgid(path);
        if(ret == TEXT_SIZE){
        }
        if(msgid > 0){
            LOGLOG("Trans_Msg:has msg id %d" ,msgid);
            hasmsg = true;
            return 0;
        }
        usleep(10*1000);
    }
    running = 1;
    return -1;
}

int Trans_Msg_Server::readThenWrite(int ,CALLBACK_Server callback ,void* para)
{
    int ret = 0;
    if(msgid <= 0){
        return -1;
    }
    LOGLOG("Trans_Msg:has msg ,read");
    msg.mtype = CLIENT_TYPE;
    ret = msgreceive(msgid ,&msg ,TEXT_SIZE);
    LOGLOG("Trans_Msg:has msg ,read over");
    if(ret != TEXT_SIZE){
        return -1;
    }
    if(!callback(para ,msg.mtext ,TEXT_SIZE)){
        LOGLOG("Trans_Msg:has msg ,write");
        msg.mtype = SERVER_TYPE;
        ret = msgsend(msgid ,&msg ,TEXT_SIZE);
    }
    hasmsg = false;
    return ret;
}

Trans_Msg_Client::Trans_Msg_Client(const char* server_path)
    :Trans_Virtual_Client(server_path)
{
    LOGLOG("Trans_Msg:create %s" ,server_path);
    FILE* file = fopen(server_path ,"w+");
    if(file){
        fclose(file);
        LOGLOG("Trans_Msg:create %s ,success" ,path);
    }
    msgid = creatmsgid(path);
    if(msgid <= 0){
        LOGLOG("Trans_Msg:can not create msg");
    }
}

Trans_Msg_Client::~Trans_Msg_Client()
{
    if(msgid > 0)
        destroymsg(msgid);
    remove(path);
}

int Trans_Msg_Client::tryConnectToServer()
{
    return 0;
}

int Trans_Msg_Client::writeThenRead(char* buffer ,int bufsize)
{
    if(bufsize > TEXT_SIZE){
        return -1;
    }
    if(msgid <= 0){
        return -1;
    }
    int ret;
    msg.mtype = CLIENT_TYPE;
    memset(msg.mtext ,0 ,TEXT_SIZE);
    strcpy(msg.mtext ,buffer);
    LOGLOG("Trans_Msg:has msg ,write");
    ret = msgsend(msgid ,&msg ,TEXT_SIZE);
    if(!ret){
        LOGLOG("Trans_Msg:send msg success ,then read");
        msg.mtype = SERVER_TYPE;
        ret = msgreceive(msgid ,&msg ,TEXT_SIZE);
        if(ret == TEXT_SIZE){
            strcpy(buffer ,msg.mtext);
        }
    }
    LOGLOG("Trans_Msg:read over");
    return 0;
}


