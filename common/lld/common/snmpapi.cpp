
#include "snmpapi.h"
#include "log.h"
#include <arpa/inet.h>
#define TimeOutSecond 5
enum{
    SNMP_CMD_getdeviceid,
    SNMP_CMD_getdeviceid_broadcast,
    SNMP_CMD_search,
};

#if 1
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BOOL int
#define BYTE unsigned char
#define TRUE 1
#define FALSE 0
#define DWORD unsigned int

char    _deviceid[2049];
BOOL outputDeviceIdValue( BYTE* oid, int oidLen, BYTE valueType, BYTE* valueData, int valueLen )
{
    if( valueType != 0x04 ) // string
    {
        return FALSE;
    }
    memcpy( _deviceid, valueData, MIN(valueLen,2049) );
    _deviceid[valueLen] = 0x00;
    return TRUE;
}
//returns next byte ptr
BYTE* parseLength( BYTE* data, int* length )
{
    int n = 0;
    int i = 0;
    char    c = *data & 0x80;
    if( c == 0x00 )
    {
        *length = *data;
        return ( data + 1 );
    }
    else
    {
        c = *data & 0x7f;
        *length = 0;
        data++;
        for( i = 0; i < c; i++ )
        {
            *length = *length << 8;
            *length += ( int )( DWORD ) * data;
            data++;
        }
        return data;
    }
}

