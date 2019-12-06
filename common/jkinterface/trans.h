#ifndef TRANS_H
#define TRANS_H

//#define TRANS_SOCKET 1
//#define TRANS_MEG 1
#define TRANS_FILE 1

#if TRANS_SOCKET
#include "trans_socket.h"
typedef Trans_Socket_Server Trans_Server;
typedef Trans_Socket_Client Trans_Client;
#elif TRANS_MEG
#include "trans_msg.h"
typedef Trans_Msg_Server Trans_Server;
typedef Trans_Msg_Client Trans_Client;
#elif TRANS_FILE
#include "trans_file.h"
//typedef Trans_File_Server Trans_Server;
//typedef Trans_File_Client Trans_Client;
#define Trans_Server Trans_File_Server
#define Trans_Client Trans_File_Client
#endif

#endif // TRANS_H
