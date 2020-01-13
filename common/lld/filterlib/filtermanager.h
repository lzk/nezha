#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include "filterlib.h"
class FilterManager
{
public:
    FilterManager(filterlib_struct*);

    static int filtermanager_job_abort();
    static int filtermanager_job(filterlib_struct*);
    static int filtermanager_job_exit();
private:
    bool g_abort;
    filterlib_struct g_para;

    int authorize(filterlib_struct*);
    int abort();
    static int result(filterlib_struct* para);
};

#endif // FILTERMANAGER_H
