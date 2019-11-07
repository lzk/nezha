#include "filterjobhistory.h"
#include "jkinterface.h"
#include "filterlibconfig.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QSettings>
#include <QMutex>
static const char* job_history_file_name = JOBHISTORYFILE;
static const char* job_key ="jobs/id/";

static QMutex mutex;

enum{
    JobHistoryIndex_id,
    JobHistoryIndex_printername,
    JobHistoryIndex_hostname,
    JobHistoryIndex_username,
    JobHistoryIndex_filename,
    JobHistoryIndex_copies,
    JobHistoryIndex_result,
//    JobHistoryIndex_isFingerEnable,
//    JobHistoryIndex_isFingerChecked,
    JobHistoryIndex_pages,
    JobHistoryIndex_state,
    JobHistoryIndex_time,

    JobHistoryIndex_max,
};

FilterJobHistory::FilterJobHistory()
{

}

static void callback_getJob(void* para,Job_struct* js)
{
    filterlib_struct* job = (filterlib_struct*)para;
    if(js->id != job->jobid)
        return;

    char hostname[256];
    gethostname(hostname ,sizeof(hostname));
//    StatusMonitor* sm = (StatusMonitor*)para;
    char job_history[512];

    sprintf(job_history ,"%d/\\%s/\\%s/\\%s/\\%s/\\%d/\\%d"
//            ,js->id ,js->printer ,hostname,js->user_name  ,js->name
            ,js->id ,job->printername ,hostname,job->username  ,job->filename
             ,(js->copies < 1) ?1 :js->copies
             ,job->status
//            ,job->is_finger_enable,job->is_finger_checked//是，成功
            );
//    sprintf(job_history ,"echo %s >> %s" ,job_history ,job_history_file_name);
//    system(job_history);

    mutex.lock();
    QSettings settings(job_history_file_name ,QSettings::defaultFormat());
    QString key = QString().sprintf("%s%d/" ,job_key ,js->id);
    settings.setValue(key + "main" ,QString(job_history));
    settings.setValue(key + "pages" ,js->pages_completed);
    settings.setValue(key + "state" ,js->state);//JOBSTATE_COMPLETED?
    settings.setValue(key + "time" ,js->timet);
    settings.sync();
    mutex.unlock();
}
#include <QDateTime>
int FilterJobHistory::save_job_history(filterlib_struct* job)
{
#ifdef DEBUG_DEBUG
    Job_struct jbs;
    Job_struct* js = &jbs;
    js->id = job->jobid;
    js->copies = 1;
    js->pages_completed = 2;
    js->state = JOBSTATE_COMPLETED;
    js->timet = QDateTime::currentDateTime().toMSecsSinceEpoch();
    strcpy(js->printer ,job->printername);
    callback_getJob(job ,js);
#else
    cups_get_job(callback_getJob ,(void*)job);
#endif
    return 0;
}

bool sort_jobs(const QString &s1, const QString &s2)
{
    return s1.toInt() > s2.toInt();
}

int FilterJobHistory::get_job_history(QList<Job_history >& pJobs)
{
    mutex.lock();
    QSettings settings(job_history_file_name ,QSettings::defaultFormat());
    QString key(job_key);
    settings.beginGroup(key);
    QStringList jobs = settings.childGroups();
    settings.endGroup();

    qSort(jobs.begin() ,jobs.end() ,sort_jobs);
    int num_of_jobs = jobs.count();

    pJobs.clear();
    QString str;
    QStringList columns;
    Job_history jh;
    for(int i = 0 ;i < num_of_jobs ;i++){

        key = QString(job_key) + jobs.at(i) +"/";
        str = settings.value(key + "main").toString()
                +QString("/\\%1/\\%2/\\%3")
                .arg(settings.value(key + "pages").toInt())
                .arg(settings.value(key + "state").toInt())
                .arg(settings.value(key + "time").toInt());
        columns = str.split("/\\");

        if(columns.length() < JobHistoryIndex_max)
            break;

        jh.id = columns.at(JobHistoryIndex_id).toInt();
        jh.result = columns.at(JobHistoryIndex_result).toInt();
        jh.copies = columns.at(JobHistoryIndex_copies).toInt();
        jh.pages = columns.at(JobHistoryIndex_pages).toInt();
        jh.state = columns.at(JobHistoryIndex_state).toInt();
        jh.time = columns.at(JobHistoryIndex_time).toLongLong();
        jh.printername = columns.at(JobHistoryIndex_printername).toUtf8();
        jh.hostname = columns.at(JobHistoryIndex_hostname).toUtf8();
        jh.username = columns.at(JobHistoryIndex_username).toUtf8();
        jh.filename = columns.at(JobHistoryIndex_filename).toUtf8();
        pJobs << jh;
    }
    mutex.unlock();
    return 0;
}

