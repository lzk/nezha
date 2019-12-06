#include "statusmanager.h"
#include <QSettings>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include <sys/file.h>
#include "jkinterface.h"
#include "smconfig.h"

const char* status_file = "/tmp/.status";
const char* status_lock_file = "/tmp/.lockstatus";
const char* statusKey = "printer/status/";
const char* printersKey = "printer/mdl/";
const char* printerinfoKey = "printer/info/";
static int saveStatus(const char* printer ,PRINTER_STATUS* status)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(statusKey) + printer;

    settings.beginGroup(key);
    settings.setValue("TonelStatusLevelK" ,status->TonelStatusLevelK);
    settings.setValue("TonelStatusLevelC" ,status->TonelStatusLevelC);
    settings.setValue("TonelStatusLevelM" ,status->TonelStatusLevelM);
    settings.setValue("TonelStatusLevelY" ,status->TonelStatusLevelY);

//    BYTE	TonelStatusLevelK;
//    BYTE	TonelStatusLevelC;
//    BYTE	TonelStatusLevelM;
//    BYTE	TonelStatusLevelY;

    settings.setValue("DrumStatusLifeRemain" ,status->DrumStatusLifeRemain);
    settings.setValue("CoverStatusFlags" ,status->CoverStatusFlags);
    settings.setValue("PaperTrayStatus" ,status->PaperTrayStatus);
    settings.setValue("PaperSize" ,status->PaperSize);
//    BYTE	DrumStatusLifeRemain;
//    BYTE	CoverStatusFlags;
//    BYTE	PaperTrayStatus;
//    BYTE	PaperSize;

    settings.setValue("OutputTrayLevel" ,status->OutputTrayLevel);
    settings.setValue("PrinterStatus" ,status->PrinterStatus);
    settings.setValue("OwnerName" ,status->OwnerName);
    settings.setValue("DocuName" ,status->DocuName);
//    BYTE	OutputTrayLevel;
//    BYTE	PrinterStatus;
////	wchar_t	OwnerName[16];
////	wchar_t	DocuName[16];
//    char	OwnerName[32];
//    char	DocuName[32];

    settings.setValue("ErrorCodeGroup" ,status->ErrorCodeGroup);
    settings.setValue("ErrorCodeID" ,status->ErrorCodeID);
    settings.setValue("PrintingPage" ,status->PrintingPage);
    settings.setValue("Copies" ,status->Copies);
//    BYTE	ErrorCodeGroup;
//    BYTE	ErrorCodeID;
//    WORD	PrintingPage;
//    WORD	Copies;
    settings.setValue("TotalCounter" ,status->TotalCounter);
//    settings.setValue("TRCCurve" ,status->TRCCurve);
//    settings.setValue("TonerSize" ,status->TonerSize);
//    DWORD	TotalCounter;
//    BYTE	TRCCurve[12];

//    BYTE	TonerSize[4];

    settings.setValue("PaperType" ,status->PaperType);
    settings.setValue("NonDellTonerMode" ,status->NonDellTonerMode);
    settings.setValue("AioStatus" ,status->AioStatus);
//    BYTE	PaperType;
//    BYTE	NonDellTonerMode;
//    BYTE	AioStatus;
//    //BYTE	bReserved;

    settings.setValue("bPhyPrinterStatus" ,status->bPhyPrinterStatus);
    settings.setValue("LowTonerAlert" ,status->LowTonerAlert);
//    BYTE	bPhyPrinterStatus;
//    //WORD	wReserved1;
//    BYTE	LowTonerAlert;
//    BYTE	bReserved1;
//    WORD	wReserved2;
    settings.endGroup();
    settings.sync();
    return 0;
}

static int getStatus(const char* printer ,PRINTER_STATUS* status)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(statusKey) + printer;
    if(!settings.allKeys().contains(key + "/PrinterStatus")){
        return -1;
    }
    settings.beginGroup(key);
    status->TonelStatusLevelK = settings.value("TonelStatusLevelK").toInt();
    status->TonelStatusLevelC = settings.value("TonelStatusLevelC").toInt();
    status->TonelStatusLevelM = settings.value("TonelStatusLevelM").toInt();
    status->TonelStatusLevelY = settings.value("TonelStatusLevelY").toInt();

    status->DrumStatusLifeRemain = settings.value("DrumStatusLifeRemain").toInt();
    status->CoverStatusFlags = settings.value("CoverStatusFlags").toInt();
    status->PaperTrayStatus = settings.value("PaperTrayStatus").toInt();
    status->PaperSize = settings.value("PaperSize").toInt();

    status->OutputTrayLevel = settings.value("OutputTrayLevel").toInt();
    status->PrinterStatus = settings.value("PrinterStatus").toInt();
    strncpy(status->OwnerName ,settings.value("OwnerName").toString().toUtf8().constData() ,sizeof(status->OwnerName));
    strncpy(status->DocuName ,settings.value("DocuName").toString().toUtf8().constData() ,sizeof(status->DocuName));

    status->ErrorCodeGroup = settings.value("ErrorCodeGroup").toInt();
    status->ErrorCodeID = settings.value("ErrorCodeID").toInt();
    status->PrintingPage = settings.value("PrintingPage").toInt();
    status->Copies = settings.value("Copies").toInt();

    status->TotalCounter = settings.value("TotalCounter").toInt();
