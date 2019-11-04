#include "smconfig.h"

static bool _isDeviceSupported(Printer_struct* ps)
{
    LOGLOG("found device name:%s \n\tmodel:%s" ,ps->name,ps->makeAndModel);
    return true;
}

func_isDeviceSupported isDeviceSupported = _isDeviceSupported;

SMConfig::SMConfig()
{

}
