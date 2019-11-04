#ifndef STATUSMONITOR_H
#define STATUSMONITOR_H

#include "status.h"
#include "jkinterface.h"
//#include <QList>
typedef
struct PrinterStatus_struct
{
    int PrinterStatus;
    int trayPaperTrayStatus;
    int	TonelStatusLevelC;
    int	TonelStatusLevelM;
    int	TonelStatusLevelY;
    int	TonelStatusLevelK;
    int ErrorCodeGroup;
    int ErrorCodeID;
    int PaperSize;
    int PaperType;
    int NonDellTonerMode;
    int LowTonerAlert;
    int job;
    int drum;
}
    PrinterStatus_struct;

typedef struct PrinterInfo_struct
{
    PrinterStatus_struct status;
    Printer_struct printer;
}
    PrinterInfo_struct;

//typedef int (*CALLBACK_getPrinterInfo)(void* ,PrinterInfo_struct*);
class StatusMonitor
{
    
public:
    StatusMonitor();

//    static int getPrinters(CALLBACK_getPrinterInfo ,void*);
//    static int getPrintersFromFile(CALLBACK_getPrinterInfo ,void*);
    static int getPrinterStatus(const char* printer ,PrinterStatus_struct* ps);
//    static int getDeviceStatus(DeviceIO* device ,PrinterStatus_struct* ps);
    static int getDeviceStatus(DeviceIO* device ,Printer_struct* printer ,PrinterStatus_struct* ps);
    static int getDeviceStatus(DeviceIOManager* device_manager ,Printer_struct* printer ,PrinterStatus_struct* ps);
    static int getDeviceStatus(DeviceIO* device ,PrinterInfo_struct* printer);

//    int get_printerlist(QList<Printer_struct >& printerlist);
//    int get_printerlist_from_file(QList<Printer_struct >& printerlist);
    static int getPrinters(CALLBACK_getPrinters ,void*);
    static int getPrintersFromFile(CALLBACK_getPrinters ,void*);
private:

};

#endif // STATUSMONITOR_H
