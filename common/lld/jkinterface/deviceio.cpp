#include "deviceio.h"
#include "log.h"
#include <string.h>
#include "api_libcups.h"

int DeviceIO::resolveUrl(const char* url)
{
    int ret = -1;
    if(url){
        strcpy(device_uri ,url);
        ret = 0;
    }
    return ret;
}

int DeviceIO::open(Printer_struct* printer ,int port)
{
    int ret = 0;
    ret = resolve(printer);
    if(!ret)
        ret = open(port);
    return ret;
}

int DeviceIO::resolve(Printer_struct* printer)
{
    return resolveUrl(printer->deviceUri);
}

bool DeviceIO::isConnected(Printer_struct* printer)
{
    if(!resolve(printer)){
        return isConnected();
    }else
        return false;
}

const char* DeviceIO::getDeviceAddress(Printer_struct* printer)
{
    if(!resolve(printer)){
        return getDeviceAddress();
    }else
        return "";
}

int DeviceIO::getDeviceId(Printer_struct* printer ,char *buffer, int bufsize)
{
    if(!resolve(printer)){
        return getDeviceId(buffer ,bufsize);
    }else
        return -1;
}

bool DeviceIO::is_the_same_device(Printer_struct* printer)
{
    return !strcmp(printer->deviceUri ,device_uri);
}
