#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <stdio.h>
#include <QStringList>
#include "statusmonitor.h"
#include "jkinterface.h"
class StatusManager: public FileLocker
{
public:
    StatusManager();

    int savePrinterInfoToFile(const char* printer ,PrinterInfo_struct* printerinfo);
    int getPrinterInfoFromFile(const char* printer ,PrinterInfo_struct* printerinfo);
    int saveStatusToFile(const char* printer ,PRINTER_STATUS* status);
    int getStatusFromFile(const char* printer ,PRINTER_STATUS* status);
    int clearFile();
    int clearPrintersOfFile();
    int savePrinterToFile(Printer_struct* printer);
    int savePrintersToFile(QList<Printer_struct > printers);
    int getPrintersFromFile(CALLBACK_getPrinters ,void*);

};

#endif // STATUSMANAGER_H
