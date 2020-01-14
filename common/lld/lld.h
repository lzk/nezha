#ifndef LLD_H
#define LLD_H
#include <QString>
#include "pro_config.h"
#include "jkinterface.h"
namespace UIConfig {

typedef enum{
    ModelSerial_unknown,
    ModelSerial_L = 1,
    ModelSerial_M = 1<<1,
    Model_D = 1 << 2,
    Model_W = 1 << 3,
    Model_N = 1 << 4,
}
    ModelSerial;

//public enum EnumStatus: byte
typedef enum
{
    Ready                       = 0x00,
    Printing                    = 0x01,
    PowerSaving                 = 0x02,
    WarmingUp                   = 0x03,
    PrintCanceling              = 0x04,
    Processing                  = 0x07,
    CopyScanning                = 0x60,
    CopyScanNextPage            = 0x61,
    CopyPrinting                = 0x62,
    CopyCanceling               = 0x63,
    IDCardMode                  = 0x64,
    DuplexCopyMode              = 0x65,
    ScanScanning                = 0x6A,
    ScanSending                 = 0x6B,
    ScanCanceling               = 0x6C,
    ScannerBusy                 = 0x6D,
    TonerEnd1                   = 0x7F,//For china maket
    TonerEnd2                   = 0x80,
    TonerNearEnd                = 0x81,
    OPCNearEnd                  = 0x86,
    OPCEnd                      = 0xCC,
    ManualFeedRequired          = 0x85,
    PaperNotReachDuplexEntrySensor = 0xBA,
    DuplexTrayNoFeedJam         = 0xBB,
    InitializeJam               = 0xBC,
    NofeedJam                   = 0xBD,
    JamAtRegistStayOn           = 0xBE,
    JamAtExitNotReach           = 0xBF,
    JamAtExitStayOn             = 0xC0,
    CoverOpen                   = 0xC1,
    NoTonerCartridge            = 0xC5,
    WasteTonerFull              = 0xC6,
    PDLMemoryOver               = 0xC2,
    FWUpdate                    = 0xC7,
    OverHeat                    = 0xC8,
    PolygomotorOnTimeoutError   = 0xCD,
    PolygomotorOffTimeoutError  = 0xCE,
    PolygomotorLockSignalError  = 0xCF,
    BeamSynchronizeError        = 0xD1,
    BiasLeak                    = 0xD2,
    PlateActionError            = 0xD3,
    MainmotorError              = 0xD4,
    MainFanMotorEorror          = 0xD5,
    JoinerThermistorError       = 0xD6,
    JoinerReloadError           = 0xD7,
    HighTemperatureErrorSoft    = 0xD8,
    HighTemperatureErrorHard    = 0xD9,
    JoinerFullHeaterError       = 0xDA,
    Joiner3timesJamError        = 0xDB,
    LowVoltageJoinerReloadError = 0xDC,
    MotorThermistorError        = 0xDD,
    EEPROMCommunicationError    = 0xDE,
    CTL_PRREQ_NSignalNoCome     = 0xDF,
    SCAN_USB_Disconnect         = 0xE1,
    SCAN_NET_Disconnect         = 0xE4,
    ScanMotorError              = 0xE5,
    SCAN_DRV_CALIB_FAIL         = 0xE9,
    NetWirelessDongleCfgFail    = 0xE8,
    ScanDriverCalibrationFail   = 0xE9,
    DMAError                    = 0xEF,
    TouchPanelError             = 0xEA,

    Unknown                     = 0xF0, // status added by SW
    Offline                     = 0xF1, // status added by SW
    PowerOff                    = 0xF2 // status added by SW
    ,Usb_Scanning = ScanScanning
    ,Usb_Printing = Printing
    ,Usb_Locked = 0xff + 1
    ,Show_duplex_automation
    ,Show_ID_Card_automation
    ,Show_Nin1_automation
}EnumStatus;

typedef enum
{
    Status_Ready,
    Status_Sleep,
    Status_Offline,
    Status_Warning,
    Status_Busy,
    Status_Error
}StatusDisplayType;

typedef enum
{
    UnknowJob     = 0,
    PrintJob      = 1,
    NormalCopyJob = 2,
    ScanJob       = 3,
    FaxJob        = 4,
    FaxJob2       = 5,
    ReportJob     = 6,
    Nin1CopyJob   = 7,
    IDCardCopyJob = 8,
    PreIDCardCopyJob = 9
}EnumMachineJob;

extern const char* const g_config_file;
extern const char* const app_version;
extern const char* const log_file;
//usb
extern const char* const lock_scan_file;
extern const char* const lock_scan_info_file;
extern const char* const lock_stm_file;
extern const char* const lock_vopscan_info_file;

extern const int usb_error_printing;
extern const int usb_error_scanning;
extern const int usb_error_usb_locked;
extern const int usb_error_busy;

int getpidvid(const QString& makeAndModel ,int& pid ,int& vid ,int& interface);

//status
extern const char* const status_file;
extern const char* const status_lock_file;

bool isDeviceSupported(Printer_struct* ps);
}
using namespace UIConfig;
#endif // LLD_H