//    status->TRCCurve = settings.value("TRCCurve").toInt();
//    status->TonerSize = settings.value("TonerSize").toInt();

    status->PaperType = settings.value("PaperType").toInt();
    status->NonDellTonerMode = settings.value("NonDellTonerMode").toInt();
    status->AioStatus = settings.value("AioStatus").toInt();

    status->bPhyPrinterStatus = settings.value("bPhyPrinterStatus").toInt();
    status->LowTonerAlert = settings.value("LowTonerAlert").toInt();

    settings.endGroup();
    return 0;
}

static int clear()
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    settings.clear();
    settings.sync();
    return 0;
}

static int clearPrinters()
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(printersKey);
    settings.remove(key);
    settings.sync();
    return 0;
}

static int savePrinter(Printer_struct* printer)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(printersKey) +"/" + printer->name;
    settings.beginGroup(key);
    settings.setValue("name" ,printer->name);
    settings.setValue("makeAndModel" ,printer->makeAndModel);
    settings.setValue("deviceUri" ,printer->deviceUri);
    settings.setValue("connectTo" ,printer->connectTo);
    settings.setValue("isDefault" ,printer->isDefault);
    settings.setValue("isConnected" ,printer->isConnected);
    settings.setValue("status" ,printer->status);
    settings.endGroup();
    settings.sync();
    return 0;
}

static int getPrinter(CALLBACK_getPrinters callback,void* para)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(printersKey);
    settings.beginGroup(key);
    QStringList printers = settings.childGroups();
    settings.endGroup();
    foreach (QString printer, printers) {
        Printer_struct ps;
        settings.beginGroup(key + "/" + printer);
        strncpy(ps.name ,settings.value("name").toString().toUtf8().constData() ,sizeof(ps.name));
        strncpy(ps.makeAndModel ,settings.value("makeAndModel").toString().toUtf8().constData() ,sizeof(ps.makeAndModel));
        strncpy(ps.deviceUri ,settings.value("deviceUri").toString().toUtf8().constData() ,sizeof(ps.deviceUri));
        strncpy(ps.connectTo ,settings.value("connectTo").toString().toUtf8().constData() ,sizeof(ps.connectTo));
        ps.isDefault = settings.value("isDefault").toBool();
        ps.isConnected = settings.value("isConnected").toBool();
        ps.status = settings.value("status").toInt();
        settings.endGroup();
        if(callback){
            callback(para ,&ps);
        }
    }
    return 0;
}

static int savePrinterinfo(const char* printername ,PrinterInfo_struct* printerinfo)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(printerinfoKey) + printername;

    Printer_struct* printer = &printerinfo->printer;
    settings.beginGroup(key + "/printer");
    settings.setValue("name" ,printer->name);
    settings.setValue("makeAndModel" ,printer->makeAndModel);
    settings.setValue("deviceUri" ,printer->deviceUri);
    settings.setValue("connectTo" ,printer->connectTo);
    settings.setValue("isDefault" ,printer->isDefault);
    settings.setValue("isConnected" ,printer->isConnected);
    settings.setValue("status" ,printer->status);
    settings.endGroup();
    if(printer->status){//not valid status
        settings.sync();
        return 0;
    }

    PrinterStatus_struct* status = &printerinfo->status;
    settings.beginGroup(key + "/status");
    settings.setValue("PrinterStatus" ,status->PrinterStatus);
    settings.setValue("trayPaperTrayStatus" ,status->trayPaperTrayStatus);
    settings.setValue("TonelStatusLevelC" ,status->TonelStatusLevelC);
    settings.setValue("TonelStatusLevelM" ,status->TonelStatusLevelM);
    settings.setValue("TonelStatusLevelY" ,status->TonelStatusLevelY);
    settings.setValue("TonelStatusLevelK" ,status->TonelStatusLevelK);
    settings.setValue("ErrorCodeID" ,status->ErrorCodeID);
    settings.setValue("PaperSize" ,status->PaperSize);
    settings.setValue("PaperType" ,status->PaperType);
    settings.setValue("NonDellTonerMode" ,status->NonDellTonerMode);
    settings.setValue("LowTonerAlert" ,status->LowTonerAlert);
    settings.setValue("job" ,status->job);
    settings.setValue("drum" ,status->drum);
    settings.endGroup();
    settings.sync();
    return 0;
}

