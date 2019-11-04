#include "log.h"
const char* log_file = "/dev/stderr";
const char* log_app_name = "statusmonitor";
const char* app_version = "1.0.0.1";

#ifndef LOG_TO_STDERR
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <string.h>
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

#include <QMutex>
static QMutex mutex_write_log_file;
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
}
#endif
