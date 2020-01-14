#include "lld.h"
#include <QString>

#ifndef Q_OS_DARWIN
extern "C"{
#include <string.h>
/* some systems do not have newest memcpy@@GLIBC_2.14 - stay with old good one */
#ifdef __x86_64__
asm (".symver memcpy, memcpy@GLIBC_2.2.5");
#elif __i386__
asm (".symver memcpy, memcpy@GLIBC_2.0");
#endif

//void *memcpy(void* ,const void* ,size_t);
void *__wrap_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

}
#endif
namespace UIConfig {

//log file var
//const char* log_app_name = EXE_SERVICE_NAME;
const char* const app_version = APP_VERSION;
const char* const g_config_file = CONFIG_FILE;
const char* const log_file = LOG_SERVICE_FILE;

//scan control var
const char* const lock_scan_file = LOCKER_SANE;
const char* const lock_scan_info_file = SANE_INFO_FILE;
const char* const lock_stm_file = "/tmp/.stm_lock";
const char* const lock_vopscan_info_file = "/tmp/.scanner_info";

//usb error control var
const int usb_error_printing = Usb_Printing;
const int usb_error_scanning = Usb_Scanning;
const int usb_error_usb_locked = Usb_Locked;
const int usb_error_busy = -103;

//status
const char* const status_file = STATUS_FILE;
const char* const status_lock_file = STATUS_LOCKER_FILE;

int getModelSerial(Printer_struct* ps)
{
    int ms = ModelSerial_unknown;
    QString makeAndModel(ps->makeAndModel);
    if(0){
#if  !LENOVO_GUOCHANHUA || NEZHA || SUNWUKONG_SFP || SUNWUKONG
    }else if(makeAndModel.startsWith("Lenovo Image LJ2310N")){
        ms = ModelSerial_L + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image LJ2320DN")){
        ms = ModelSerial_L + Model_D + Model_N;
#endif
#if  !LENOVO_GUOCHANHUA || SUNWUKONG_SFP || SUNWUKONG
    }else if(makeAndModel.startsWith("Lenovo Image G262DN")){
        ms = ModelSerial_L + Model_D + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image G336DN")){
        ms = ModelSerial_L + Model_D + Model_N;
#endif
#if  !LENOVO_GUOCHANHUA || SUNWUKONG
    }else if(makeAndModel.startsWith("Lenovo Image GM265DN")){
        ms = ModelSerial_M + Model_D + Model_N;
//        ms = ModelSerial_L + Model_D + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image GM337DN")){
        ms = ModelSerial_M + Model_D + Model_N;
//        ms = ModelSerial_L + Model_D + Model_N;
#endif
#if !LENOVO_GUOCHANHUA
    }else if(makeAndModel.startsWith("Lenovo L100DW")){
        ms = ModelSerial_L + Model_D+ Model_N;// + Model_W;
    }else if(makeAndModel.startsWith("Lenovo L100W")){
        ms = ModelSerial_L;// + Model_W;
    }else if(makeAndModel.startsWith("Lenovo L100D")){
        ms = ModelSerial_L + Model_D;
    }else if(makeAndModel.startsWith("Lenovo L100")){
        ms = ModelSerial_L;
    }else if(makeAndModel.startsWith("Lenovo M100W")){
        ms = ModelSerial_M + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M100D")){
        ms = ModelSerial_M + Model_D;
    }else if(makeAndModel.startsWith("Lenovo M100")){
        ms = ModelSerial_M;
    }else if(makeAndModel.startsWith("Lenovo M101DW")){
        ms = ModelSerial_M + Model_D + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M101W")){
        ms = ModelSerial_M + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M101")){
        ms = ModelSerial_M;
    }else if(makeAndModel.startsWith("Lenovo M102W")){
        ms = ModelSerial_M + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M102")){
        ms = ModelSerial_M;
#endif
    }
//    return ModelSerial_M + Model_D + Model_W;
    return ms;
}

bool isDeviceSupported(Printer_struct* ps)
{
//    LOGLOG("test found device name:%s \n\tmodel:%s" ,ps->name,ps->makeAndModel);
    if(UIConfig::ModelSerial_unknown == getModelSerial(ps))
        return false;
    if(QString(ps->deviceUri).toLower().contains("://[fe80"))
        return false;
    return true;
}

int getpidvid(const QString& makeAndModel ,int& pid ,int& vid ,int& interface)
{
    vid = 0x17ef;
    pid = -1;
    if(0){

#if  !LENOVO_GUOCHANHUA || NEZHA || SUNWUKONG_SFP || SUNWUKONG
    }else if(makeAndModel.contains("LJ2310N")){
        pid = 0x5459;
        interface = 1;
    }else if(makeAndModel.contains("LJ2320DN")){
        pid = 0x5454;
        interface = 1;
#endif
#if  !LENOVO_GUOCHANHUA || SUNWUKONG_SFP || SUNWUKONG
    }else if(makeAndModel.contains("G262DN")){
        pid = 0x544D;
        interface = 1;
    }else if(makeAndModel.contains("G336DN")){
        pid = 0x5466;
        interface = 1;
#endif
#if  !LENOVO_GUOCHANHUA || SUNWUKONG
    }else if(makeAndModel.contains("GM265DN")){
        pid = 0x5663;
        interface = 1;
    }else if(makeAndModel.contains("GM337DN")){
        pid = 0x5671;
        interface = 1;
#endif
#if !LENOVO_GUOCHANHUA
    }else if(makeAndModel.contains("L100DW")){
        pid = 0x5445;
        interface = 0;
    }else if(makeAndModel.contains("L100W")){
        pid = 0x5443;
        interface = 0;
    }else if(makeAndModel.contains("L100D")){
        pid = 0x5444;
        interface = 0;
    }else if(makeAndModel.contains("L100")){
        pid = 0x5442;
        interface = 0;
    }else if(makeAndModel.contains("M100W")){
        pid = 0x5641;
        interface = 1;
    }else if(makeAndModel.contains("M100D")){
        pid = 0x5644;
        interface = 1;
    }else if(makeAndModel.contains("M100")){
        pid = 0x563e;
        interface = 1;
    }else if(makeAndModel.contains("M101DW")){
        pid = 0x5645;
        interface = 1;
    }else if(makeAndModel.contains("M101W")){
        pid = 0x5642;
        interface = 1;
    }else if(makeAndModel.contains("M101")){
        pid = 0x563f;
        interface = 1;
    }else if(makeAndModel.contains("M102W")){
        pid = 0x5643;
        interface = 1;
    }else if(makeAndModel.contains("M102")){
        pid = 0x5640;
        interface = 1;
    }else if(makeAndModel.contains("M7268W")){
        pid = 0x563a;
        interface = 1;
#endif
    }
    return (pid == -1) ?-1 :0;
}

}
