#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include "statusmonitor.h"
#include <QMetaType>
//typedef struct{
//    PrinterStatus_struct status;
//    ErrorInfo_struct errorInfo;
//}
//    Status_struct;

Q_DECLARE_METATYPE(PrinterInfo_struct)
Q_DECLARE_METATYPE(QList<PrinterInfo_struct>)
Q_DECLARE_METATYPE(QList<Job_struct>)

#include <QObject>
class DataStruct : public QObject {
    Q_OBJECT
public:
    enum CmdType{
        CMD_GetDefaultPrinter,
        CMD_GetPrinters,
        CMD_GetStatus,
        CMD_GetJobs,
    };
    Q_ENUMS(CmdType)
};

#endif // DATASTRUCT_H
