#ifndef CUPSMANAGER_H
#define CUPSMANAGER_H

#include "api_libcups.h"
typedef int (*CALLBACK_getPrinter)(void* ,Printer_struct*);

class CupsManager
{
public:
    CupsManager();
    int getPrinters(CALLBACK_getPrinter ,void*);
    const char* getDefaultPrinter();
    int getJobs(CALLBACK_getJob ,void*,const char* printer = 0 ,const char* which = 0 ,const char* users = 0);
};

#endif // CUPSMANAGER_H
