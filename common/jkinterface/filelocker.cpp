#include "filelocker.h"
#include "log.h"
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <string.h>

FileLocker::FileLocker():
    fp(NULL)
{
}

int FileLocker::lock(const char* filename)
{
    int ret = -1;
//    LOGLOG("pid %ld get file %s lock" ,getpid() ,filename);
    fp = fopen(filename, "ab+");
    chmod(filename ,DEFFILEMODE);
    strcpy(lock_file ,filename);

    if(fp){
        int fd;
#ifdef JK_OS_MAC
        fd = fp->_file;
#else
        fd = fp->_fileno;
#endif
        ret = flock(fd, LOCK_EX);//will block
        if (ret){
            fclose(fp);
            fp = NULL;
        }
//        LOGLOG("pid %d file %s locked" ,getpid() ,filename);
    }else{
    }
    return ret;
}

//static int aaa = 0;
int FileLocker::trylock(const char* filename)
{
    int ret = -1;
    fp = fopen(filename, "ab+");
    chmod(filename ,0666);
    strcpy(lock_file ,filename);
    int fd;
    if(fp){
#ifdef JK_OS_MAC
        fd = fp->_file;
#else
        fd = fp->_fileno;
#endif
        ret = flock(fd, LOCK_EX | LOCK_NB);
        if (ret){
//            LOGLOG("not locked");
            fclose(fp);
            fp = NULL;
        }else{
//            LOGLOG("%s locked %d" ,filename ,++aaa);
        }
    }
    return ret;
}

int FileLocker::unlock()
{
    if(fp){
        int fd;
#ifdef JK_OS_MAC
        fd = fp->_file;
#else
        fd = fp->_fileno;
#endif
        int ret = flock(fd, LOCK_UN);
        fclose(fp);
        fp = NULL;
//        LOGLOG("pid %d unlocked" ,getpid());
        if(ret){
            LOGLOG("unlock fail,remove it!");
            remove(lock_file);
        }
//        LOGLOG("%s unlocked %d" ,lock_file ,aaa);
    }
    return 0;
}
