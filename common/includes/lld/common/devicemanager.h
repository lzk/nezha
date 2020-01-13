#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QString>
#include "deviceio.h"
struct Printer_struct;
class DeviceManager :public DeviceIOManager
{
public:
    DeviceManager();
    ~DeviceManager();
    virtual DeviceIO* getDevice(Printer_struct* printer);
    static int getDeviceType(const char* uri);
    static int get_device_connect_status(Printer_struct* printer);
    static DeviceIO* new_device(Printer_struct* printer);
private:
    DeviceIO* device;
    DeviceIO* usbIO;
    DeviceIO* netIO;
    QString device_uri;
};

#endif // DEVICEMANAGER_H
