#include "trans_virtual.h"
#include "string.h"

Trans_Virtual_Server::Trans_Virtual_Server()
{
    memset(path ,0 ,sizeof(path));
}

Trans_Virtual_Client::Trans_Virtual_Client(const char* server_path)
{
    memset(path ,0 ,sizeof(path));
    if(server_path)
        strcpy(path ,server_path);
//    LOGLOG("try to get server path:%s" ,server_path);

}
