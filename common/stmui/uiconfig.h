#ifndef UICONFIG_H
#define UICONFIG_H

#include "pro_config.h"

//#define SERVER_PATH SERVICE_TRANS_PATH

#define LOCKER_EXE LOCKER_UI

#define EXE_NAME EXE_UI_NAME

//#define FILTERUIEXE_DIR APP_PATH

#define LOG_FILE_NAME LOG_UI_FILE

#include "statusmonitor.h"
#include <QObject>
#include <QMetaType>

extern bool use_status_thread;
extern bool testmode;

Q_DECLARE_METATYPE(Printer_struct)
Q_DECLARE_METATYPE(QList<Printer_struct>)
Q_DECLARE_METATYPE(PrinterInfo_struct)
Q_DECLARE_METATYPE(QList<PrinterInfo_struct>)

#include "smconfig.h"
class UIConfig : public QObject ,SMConfig{
    Q_OBJECT
public:
    typedef enum{
        ModelSerial_unknown,
        ModelSerial_L = 1,
        ModelSerial_M = 1<<1,
        Model_D = 1 << 2,
        Model_W = 1 << 3
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
        ,Usb_Locked
        ,Usb_Printing
        ,Usb_Scanning
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

    explicit UIConfig(QObject *parent = 0);

    static int initConfig();
    static void exit_app();
    static int getModelSerial(Printer_struct* ps);
    static QString GetStatusTypeString(int type);
    static int GetStatusTypeForUI(int status);
    static QString getErrorMsg(int status,int job,bool isAbcPlusModel);
    static bool isAutoShow(int status);
public:
    enum CmdType{
        CMD_GetDefaultPrinter,
        CMD_GetPrinters,
        CMD_GetStatus,
        CMD_Scan,
        CMD_SetCurrentPrinter,

        CMD_WIFI_refresh_plus,

        LS_CMD_GetCopy,
        LS_CMD_COPY,
        LS_CMD_WIFI_apply,
        LS_CMD_WIFI_get,
        LS_CMD_WIFI_getAplist,
        LS_CMD_PASSWD_set,
        LS_CMD_PASSWD_get,
        LS_CMD_PASSWD_confirm,
        LS_CMD_WIFI_GetWifiStatus,
        LS_CMD_PRN_TonerEnd_Get,
        LS_CMD_PRN_TonerEnd_Set,
        LS_CMD_PRN_PSaveTime_Get,
        LS_CMD_PRN_PSaveTime_Set,
        LS_CMD_PRN_PowerOff_Get,
        LS_CMD_PRN_PowerOff_Set,
        LS_CMD_NET_GetV4,
        LS_CMD_NET_SetV4,
        LS_CMD_NET_GetV6,
        LS_CMD_NET_SetV6,
        LS_CMD_PRN_Set_UserConfig,
        LS_CMD_PRN_Get_UserConfig,
        LS_CMD_WIFI_Set_SoftAp,
        LS_CMD_WIFI_Get_SoftAp,
        LS_CMD_PRN_FusingScReset,
        LS_CMD_PRN_TonerReset,
        LS_CMD_PRN_DrumReset,
        LS_CMD_PRN_Get_UserCenterInfo,
        LS_CMD_PRN_poweroff,
        LS_CMD_PRN_getCounterPrintScan,
    };
    Q_ENUMS(CmdType)
};

#endif // UICONFIG_H
