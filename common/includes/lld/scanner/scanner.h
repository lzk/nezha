#ifndef SCANNER_H
#define SCANNER_H

#include "scannerapp.h"
class Printer_struct;
class ScannerApi;
class Scanner
{
public:
    Scanner(ScannerApp* app);
    ~Scanner();

    int flat_scan(Printer_struct* printer ,ScanSettings* settings);
    int adf_scan(Printer_struct* printer ,ScanSettings* settings);
    void set_cancel(bool cancel){m_cancel = cancel;}
private:
    ScannerApi* scannner_api;
    ScannerApp* scanner_app;
    bool m_cancel;
    void exit_scan();
};

#endif // SCANNER_H
