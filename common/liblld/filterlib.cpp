#include "filterlib.h"
#include "jkinterface.h"
#include "filterlibconfig.h"
#include <string.h>
#include "filtermanager.h"
#include <pthread.h>
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif

static bool isusb;
static bool abort;
#include <cups/sidechannel.h>

static int trans_result(filterlib_struct* para)
{
    if(!para)
        return -1;
    Trans_Client tc(SERVER_PATH);
    char buffer[256];
    sprintf(buffer ,"result://%s?jobid=%d&status=%d&username=%s&filename=%s"
            ,para->printername ,para->jobid ,para->status
            ,para->username ,para->filename);
    tc.writeThenRead(buffer ,sizeof(buffer));
    if(!strcmp(buffer ,"resultok"))
    {
        LOGLOG("filterlib: result ok");
    }
//    LOGLOG("filterlib: filterlib_result end");
    return 0;
}

static void get_status(void* para)
{
    filterlib_struct* filterlib_para = (filterlib_struct*) para;
//    int result;
    static int count = 0;
    char data[1025];
    int datalen = 1024;
    memset(data ,0 ,sizeof(data));

    Trans_Client tc(SERVER_PATH);
    char buffer[1024];
    memset(buffer ,0 ,sizeof(buffer));

#ifdef DEBUG_DEBUG
//        result =
        sprintf(data ,"STS:AB32");
#else
//        result = cups_usb_getDeviceID(data ,datalen);
        cups_sc_status_t status;
        status = cupsSideChannelDoRequest(CUPS_SC_CMD_GET_DEVICE_ID, data, &datalen, 1.0);
#endif
    LOGLOG("filterlib: get device id %d: %s" ,count++ ,data);

    sprintf(buffer ,"dvid://%s?deviceid=%s" ,filterlib_para->printername ,data);
    tc.writeThenRead(buffer ,1024);

//        str = QString("echo %1 >> /tmp/abcd.log").arg(data);
//        system(str.toLatin1().constData());
}

static void* filterThread(void* para)
{
    pthread_detach(pthread_self());
    LOGLOG("filterlib: thread start");

    abort = false;
    get_status((void *)para);
    sleep(1);
    while (1) {
        if (abort){
            break;
        }
//        LOGLOG("filterlib: get device id %x" ,getpid());

        get_status(para);
        sleep(6);
    }
    abort = false;
    LOGLOG("filterlib: thread exit");
    return NULL;
}

const char* log_app_name;
const char* app_version;
const char* log_file;
int filterlib(filterlib_struct* para)
{
    log_app_name = EXE_NAME;
    app_version = APP_VERSION;
    log_file = LOG_FILE_NAME;
    log_init();
    LOGLOG("--------%s v%s-------" ,log_app_name ,app_version);

    int ret;
    char deviceuri[256];
    ret = cups_get_device_uri(para->printername ,deviceuri);
    LOGLOG("filterlib: enter:deviceuri %s" ,deviceuri);
    isusb = false;
    if(!ret && (strstr(deviceuri ,"usb://") != NULL)){
        LOGLOG("filterlib: now create thread");
        pthread_t thread;
        ret = pthread_create(&thread, NULL, filterThread, (void *)para);
        if (ret){
            LOGLOG("filterlib: can not create thread");
            isusb = true;
            ret = 0;
        }
    }
    LOGLOG("filterlib: create thread ok");
//    ret = FilterManager::filtermanager_job(para);
//    trans_result(para);
    return ret;
}

int filterlib_exit()
{
    if(isusb){
        abort = true;
        while (abort) {
            usleep(1000);
        }
    }
//    return FilterManager::filtermanager_job_exit();
    LOGLOG("filterlib: exit");
    return 0;
}

int filterlib_abort()
{
    LOGLOG("filterlib: abort");
//    return FilterManager::filtermanager_job_abort();
    return 0;
}

#ifdef __cplusplus
}
#endif
