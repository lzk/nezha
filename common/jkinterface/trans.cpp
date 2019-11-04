#include "trans.h"

#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "log.h"

//函数：设置sock为non-blocking mode
int setSockNonBlock(int sock)
{
    int flags;
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        return flags;
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) {
        return -1;
    }
    return 0;
}
#ifdef GLIBC_2_15
int select_fd(int fd)
{
    fd_set readfds;
//    fd_set writefds;
    FD_ZERO(&readfds);
//    FD_ZERO(&writefds);
    FD_SET(fd, &readfds);
//    FD_SET(listen_fd, &writefds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    int res;

    res = select(fd + 1, &readfds, NULL, NULL, &timeout);
//    res = select(listen_fd + 1, &readfds, &writefds, NULL, &timeout);
    if (res == -1) {
        return -1;
    } else if (res == 0) {
        return -2;//time out
//        fprintf(stderr, "no socket ready for read within %d secs\n", SELECT_TIMEOUT);
    }
    if(FD_ISSET(fd ,&readfds)){
        return 0;
    }
    return -3;
}
#endif
Trans_Server::Trans_Server()
{
    memset(path ,0 ,sizeof(path));
}

Trans_Server::~Trans_Server()
{
    if(listen_fd > 0){
        close(listen_fd);
        if(path[0])
            unlink(path);
//        remove(path);
    }
}

#include <sys/stat.h>
int Trans_Server::createServer(const char* server_path)
{
    int ret;
    struct sockaddr_un srv_addr;

    if(!server_path)
        return -1;

    strcpy(path ,server_path);

    listen_fd=socket(AF_UNIX,SOCK_STREAM,0);
    if(listen_fd<0){
        LOGLOG("connect creat communication socket");
    }

    //in case of 'address already in use' error message
    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
        return -1;
    }

//    setSockNonBlock(listen_fd);

    // set srv_addr param
    srv_addr.sun_family=AF_UNIX;
    strncpy(srv_addr.sun_path,path,sizeof(srv_addr.sun_path)-1);
    unlink(path);

    //bind sockfd&addr
    ret=bind(listen_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));

//    chmod(path ,);
    if(ret<0){
        LOGLOG("cannot bind server socket");
        close(listen_fd);
        unlink(path);
        listen_fd = -1;
    }
    //listen sockfd
    ret=listen(listen_fd,1);
    if(ret<0){
        LOGLOG("cannot listen sockfd");
        close(listen_fd);
        unlink(path);
        listen_fd = -1;
    }
//    char command[256];
//    sprintf(command ,"chmod a+w %s" ,path);
//    system(command);
    chmod(path ,DEFFILEMODE);
    return listen_fd;
}

int Trans_Server::any_client_connected()
{
    if(listen_fd < 0){
        return -1;
    }
#ifdef GLIBC_2_15
    if(select_fd(listen_fd))
        return -1;
#endif
    int com_fd = -1;
    socklen_t len;
    struct sockaddr_un clt_addr;

    //have connect requst use accept
    len=sizeof(clt_addr);
    com_fd=accept(listen_fd,(struct sockaddr*)&clt_addr,&len);
    if(com_fd<0){
//        LOGLOG("cannot accept requst,%d" ,com_fd);
        return -1;
    }
    return com_fd;
}

int Trans_Server::readThenWrite(int com_fd ,CALLBACK_Server callback ,void* para)
{
    if(com_fd < 0){
        return -1;
    }

    char buffer[1024];
    memset(buffer,0,sizeof(buffer));
    int num = read(com_fd ,buffer ,sizeof(buffer));

    if(num > 0){
        if(!callback(para ,buffer ,sizeof(buffer))){
            write(com_fd ,buffer ,strlen(buffer));
        }
    }

    close(com_fd);
    return 0;
}

Trans_Client::Trans_Client(const char* server_path)
{
    memset(path ,0 ,sizeof(path));
    if(server_path)
        strcpy(path ,server_path);
//    LOGLOG("try to get server path:%s" ,server_path);

}

int Trans_Client::tryConnectToServer()
{
    int connect_fd;
    int ret;
    struct sockaddr_un srv_addr;

    if(!path[0]){
        LOGLOG("tryConnectToServer:cannot get server path");
        return -1;
    }

    // creat unix socket
    connect_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd<0){
        LOGLOG("tryConnectToServer:cannot creat socket");
        return -1;
    }

    srv_addr.sun_family=AF_UNIX;
    strcpy(srv_addr.sun_path,path);
    //connect server
    ret = connect(connect_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if (ret < 0){
//        LOGLOG("cannot connect server");
        close(connect_fd);
        return -2;
    }
    close(connect_fd);
    return 0;
}

int Trans_Client::writeThenRead(char* buffer ,int bufsize)
{
    int connect_fd;
    int ret;
    struct sockaddr_un srv_addr;

    if(!path[0]){
        LOGLOG("cannot get server path");
        return -1;
    }

    // creat unix socket
    connect_fd=socket(PF_UNIX,SOCK_STREAM,0);
    if(connect_fd<0){
        LOGLOG("cannot creat socket");
        return -1;
    }
#ifdef GLIBC_2_15
    setSockNonBlock(connect_fd);
#endif
    srv_addr.sun_family=AF_UNIX;
    strcpy(srv_addr.sun_path,path);

    //connect server
    ret = connect(connect_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if (ret < 0){
        LOGLOG("cannot connect server");
        close(connect_fd);
        return -2;
    }
    ret = write(connect_fd,buffer,strlen(buffer));
    if(ret < 0){
        LOGLOG("cannot write to server");
        close(connect_fd);
        return -3;
    }

    while (true) {
#ifdef GLIBC_2_15
        ret = select_fd(connect_fd);
        if(ret == -1){
            ret = -4;
            break;
        }else if(ret == -2){
            continue;
        }
#endif
        memset(buffer ,0 ,bufsize);
        int num = read(connect_fd,buffer,bufsize);
        if(num > 0){
            ret = 0;
        }
        break;
    }
    close(connect_fd);
    return ret;
}


