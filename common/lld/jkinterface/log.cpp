#include "log.h"
#if 1
#include "lld.h"

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
//#ifndef LOG_TO_STDERR
#if 0
#include <QMutex>
static QMutex mutex_write_log_file;
char tmp_filename[1024];
class JKLog
{
public:
  JKLog();
  ~JKLog();
  void init();
  int log(const char*  ,va_list pArgs);

private:
  FILE* file;
};

JKLog::JKLog()
    :file(NULL)
{
}

void JKLog::init()
{
    if(!strcmp(log_file, "/dev/stderr")){
        struct timeb tp;
        ftime(&tp);
        time_t t;
        struct tm *p_time;
        time(&t);
        p_time = gmtime(&t);

        sprintf(tmp_filename ,"/tmp/%s_%04d%02d%02d%02d%02d%02d.log"
                ,log_app_name
                ,p_time->tm_year + 1900 ,p_time->tm_mon + 1 ,p_time->tm_mday
                ,p_time->tm_hour ,p_time->tm_min ,p_time->tm_sec);
        log_file = tmp_filename;
    }
    file = fopen(log_file ,"w+");
    chmod(log_file ,DEFFILEMODE);
}

JKLog::~JKLog()
{
    if(file)
        fclose(file);
}

int JKLog::log(const char* para ,va_list pArgs)
{
    QMutexLocker locker(&mutex_write_log_file);
    if(!file)
        return -1;
    int ret = vfprintf(file ,para ,pArgs);
    fflush(file);
    return ret;
}

static JKLog gJKLog;
int jklog(const char* para ,...)
{
    int result;
    va_list pArgs;
    va_start(pArgs, para);
    result = gJKLog.log(para ,pArgs);
    va_end(pArgs);
    return result;
}
void log_init()
{
    gJKLog.init();
}
#else
void log_init()
{
    FILE* file;
    file = fopen(log_file ,"w+");
    fclose(file);
    chmod(log_file ,DEFFILEMODE);
}

#include <unistd.h>
static int llog(const char* para ,va_list pArgs)
{
//    QMutexLocker locker(&mutex_write_log_file);
    FILE* file;
    file = fopen(log_file ,"a+");
    if(!file)
        return -1;

    struct timeb tp;
    ftime(&tp);
    time_t t;
    struct tm *p_time;
    time(&t);
    p_time = gmtime(&t);

    fprintf(file ,"%04d%02d%02d%02d%02d%02d"
            ,p_time->tm_year + 1900 ,p_time->tm_mon + 1 ,p_time->tm_mday
            ,p_time->tm_hour ,p_time->tm_min ,p_time->tm_sec);

    fprintf(file ,"---%d---:" ,getpid());

    int ret = vfprintf(file ,para ,pArgs);
    fprintf(file ,"\n");
    fflush(file);
    fclose(file);
    return ret;
}

int jklog(const char* para ,...)
{
    int result;
    va_list pArgs;
    va_start(pArgs, para);
    result = llog(para ,pArgs);
    va_end(pArgs);
    return result;
}
#endif

#else

void log_init()
{
//    FILE* file;
//    file = fopen(log_file ,"w+");
//    fclose(file);
}
#endif