int FilterJobHistory::get_job_history(QList<Job_history >& pJobs ,int page ,int countofpage)
{
    mutex.lock();
    QSettings settings(job_history_file_name ,QSettings::defaultFormat());
    QString key(job_key);
    settings.beginGroup(key);
    QStringList jobs = settings.childGroups();
    settings.endGroup();

    if(jobs.isEmpty())
        return 0;

    qSort(jobs.begin() ,jobs.end() ,sort_jobs);
    int num_of_jobs = jobs.count();
    int pages = num_of_jobs / countofpage + 1;
    if(page > pages){
        page = pages - 1;
        LOGLOG("out of page");
        return -1;
    }
    int index = page * countofpage;

    pJobs.clear();
    QString str;
    QStringList columns;
    Job_history jh;
    for(int i = 0 ;i < countofpage ;i++,index++){
        if(index > num_of_jobs - 1)
            break;
        key = QString(job_key) + jobs.at(index) +"/";
        str = settings.value(key + "main").toString()
                +QString("/\\%1/\\%2/\\%3")
                .arg(settings.value(key + "pages").toInt())
                .arg(settings.value(key + "state").toInt())
                .arg(settings.value(key + "time").toInt());
        columns = str.split("/\\");

        if(columns.length() < JobHistoryIndex_max)
            break;

        jh.id = columns.at(JobHistoryIndex_id).toInt();
        jh.result = columns.at(JobHistoryIndex_result).toInt();
        jh.copies = columns.at(JobHistoryIndex_copies).toInt();
        jh.pages = columns.at(JobHistoryIndex_pages).toInt();
        jh.state = columns.at(JobHistoryIndex_state).toInt();
        jh.time = columns.at(JobHistoryIndex_time).toLongLong();
        jh.printername = columns.at(JobHistoryIndex_printername);
        jh.hostname = columns.at(JobHistoryIndex_hostname);
        jh.username = columns.at(JobHistoryIndex_username);
        jh.filename = columns.at(JobHistoryIndex_filename);
        pJobs << jh;
    }

    mutex.unlock();
    return 0;
}



static void callback_updatJobList(void* para,Job_struct* js)
{
    QStringList jobs = *(QStringList*)para;
    if(!jobs.contains(QString("%1").arg(js->id))){
        return;
    }
    //update job state
    mutex.lock();
    QSettings settings(job_history_file_name ,QSettings::defaultFormat());
    QString key = QString().sprintf("%s%d/" ,job_key ,js->id);
    settings.setValue(key + "pages" ,js->pages_completed);
    settings.setValue(key + "state" ,js->state);//JOBSTATE_COMPLETED?
    settings.setValue(key + "time" ,js->timet);
    settings.sync();
    mutex.unlock();
}

int FilterJobHistory::update_job_history()
{
    mutex.lock();
    QSettings settings(job_history_file_name ,QSettings::defaultFormat());
    QString key(job_key);
    settings.beginGroup(key);
    QStringList jobs = settings.childGroups();
    settings.endGroup();
    mutex.unlock();
    cups_get_job(callback_updatJobList ,(void*)&jobs);
    return 0;
}
