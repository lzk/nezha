#ifndef HAVE_STRTOULL
#define HAVE_STRTOULL 1
#endif
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "log.h"
//enterprises.2699.1.2.1.2.1.1.3.1
//enterprises.26266.86.10.1.1.1.1.0
static const oid oid_status[] = {1,3,6,1,4,1,2699,1,2,1,2,1,1,3,1};

#define TimeOutSecond 5
enum{
    SNMP_CMD_getdeviceid,
    SNMP_CMD_search
};

struct My_synch_state{
    struct synch_state state;
    int cmd;
    char* buffer;
    int bufsize;
    int ret_var;
    const oid* oidname;
    size_t sizeof_oidname;
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
                *ret_var = 0;
            }
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
                    LOGLOG("snmp:time out");
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

static int snmp_get_oid(char* ip ,char* buffer ,int bufsize ,const oid* oidname ,size_t lengthofoid)
{
    netsnmp_session session;
//    init_snmp ("snmp");
    snmp_sess_init (&session);
    session.version = SNMP_VERSION_2c;
    session.community = (u_char*)"public";
    session.community_len = strlen ((const char*)session.community);
    session.peername = ip;
    session.timeout =  TimeOutSecond *1000 * 1000;

    struct My_synch_state lstate;
    memset((void*)&lstate ,0 ,sizeof(lstate));
    lstate.buffer = buffer;
    lstate.bufsize = bufsize;
    lstate.cmd = SNMP_CMD_getdeviceid;
    lstate.ret_var = -1;
    lstate.oidname = oidname;
    lstate.sizeof_oidname = lengthofoid;
    snmp_handler(session ,(void*)&lstate);
    return lstate.ret_var;
}

int snmp_get_deviceid(char* ip ,char* buffer ,int bufsize)
{
    return snmp_get_oid(ip ,buffer ,bufsize ,oid_status ,sizeof(oid_status)/sizeof(oid_status[0]));
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