typedef BOOL (*FNOUTPUTRESPONSEVALUE)(BYTE* oid, int oidLen, BYTE valueType, BYTE* valueData, int valueLen);
BOOL parseGetResponse( BYTE* udpdata, int len, int* version, char* community, BYTE* requestId, BYTE* errorStatus, BYTE* errorIndex, FNOUTPUTRESPONSEVALUE outputResponseValue )
{
    BYTE    temp[2048];
    BYTE    rType;
    int i;
    int length;
    BYTE*   next;
    char    oid[1024];


    if( udpdata[0] != 0x30 )
    {
        return FALSE;
    }
    next = parseLength( &udpdata[1], &length );
    if( length != len - ( next - udpdata ) )
    {
        return FALSE;
    }
    // version
    if( next[0] != 0x02 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( version )
    {
        if( length == 1 )
        {
            *version = ( int )( *next );
        }
        else
        {
            *version = ( int )( *next );    // I don'care such case!
        }
    }
    next += length;
    // community
    if( next[0] != 0x04 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( community )
    {
        memcpy( community, next, length );
        community[length] = 0x00;
    }
    next += length;
    // PDU type
    if( *next != 0xa2 )  // check if it is GetResponse
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( length != len - ( next - udpdata ) ) // check length
    {
        return FALSE;
    }
    // request ID
    if( next[0] != 0x02 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( requestId )
    {
        if( length == 1 )
        {
            *requestId = *next;
        }
        else
        {
            *requestId = *next;    // I don'care such case!
        }
    }
    next += length;
    // error status
    if( next[0] != 0x02 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( errorStatus )
    {
        if( length == 1 )
        {
            *errorStatus = *next;
        }
        else
        {
            *errorStatus = *next;    // I don'care such case!
        }
    }
    next += length;
    // error index
    if( next[0] != 0x02 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    if( errorIndex )
    {
        if( length == 1 )
        {
            *errorIndex = *next;
        }
        else
        {
            *errorIndex = *next;    // I don'care such case!
        }
    }
    next += length;

    // sequence for the list of name-value pairs
    if( *next != 0x30 )
    {
        return FALSE;
    }
    next++;
    next = parseLength( next, &length );
    BYTE* p = next;
    BYTE* q = next + length;
    while( p < q )
    {
        BYTE*   oidTemp;
        int oidLen;
        BYTE    valType;
        BYTE*   value;
        int valLen;
        // sequence for a name-value pair
        if( *p != 0x30 )
        {
            return FALSE;
        }
        p++;
        p = parseLength( p, &length );
        if( *p != 0x06 ) // this should be an OID type
        {
            return FALSE;
        }
        p++;
        p = parseLength( p, &oidLen ); // length of OID
        oidTemp = p;
        p += oidLen; // goto start of value
        valType = *p;
        p++;
        p = parseLength( p, &valLen );
        value = p;
        p = p + valLen;
        // call the callback function to report the name-value pair
        if( outputResponseValue )
        {
            outputResponseValue( oidTemp, oidLen, valType, value, valLen );
        }
    }
    return TRUE;
}
BOOL parseForDeviceId( BYTE* udpdata, int len, char* deviceid )
{
    if( !parseGetResponse( udpdata, len, NULL, NULL, NULL, NULL, NULL, outputDeviceIdValue ) )
    {
        return FALSE;
    }
    strcpy( deviceid, _deviceid );
    return TRUE;
}

struct snmp_data_t
{
    int state;
    int cmd;
    char* buffer;
    int bufsize;
    struct sockaddr* sock_addr;
    callback_snmp callback;
    void* data;
    char ip[256];
    int isv6;
};

//enterprises.2699.1.2.1.2.1.1.3.1
//enterprises.26266.86.10.1.1.1.1.0
//static const oid oid_status[] = {1,3,6,1,4,1,2699,1,2,1,2,1,1,3,1};
const unsigned char oid_data[] = {0x30,0x30,0x02,0x01,0x01,0x04,0x06,0x70,0x75,0x62,0x6c,0x69,0x63,0xa0,0x23,0x02
                         ,0x04,0x66,0x02,0x52,0x0b,0x02,0x01,0x00,0x02,0x01,0x00,0x30,0x15,0x30,0x13,0x06
                         ,0x0f,0x2b,0x06,0x01,0x04,0x01,0x95,0x0b,0x01,0x02,0x01,0x02,0x01,0x01,0x03,0x01
                         ,0x05,0x00};

void handler_data(struct snmp_data_t* pdata)
{
    char buffer[1024];
    switch (pdata->cmd) {
    case SNMP_CMD_getdeviceid_broadcast:
        if(parseForDeviceId((unsigned char*)pdata->buffer ,pdata->bufsize ,buffer)){
            strcpy(pdata->buffer ,buffer);
            if(pdata->callback)
                pdata->callback(pdata->ip,pdata->buffer ,pdata->bufsize ,pdata->data);
        }
        break;
    case SNMP_CMD_getdeviceid:
        pdata->state = 0;
        break;
    default:
        break;
    }
}

int snmp_handler(
        struct snmp_data_t* sdata ,char** ips ,int numOfips)
{
    int sock;
    if(!sdata->isv6)
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else
        sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1){
        return -1;
    }

    int    bOptVal = 1;
    if(sdata->cmd == SNMP_CMD_getdeviceid)
        bOptVal = 0;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bOptVal, sizeof(bOptVal));

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    int nRet;
    struct sockaddr_in	RecvAddr;
    bzero(&RecvAddr,sizeof(RecvAddr));
    struct sockaddr_in6	RecvAddr6;
    bzero(&RecvAddr6,sizeof(RecvAddr6));

    socklen_t socklen;
    struct sockaddr* addr;

    if(!sdata->isv6){
        RecvAddr.sin_family = AF_INET;
        RecvAddr.sin_port = htons(161);
        socklen = sizeof(RecvAddr);
        addr = (struct sockaddr*) &RecvAddr;
    }else{
        RecvAddr6.sin6_family = AF_INET6;
        RecvAddr6.sin6_port = htons(161);
        socklen = sizeof(RecvAddr6);
        addr = (struct sockaddr*) &RecvAddr6;
    }

    int i;
    for(i = 0; i < numOfips; i++)
    {
        if(!sdata->isv6){
            RecvAddr.sin_addr.s_addr = inet_addr(ips[i]);
        }else{
            inet_pton(AF_INET6, ips[i], &RecvAddr6.sin6_addr);
        }
        nRet = sendto(sock, oid_data, sizeof(oid_data), 0, addr, socklen);

        usleep(30);
    }
    if (nRet <= 0)
    {
        close(sock);
        return -1;
    }

    fd_set fdset;
    int numfds;
    int count;
    int nRecv;

    timeout.tv_sec = TimeOutSecond;
    timeout.tv_usec = 0;
    while (sdata->state) {
        numfds = sock + 1;
        FD_ZERO(&fdset);
        FD_SET(sock,&fdset);
        count = select(numfds ,&fdset ,NULL ,NULL ,&timeout);
        switch (count) {
        case -1://error
            sdata->state = 0;
            break;
        case 0://time out
            sdata->state = 0;
            break;
        default:
            if(FD_ISSET(sock,&fdset))
            {
                nRecv = recvfrom(sock,sdata->buffer,(size_t)sdata->bufsize,0 ,addr, &socklen);
                if(nRecv > 0){
                    sdata->bufsize = nRecv;
                    if(!sdata->isv6){
                        strcpy(sdata->ip ,inet_ntoa(RecvAddr.sin_addr));
                    }else{
                        inet_ntop(AF_INET6,&RecvAddr6.sin6_addr,sdata->ip,sizeof(sdata->ip));
                    }
                    handler_data(sdata);
                }
            }
            break;
        }
    }
    close(sock);
    return 0;
}

int snmp_broadcast(char**ips, int numOfips ,char *buffer, int bufsize
                   ,callback_snmp callback ,void* data)
{
    if(!numOfips)
        return 0;
    struct snmp_data_t sdata;
    sdata.buffer = buffer;
    sdata.bufsize = bufsize;
    sdata.callback = callback;
    sdata.data = data;
    sdata.state = 1;
    sdata.isv6 = 0;
    sdata.cmd = SNMP_CMD_getdeviceid_broadcast;
    int ret = snmp_handler(&sdata ,ips ,numOfips);
    return ret;
}

int snmp_broadcast6(char**ips, int numOfips ,char *buffer, int bufsize
                   ,callback_snmp callback ,void* data)
{
    if(!numOfips)
        return 0;
    struct snmp_data_t sdata;
    sdata.buffer = buffer;
    sdata.bufsize = bufsize;
    sdata.callback = callback;
    sdata.data = data;
    sdata.state = 1;
    sdata.isv6 = 1;
    sdata.cmd = SNMP_CMD_getdeviceid_broadcast;
    return snmp_handler(&sdata ,ips ,numOfips);
}


int snmp_get_deviceid(char* ip ,char* buffer ,int bufsize)
{
    return 0;
}

int snmp_get_deviceid_broadcast(char *ip, char *buffer, int bufsize ,callback_snmp callback ,void* data)
{
    return 0;
}

#else

#ifndef HAVE_STRTOULL
#define HAVE_STRTOULL 1
#endif
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

//enterprises.2699.1.2.1.2.1.1.3.1
//enterprises.26266.86.10.1.1.1.1.0
static const oid oid_status[] = {1,3,6,1,4,1,2699,1,2,1,2,1,1,3,1};

struct My_synch_state{
    struct synch_state state;
    int cmd;
    char* buffer;
    int bufsize;
    int ret_var;
    const oid* oidname;
    size_t sizeof_oidname;
    callback_snmp callback;
    void * data;
};

static int handlerData(netsnmp_pdu *pdu, void *magic)
{
    int ret;
    struct synch_state *state = (struct synch_state *) magic;
    struct My_synch_state *mystate;
    mystate = (struct My_synch_state*)magic;
    int cmd = mystate->cmd;
    int* ret_var = &mystate->ret_var;
    switch (cmd) {
    case SNMP_CMD_getdeviceid:
    {
        state->waiting = 0;
        ret = 0;
        char* buffer = mystate->buffer;
        int bufsize = mystate->bufsize;
        struct variable_list *vars;
        for(vars = pdu->variables; vars; vars = vars->next_variable){
            if((ASN_OCTET_STR == vars->type) && (bufsize >= (int)vars->val_len)){
                memcpy(buffer ,vars->val.string ,vars->val_len);
                *ret_var = vars->val_len;
                break;
            }
        }
    }
        break;
    case SNMP_CMD_getdeviceid_broadcast:{
        state->waiting = 1;
        ret = 0;
        char* buffer = mystate->buffer;
        int bufsize = mystate->bufsize;
        struct variable_list *vars;
        for(vars = pdu->variables; vars; vars = vars->next_variable){
            if((ASN_OCTET_STR == vars->type)){
                if(bufsize >= (int)vars->val_len){
                    memcpy(buffer ,vars->val.string ,vars->val_len);
                    *ret_var = vars->val_len;
                    break;
                }
            }
        }

        char ip[256];
        struct sockaddr_in *to4 = NULL;
        to4 = (struct sockaddr_in *)pdu->transport_data;
        if(to4->sin_family == AF_INET){
            strcpy(ip ,inet_ntoa(to4->sin_addr));
        }else{
            struct sockaddr_in6 *to6 = NULL;
            to6 = (struct sockaddr_in6 *)pdu->transport_data;
            if(to6->sin6_family == AF_INET6){
                inet_ntop(AF_INET6 ,&to6->sin6_addr ,ip ,256);
            }
        }
        if(mystate->callback){
            mystate->callback(ip ,buffer ,*ret_var ,mystate->data);
        }
    }
        break;
    case SNMP_CMD_search:
        break;
    default:
        break;
    }
    return ret;
}

static int  callback(int op,
              netsnmp_session * session,
              int reqid, netsnmp_pdu *pdu, void *magic)
{
    struct synch_state *state = (struct synch_state *) magic;
    int             rpt_type;
    int ret = 1;
    if (reqid != state->reqid && pdu && pdu->command != SNMP_MSG_REPORT) {
        LOGLOG("snmp:Unexpected response (ReqID: %d,%d - Cmd %d)",
                                   reqid, state->reqid, pdu->command );
        return 0;
    }
    if (op == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE && pdu) {
        if (pdu->command == SNMP_MSG_RESPONSE) {
            ret = handlerData(pdu ,magic);
        }else if (pdu->command == SNMP_MSG_REPORT) {
            rpt_type = snmpv3_get_report_type(pdu);
            if (SNMPV3_IGNORE_UNAUTH_REPORTS ||
                rpt_type == SNMPERR_NOT_IN_TIME_WINDOW) {
                state->waiting = 1;
            }
            state->pdu = NULL;
            state->status = STAT_ERROR;
            session->s_snmp_errno = rpt_type;
            SET_SNMP_ERROR(rpt_type);
        }else {
            char msg_buf[50];
            state->status = STAT_ERROR;
            session->s_snmp_errno = SNMPERR_PROTOCOL;
            SET_SNMP_ERROR(SNMPERR_PROTOCOL);
            snprintf(msg_buf, sizeof(msg_buf), "Expected RESPONSE-PDU but got %s-PDU",
                     snmp_pdu_type(pdu->command));
            snmp_set_detail(msg_buf);
            return 0;
        }
    }else if (op == NETSNMP_CALLBACK_OP_TIMED_OUT) {
        state->pdu = NULL;
        state->status = STAT_TIMEOUT;
        session->s_snmp_errno = SNMPERR_TIMEOUT;
        SET_SNMP_ERROR(SNMPERR_TIMEOUT);
    } else if (op == NETSNMP_CALLBACK_OP_DISCONNECT) {
        state->pdu = NULL;
        state->status = STAT_ERROR;
        session->s_snmp_errno = SNMPERR_ABORT;
        SET_SNMP_ERROR(SNMPERR_ABORT);
    }
    return ret;//1:still alive ,0:end
}

int snmp_handler(netsnmp_session& session ,void* magic)
{
    netsnmp_session *ss;
    netsnmp_pdu *pdu;
    ss = snmp_open (&session);
    int liberr,syserr;
    char* errstr;
    if (ss == NULL){
        snmp_error(&session ,&liberr ,&syserr ,&errstr);
        LOGLOG("snmp_open:%s" ,errstr);
        free(errstr);
        return (1);
    }
    struct My_synch_state *mystate;
    mystate = (struct My_synch_state*)magic;
    struct synch_state *state;
    state = (struct synch_state*)magic;

    pdu = snmp_pdu_create (SNMP_MSG_GET);
    snmp_add_null_var (pdu, mystate->oidname, mystate->sizeof_oidname);

    ss->callback = callback;
    ss->callback_magic = (void*) state;
    state->reqid = snmp_send(ss ,pdu);
    state->waiting = 1;
    if(state->reqid == 0){
        snmp_free_pdu(pdu);
        state->status = STAT_ERROR;
    }else{
        fd_set fdset;
        int block;
        int numfds ,count;
        struct timeval timeout ,*tvp;
        while(state->waiting){
            numfds = 0;
            FD_ZERO(&fdset);
            block = NETSNMP_SNMPBLOCK;
            timeout.tv_sec = TimeOutSecond;
            timeout.tv_usec = 0;
            tvp=&timeout;
            timerclear(tvp);
            snmp_select_info(&numfds ,&fdset ,tvp ,&block);
//            snmp_sess_select_info_flags(0 ,&numfds ,&fdset ,tvp ,&block ,NETSNMP_SELECT_NOALARMS);
//            if(block == 1)
//                tvp=NULL;
            count = select(numfds ,&fdset ,NULL ,NULL ,tvp);
            if(count > 0){
                snmp_read(&fdset);
            }else{
                switch (count) {
                case 0:
                    snmp_timeout();
//                    LOGLOG("snmp:time out");
                    break;
                case -1:
//                    if(errno == EINTR)
//                        continue;
//                    else
//                        snmp_errno = SNMPERR_GENERR;
                default:
                    state->status = STAT_ERROR;
                    LOGLOG("snmp:something error");
                    break;
                }
                break;
            }
        }
    }
//    if(mystate->ret_var){
//        snmp_error(ss ,&liberr ,&syserr ,&errstr);
//        LOGLOG("snmp_synch_response:%s" ,errstr);
//        free(errstr);
//    }
    snmp_close (ss);
    return 0;

}

void snmp_cancel()
{
    snmp_close_sessions();
}

static int snmp_get_oid(char* ip ,struct My_synch_state* lstate)
//static int snmp_get_oid(char* ip ,char* buffer ,int bufsize ,const oid* oidname ,size_t lengthofoid)
{
    netsnmp_session session;
//    init_snmp ("snmp");
    snmp_sess_init (&session);
    session.version = SNMP_VERSION_2c;
    session.community = (u_char*)"public";
    session.community_len = strlen ((const char*)session.community);
    session.peername = ip;
    session.timeout =  TimeOutSecond *1000 * 1000;
    if(lstate->cmd == SNMP_CMD_getdeviceid_broadcast){
        session.flags |= SNMP_FLAGS_UDP_BROADCAST;
    }

    lstate->ret_var = -1;
    snmp_handler(session ,(void*)lstate);
    return lstate->ret_var;
}

int snmp_get_deviceid(char* ip ,char* buffer ,int bufsize)
{
    struct My_synch_state lstate;
    memset((void*)&lstate ,0 ,sizeof(lstate));
    lstate.buffer = buffer;
    lstate.bufsize = bufsize;
    lstate.cmd = SNMP_CMD_getdeviceid;
    lstate.oidname = oid_status;
    lstate.sizeof_oidname = sizeof(oid_status)/sizeof(oid_status[0]);
    lstate.callback = NULL;
    int ret = snmp_get_oid(ip ,&lstate);
//    int ret = snmp_get_oid(ip ,buffer ,bufsize ,oid_status ,sizeof(oid_status)/sizeof(oid_status[0]));
    return ret < 0 ?-1 :0;
}

int snmp_get_deviceid_broadcast(char *ip, char *buffer, int bufsize ,callback_snmp callback ,void* data)
{
    struct My_synch_state lstate;
    memset((void*)&lstate ,0 ,sizeof(lstate));
    lstate.buffer = buffer;
    lstate.bufsize = bufsize;
    lstate.cmd = SNMP_CMD_getdeviceid_broadcast;
    lstate.oidname = oid_status;
    lstate.sizeof_oidname = sizeof(oid_status)/sizeof(oid_status[0]);
    lstate.callback = callback;
    lstate.data = data;
    int ret = snmp_get_oid(ip ,&lstate);
    return ret < 0 ?-1 :0;
}

//sometimes can not time out.
int snmpGetResponse(char* ip ,char* buffer ,int bufsize ,const oid* oidname ,size_t lengthofoid)
{
    bool ret = -1;
    netsnmp_session session;
//    init_snmp ("snmp");
    snmp_sess_init (&session);
    session.version = SNMP_VERSION_2c;
    session.community = (u_char*)"public";
    session.community_len = strlen ((const char*)session.community);
    session.peername = ip;
    session.timeout = TimeOutSecond *1000 * 1000;
    netsnmp_session *ss;
    netsnmp_pdu *pdu;
    ss = snmp_open (&session);
    int liberr,syserr;
    char* errstr;
    if (ss == NULL){
//        snmp_sess_perror ("snmp_open", &session);
        snmp_error(&session ,&liberr ,&syserr ,&errstr);
        LOGLOG("snmp_open:%s" ,errstr);
        free(errstr);
        return (1);
    }

    pdu = snmp_pdu_create (SNMP_MSG_GET);
    snmp_add_null_var (pdu, oidname, lengthofoid);

    netsnmp_pdu *response;
    int status = snmp_synch_response (ss, pdu, &response);
    if (status != STAT_SUCCESS || !response){
//        snmp_sess_perror ("snmp_synch_response", ss);
        snmp_error(ss ,&liberr ,&syserr ,&errstr);
        LOGLOG("snmp_synch_response:%s" ,errstr);
        free(errstr);
    }else{
        netsnmp_variable_list *vars;
        if (response->errstat == SNMP_ERR_NOERROR) {
            for (vars = response->variables; vars;
                 vars = vars->next_variable)
//                print_variable(vars->name, vars->name_length, vars);
                if((ASN_OCTET_STR == vars->type) && (bufsize >= (int)vars->val_len)){
                    memcpy(buffer ,vars->val.string ,vars->val_len);
                    ret = 0;
                }

        }else{
            LOGLOG("snmp: Error in packet: %s\n",snmp_errstring (response->errstat));
        }
        snmp_free_pdu (response);
    }
    snmp_close (ss);
    return ret;
}
#endif
