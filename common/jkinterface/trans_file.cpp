#include "trans_file.h"
#include "log.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>


Trans_File_Server::Trans_File_Server()
{
    running = 1;
    hasmsg = false;
    memset(mtext ,0 ,TEXT_SIZE + 1);
}

Trans_File_Server::~Trans_File_Server()
{
    running = 0;
    while(!running){
        usleep(10*1000);
    }
}

int Trans_File_Server::createServer(const char* server_path)
{
    if(!server_path)
        return -1;

    strcpy(path ,server_path);
    return 0;
}

int Trans_File_Server::any_client_connected()
{
    int ret;
    FILE* file;
    int once = 1;
    while(running){
        if(hasmsg){
            if(once){
                LOGLOG("\t\Trans_File_Server:has msg ,waiting for read");
                once = 0;
            }
            usleep(10*1000);
            continue;
        }
        file = fopen(path ,"r");
        if(file){
            fseek(file ,0 ,SEEK_END);
            ret = ftell(file);
            if(ret != TEXT_SIZE){
                fclose(file);
                continue;
            }
            fseek(file ,0 ,SEEK_SET);
            ret = fread(mtext ,TEXT_SIZE ,1 ,file);
            fclose(file);
            if(ret == 1){
                hasmsg = true;
                LOGLOG("\t\Trans_File_Server:read %d" ,TEXT_SIZE);
                return 0;
            }
        }
        usleep(10*1000);
    }
    running = 1;
    return -1;
}

int Trans_File_Server::readThenWrite(int ,CALLBACK_Server callback ,void* para)
{
    int ret = 0;
    LOGLOG("\t\Trans_File_Server:no need read again");
    LOGLOG("\t\Trans_File_Server:read %s" ,mtext);
    if(!callback(para ,mtext ,TEXT_SIZE)){
        FILE* file;
        LOGLOG("\t\Trans_File_Server:write %s" ,mtext);
        file = fopen(path ,"r+");
        if(file){
            ret = fwrite(mtext ,TEXT_SIZE + 1 ,1 ,file);
            fclose(file);
            if(ret == 1){
                LOGLOG("\t\Trans_File_Server:write %d success" ,TEXT_SIZE + 1);
            }
        }
    }
    hasmsg = false;
    return ret;
}

Trans_File_Client::Trans_File_Client(const char* server_path)
    :Trans_Virtual_Client(server_path)
{
    memset(mtext ,0 ,TEXT_SIZE + 1);
}

Trans_File_Client::~Trans_File_Client()
{
}

int Trans_File_Client::tryConnectToServer()
{
    return 0;
}

#include <sys/stat.h>
#include "filelocker.h"
int Trans_File_Client::writeThenRead(char* buffer ,int bufsize)
{

    if(bufsize > TEXT_SIZE){
        return -1;
    }
    int ret;
    FILE* file;
    LOGLOG("\t\tTrans_File:create %s" ,path);
    char file_lock[256];
    sprintf(file_lock ,"%s_locker" ,path);
    FileLocker fl;
    ret = fl.lock(file_lock);
    if(ret){
        return -1;
    }
    file = fopen(path ,"w+");
    chmod(path ,DEFFILEMODE);
    if(file){
        LOGLOG("\t\tTrans_File:create %s ,success" ,path);
        memset(mtext ,0 ,TEXT_SIZE + 1);
        strcpy(mtext ,buffer);
        LOGLOG("\t\tTrans_File:write %d" ,TEXT_SIZE);
        LOGLOG("\t\tTrans_File:write %s" ,mtext);
        ret = fwrite(mtext ,TEXT_SIZE ,1 ,file);
        if(ret == 1){
            LOGLOG("\t\tTrans_File:write %d success" ,TEXT_SIZE);
        }
        fclose(file);
    }

    ret = -1;
    for(int i = 0 ;i < 50 ;i++){
        file = fopen(path ,"r");
        if(file){
            ret = fread(mtext ,TEXT_SIZE + 1 ,1 ,file);
            fclose(file);
            if(ret == 1){
                LOGLOG("\t\tTrans_File:read %d ,success" ,TEXT_SIZE + 1);
                LOGLOG("\t\tTrans_File:read %s" ,mtext);
                strcpy(buffer ,mtext);
                ret = 0;
                break;
            }
            usleep(1000 * 1000);
        }
    }
    remove(path);
    fl.unlock();
    return ret;
}


