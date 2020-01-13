#ifndef API_LIBCUPS_H
#define API_LIBCUPS_H

typedef
struct Printer_struct{
    char name[256];
    char makeAndModel[256];
    char deviceUri[256];
//    char scheme[256];
//    char hostname[256];
//    char resource[256];
//    char username[256];
//    int port;
    char connectTo[256];
    bool isDefault;
    bool isConnected;
    int status;
    int toner;
    int drum;
}
Printer_struct;

typedef int (*CALLBACK_getPrinters)(void* ,Printer_struct*);

typedef enum 		/**** Job states ****/
{
  JOBSTATE_PENDING = 3,		/* Job is waiting to be printed */
  JOBSTATE_HELD,			/* Job is held for printing */
  JOBSTATE_PROCESSING,		/* Job is currently printing */
  JOBSTATE_STOPPED,			/* Job has been stopped */
  JOBSTATE_CANCELED,			/* Job has been canceled */
  JOBSTATE_ABORTED,			/* Job has aborted due to error */
  JOBSTATE_COMPLETED			/* Job has completed successfully */
}
    JOBSTATE_ENUM;

typedef struct Job_struct{
    char name[256];
    char time[256];
    char printer[256];
    char user_name[256];
//    char message[1024];
//    char alerts[1024];
    int id;
    int document_size;
    int pages_completed;
    int copies;
    int state;
    long long timet;

}
    Job_struct;
typedef void (*CALLBACK_getJob)(void* ,Job_struct*);

const char* cups_get_default_printer();
int cups_get_job(CALLBACK_getJob callback,void* para,const char* printer = 0 ,const char* which = 0 ,const char* users = 0);
int cups_get_device_uri(const char* printer ,char* device_uri);
int cups_get_printers(CALLBACK_getPrinters callback ,void* para);
int snmpGetDeviceID(const char* device_uri ,char* buffer ,int bufsize);
int cups_resolve_uri(const char* device_uri ,char* buffer ,int bufsize);
int cups_usb_getDeviceID(char* data ,int datalen);

typedef int (*CALLBACK_getDevices)(const char* ,const char* ,void*);
int cups_getDevices(const char *include_schemes ,int timeout ,CALLBACK_getDevices func ,void*);

int cups_get_printer_status(const char  *device_url);/* O - not busy, 1 busy */
#endif // API_LIBCUPS_H
