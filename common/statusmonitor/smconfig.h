#ifndef SMCONFIG_H
#define SMCONFIG_H

#include "jkinterface.h"
typedef bool (*func_isDeviceSupported)(Printer_struct*);

extern const char* status_file;
extern const char* status_lock_file;
extern func_isDeviceSupported isDeviceSupported;
class SMConfig : public JKConfig
{
public:
    SMConfig();

};

#endif // SMCONFIG_H
