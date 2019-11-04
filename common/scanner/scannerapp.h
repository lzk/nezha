#ifndef SCANNERAPP_H
#define SCANNERAPP_H

#include <stdio.h>
#include <pthread.h>
#include "imagetrans.h"
#include "imageinfo.h"

//namespace Scanner {

#define MAX_BRIGHT 100
#define MIN_BRIGHT 0
#define MAX_CONTRAST 100
#define MIN_CONTRAST 0

enum Scan_DocType {T_Photo,T_Text_Graph, T_Text};
enum Scan_Dpi {Scan_100DPI, Scan_200DPI, Scan_300DPI, Scan_600DPI, Scan_1200DPI};
enum ColorModel {Color, Black_White, Grayscale};
enum Scan_Size {Scan_A4, Scan_A5, Scan_B5, Scan_Letter, Scan_4_6};
enum Scan_Type {Hight_Speed,Hight_Quality};

typedef struct
{

    int scan_doctype;
    int scan_dpi;
    int colorModel;
    int scan_size;
    int scan_type;
    int contrast;
    int brightness;

}
    UiSettings;

typedef
struct{
    Image_Data_Struct source;
    Image_Data_Struct target;
//    int dpi_x;
//    int dpi_y;
//    int source_pixel_resolution;
//    int source_motor_resolution;
//    int source_pixels_per_line;
//    int source_total_lines;
//    int pixels_per_line;
//    int source_bit_per_pixel;
    long long source_size;
    char* scan_buffer;
    int scan_buffer_size;
}
    Calc_Data;

struct Printer_struct;
typedef void (*callback_scan)(void* para);
typedef struct{
    ImageTransInfo* info;
//    FILE* file;
    callback_scan callback;
    void* callback_para;
    int progress;
    char filename[256];
    UiSettings settings;
    Calc_Data calc_data;
    long long received_bytes;
    Printer_struct* printer;
}
    ScanSettings;

class Scanner;
class DeviceIOManager;
class ScannerApp
{
public:
    ScannerApp(DeviceIOManager* _device_manager);
    ~ScannerApp();

public:
    typedef enum _DevStatus
    {
        STATUS_OK,                                    // 0 - OK, indicates command succeeded; or Passed for N3R test command
        STATUS_ILLEGALCMD,                        // 1 - Error, illegal command; or Fail for N3R test command
        STATUS_USEWITHOUTLOCK,                    // 2 - Error, scanner is in use without lock; or System error for N3R test command
        STATUS_WARMINGUP,                        // 3 - Scanner is warming up
        STATUS_POWEREDOFF,                        // 4 - Device currently powered off
        STATUS_COPYNOTLOCKED,                    // 5 - Error, copy not locked
        STATUS_UNKNOWNPARAMID,                    // 6 - Unable to set parameter, unknown parameter id
        STATUS_DATASIZEMISMATCH,                    // 7 - Unable to set parameter, data size mismatch
        STATUS_PARAMINUSE,                        // 8 - Parameter in use
        STATUS_SCANSTOPPED,                        // 9 - Scan stopped from MFP
        STATUS_OUTOFMEMERY,                        // 0x0A - Out of Memory, Scan stopped
        STATUS_FLASHDNFAIL,                        // 0x0B - Flash Download Fail.
        STATUS_INVALIDFLASHDATA = 0x1B,            // 0x1B - Invalid Flash Data.
        STATUS_INCOMPLETEFLASHDATA = 0x2B,        // 0x2B - Incomplete Flash Data.
        STATUS_FAILDNMOTORTABLE = 0x0C,            // 0x0C - Motor table Download Fail.
        STATUS_INVALIDFLASHADDR = 0x0D,            // 0x0D - Invalid flash address, parameter download fail.
        STATUS_INVALIDPARAMETERFMT = 0x0E,        // 0x0E - Invalid parameter format, fail to download
        STATUS_ERROR = 0x0F                        // 0x0F - Error, Check other status

        ,STATUS_Error_Error = 100
        ,STATUS_Cancel
        ,STATUS_Error_App
        ,STATUS_Error_machine
        ,STATUS_Error_busy
        ,STATUS_Error_lock
    } DEVSTATUS;

    int scan(Printer_struct* printer ,ScanSettings* settings);
    void set_cancel(bool cancel);
private:
    Scanner* scanner;
    DeviceIOManager* device_manager;
    ImageTrans* image_trans;

    pthread_t thread;
    pthread_mutex_t mutex;

//    int init_scan(ScanSettings* settings);
    int start_scan(ScanSettings* settings);
    int save_scan_data(ScanSettings* settings ,char* buffer ,int buf_size);
//    int exit_scan(ScanSettings* settings);

    int trans_process(ScanSettings* settings);
    friend class Scanner;

};

//}
#endif // SCANNERAPP_H
