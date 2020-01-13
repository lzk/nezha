#include <sys/statfs.h>
#include "log.h"

bool is_disk_no_space(const char* path ,unsigned int prefer_size)
{
    struct statfs tmp_buf;
    unsigned int t_space_size = prefer_size;
    if(!statfs(path ,&tmp_buf)){
        t_space_size = (((unsigned long long)tmp_buf.f_bavail * tmp_buf.f_bsize) >> 20);
//        LOGLOG("tmp_buf.f_bavail:%ld" ,tmp_buf.f_bavail);
//        LOGLOG("tmp_buf.f_bsize:%ld" ,tmp_buf.f_bsize);
    }
    LOGLOG("device space:%ld M" ,t_space_size);
    return t_space_size < prefer_size;
}