extern int usb_error_scanning;
extern int usb_error_usb_locked;
static int getPrinterinfo(const char* printername ,PrinterInfo_struct* printerinfo)
{
    QSettings settings(status_file ,QSettings::defaultFormat());
    QString key = QString(printerinfoKey) + printername;
    if(!settings.allKeys().contains(key + "/printer/name")){
        return -1;
    }
    Printer_struct* printer = &printerinfo->printer;
    settings.beginGroup(key + "/printer");
    strncpy(printer->name ,settings.value("name").toString().toUtf8().constData() ,sizeof(printer->name));
    strncpy(printer->makeAndModel ,settings.value("makeAndModel").toString().toUtf8().constData() ,sizeof(printer->makeAndModel));
    strncpy(printer->deviceUri ,settings.value("deviceUri").toString().toUtf8().constData() ,sizeof(printer->deviceUri));
    strncpy(printer->connectTo ,settings.value("connectTo").toString().toUtf8().constData() ,sizeof(printer->connectTo));
    printer->isDefault = settings.value("isDefault").toBool();
    printer->isConnected = settings.value("isConnected").toBool();
    printer->status = settings.value("status").toInt();
    settings.endGroup();

    PrinterStatus_struct* status = &printerinfo->status;
    settings.beginGroup(key + "/status");
    status->PrinterStatus = settings.value("PrinterStatus").toInt();
    status->trayPaperTrayStatus = settings.value("trayPaperTrayStatus").toInt();
    status->TonelStatusLevelK = settings.value("TonelStatusLevelK").toInt();
    status->TonelStatusLevelC = settings.value("TonelStatusLevelC").toInt();
    status->TonelStatusLevelM = settings.value("TonelStatusLevelM").toInt();
    status->TonelStatusLevelY = settings.value("TonelStatusLevelY").toInt();
    status->ErrorCodeID = settings.value("ErrorCodeID").toInt();
    status->PaperSize = settings.value("PaperSize").toInt();
    status->PaperType = settings.value("PaperType").toInt();
    status->NonDellTonerMode = settings.value("NonDellTonerMode").toInt();
    status->LowTonerAlert = settings.value("LowTonerAlert").toInt();
    status->job = settings.value("job").toInt();
    status->drum = settings.value("drum").toInt();
    settings.endGroup();
    settings.sync();
    if(printer->status == usb_error_scanning){
        printer->status = 0;
        printer->isConnected = true;
        status->PrinterStatus = usb_error_scanning;
    }
    return 0;
}

StatusManager::StatusManager()
{
}

int StatusManager::savePrinterInfoToFile(const char* printer ,PrinterInfo_struct* printerinfo)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = savePrinterinfo(printer ,printerinfo);
        unlock();
    }
    return ret;
}

int StatusManager::getPrinterInfoFromFile(const char* printer ,PrinterInfo_struct* printerinfo)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = getPrinterinfo(printer ,printerinfo);
        unlock();
    }
    return ret;
}
int StatusManager::saveStatusToFile(const char* printer ,PRINTER_STATUS* status)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = saveStatus(printer ,status);
        unlock();
    }
    return ret;
}

int StatusManager::getStatusFromFile(const char* printer ,PRINTER_STATUS* status)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = getStatus(printer ,status);
        unlock();
    }
    return ret;
}

int StatusManager::clearFile()
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = clear();
        unlock();
    }
    return ret;
}

int StatusManager::clearPrintersOfFile()
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = clearPrinters();
        unlock();
    }
    return ret;
}

int StatusManager::getPrintersFromFile(CALLBACK_getPrinters callback,void* para)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = getPrinter(callback ,para);
        unlock();
    }
    return ret;
}

int StatusManager::savePrinterToFile(Printer_struct* printer)
{
    int ret;
    ret = lock(status_lock_file);
    if(!ret){
        ret = savePrinter(printer);
        unlock();
    }
    return ret;
}

int StatusManager::savePrintersToFile(QList<Printer_struct > printers)
{
    int ret;
    ret = lock(status_lock_file);
    ret = clearPrinters();
    if(!ret){
        foreach (Printer_struct ps, printers) {
            ret = savePrinter(&ps);
        }
        unlock();
    }
    return ret;
}
