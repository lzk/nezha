#ifndef SCANNERAPI_H
#define SCANNERAPI_H

#include "jkinterface.h"
#include "scannerapp.h"

typedef
struct{
    int status;
    int data_size;
    int error_code;
}
    ScanStatus;
enum{
    ScanStatus_Scanning,
    ScanStatus_End,
};

class ScannerApi
{
public:
    ScannerApi(DeviceIOManager* _device_manager);

    int open(Printer_struct*);
    int close();
    int lock();
    int unlock();
    int set_parameters(ScanSettings*);
    int start();
    int stop();
    int abort();
    int get_scan_status(ScanStatus* status);
    int get_scan_data(char* buffer ,int buffer_size);

private:
    DeviceIO* device;
    DeviceIOManager* device_manager;
//    bool scanner_locked;
    int get_cmd_status();
};

#endif // SCANNERAPI_H
