#include "uiconfig.h"
#include "commonapi.h"
#include "filelocker.h"
//#include "appserver.h"
#include <QFile>
#include <QDir>
#include <sys/stat.h>
#include <QDateTime>
#include <stdlib.h>

bool testmode = false;
//const QString app_name = QString::fromUtf8("打印机状态监视器");
FileLocker app_file_locker;
//AppServer* app_server;

extern
int (* getpidvid)(const QString& makeAndModel ,int& pid ,int& vid ,int& interface);
//log file var
extern const char* log_app_name;
extern const char* app_version;
//usb error control var
extern int usb_error_printing;
extern int usb_error_scanning;
extern int usb_error_usb_locked;
//scan control var
extern const char* lock_scan_file;
extern const char* lock_scan_info_file;

static bool _isDeviceSupported(Printer_struct* ps)
{
//    LOGLOG("test found device name:%s \n\tmodel:%s" ,ps->name,ps->makeAndModel);
    if(UIConfig::ModelSerial_unknown == UIConfig::getModelSerial(ps))
        return false;
    if(QString(ps->deviceUri).toLower().contains("://[fe80"))
        return false;
    return true;
}

static int _getpidvid(const QString& makeAndModel ,int& pid ,int& vid ,int& interface)
{
    vid = 0x17ef;
    pid = -1;
    if(0){
#ifdef DEBUG_DEBUG
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
    }else if(makeAndModel.contains("M7268")){
        pid = 0x5639;
        interface = 1;
//    }else if(makeAndModel.contains("G262DNT")){
//        pid = 0x5461;
//        interface = 1;
//    }else if(makeAndModel.contains("G336DNT")){
//        pid = 0x5462;
//        interface = 1;
#endif
    }else if(makeAndModel.contains("LJ2310N")){
        pid = 0x5459;
        interface = 1;
    }else if(makeAndModel.contains("LJ2320DN")){
        pid = 0x5454;
        interface = 1;
    }else if(makeAndModel.contains("G262DN")){
        pid = 0x544D;
        interface = 1;
    }else if(makeAndModel.contains("G336DN")){
        pid = 0x5466;
        interface = 1;
//    }else if(makeAndModel.contains("GM265DN")){
//        pid = 0x5663;
//        interface = 1;
//    }else if(makeAndModel.contains("GM337DN")){
//        pid = 0x5671;
//        interface = 1;
    }
    return (pid == -1) ?-1 :0;
}

QString UIConfig::tmp_scan_dir;
UIConfig::UIConfig(QObject *parent) :
    QObject(parent)
{
}

extern const char* log_file;
int UIConfig::initConfig()
{
    log_app_name = EXE_NAME;
    app_version = APP_VERSION;
    log_file = LOG_FILE_NAME;
    log_init();

    if(app_file_locker.trylock(LOCKER_EXE)){
        LOGLOG("app had been locked!");
        return -1;
    }
//    if(is_app_running(DOMAIN_UIEXE)){
//        LOGLOG("socket working!");
//        return -2;
//    }

    LOGLOG("--------%s v%s-------" ,log_app_name ,app_version);

    g_config_file = CONFIG_FILE;

    //config status server thread
    if(testmode){
        status_file = TEST_STATUS_FILE;
        status_lock_file = TEST_STATUS_LOCKER_FILE;
    }else{
#ifdef DEBUG_DEBUG
        status_file = TEST_STATUS_FILE;
        status_lock_file = TEST_STATUS_LOCKER_FILE;
#else
        status_file = STATUS_FILE;
        status_lock_file = STATUS_LOCKER_FILE;
#endif
    }

    //usb special config
    lock_scan_file = LOCKER_SANE;
    lock_scan_info_file = SANE_INFO_FILE;
    usb_error_printing = Usb_Printing;
    usb_error_scanning = Usb_Scanning;
    usb_error_usb_locked = Usb_Locked;

    //config supported printer model
    isDeviceSupported = _isDeviceSupported;
    getpidvid = _getpidvid;

    QDateTime time = QDateTime::currentDateTime();
    QString str_time = time.toString("yyyy-MM-dd_hh-mm-ss-zzz");
    tmp_scan_dir = QString("/tmp/tmpscan_") + str_time;
    QDir().mkdir(tmp_scan_dir);
//    QDir dir(TMP_SCAN_DIR);
//    QDir *path = &dir;
//    if(path->exists(TMP_SCAN_DIR)){
//        path->remove(TMP_SCAN_DIR);
//    }
//    path->mkdir(TMP_SCAN_DIR);

//    app_server = new AppServer(DOMAIN_UIEXE);
    return 0;
}

void UIConfig::exit_app()
{
//    QFile::remove(filepath);
//    QFile::remove(lockfile);
    app_file_locker.unlock();
 //   delete app_server;
//    QDir dir(TMP_SCAN_DIR);
//    QDir *path = &dir;
//    if(path->exists(TMP_SCAN_DIR)){
//        path->remove(TMP_SCAN_DIR);
//    }
    QString cmd("rm -rf ");
    cmd += tmp_scan_dir;
    system(cmd.toLatin1().constData());
//    QDir().remove(tmp_scan_dir);
}

int UIConfig::getModelSerial(Printer_struct* ps)
{
    int ms = ModelSerial_unknown;
    QString makeAndModel(ps->makeAndModel);
    if(0){
#ifdef DEBUG_DEBUG
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
//        ms = ModelSerial_M + Model_D + Model_W;
        ms = ModelSerial_L + Model_D + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M101W")){
        ms = ModelSerial_M + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M101")){
        ms = ModelSerial_M;
    }else if(makeAndModel.startsWith("Lenovo M102W")){
        ms = ModelSerial_M + Model_W;
    }else if(makeAndModel.startsWith("Lenovo M102")){
        ms = ModelSerial_M;
#endif
    }else if(makeAndModel.startsWith("Lenovo Image LJ2310N")){
        ms = ModelSerial_L + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image LJ2320DN")){
        ms = ModelSerial_L + Model_D + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image G262DN")){
        ms = ModelSerial_L + Model_N;
    }else if(makeAndModel.startsWith("Lenovo Image G336DN")){
        ms = ModelSerial_L + Model_N;
//    }else if(makeAndModel.startsWith("Lenovo Image GM265DN")){
//        ms = ModelSerial_M + Model_D + Model_N;
//    }else if(makeAndModel.startsWith("Lenovo Image GM337DN")){
//        ms = ModelSerial_M + Model_D + Model_N;
    }
//    return ModelSerial_M + Model_D + Model_W;
    return ms;
}

int UIConfig::GetStatusTypeForUI(int status)
{
    int st = Status_Offline;

    switch ( status )
    {
                    case Ready                       : st = Status_Ready; break;
                    case Printing                    : st = Status_Busy ; break;
                    case PowerSaving                 : st = Status_Sleep; break;
                    case WarmingUp                   : st = Status_Busy ; break;
                    case PrintCanceling              : st = Status_Busy ; break;
                    case Processing                  : st = Status_Busy ; break;
                    case CopyScanning                : st = Status_Busy ; break;
                    case CopyScanNextPage            : st = Status_Busy ; break;
                    case CopyPrinting                : st = Status_Busy ; break;
                    case CopyCanceling               : st = Status_Busy ; break;
                    case IDCardMode                  : st = Status_Ready ; break;
                    case DuplexCopyMode              : st = Status_Ready ; break;
                    case ScanScanning                : st = Status_Busy ; break;
                    case ScanSending                 : st = Status_Busy ; break;
                    case ScanCanceling               : st = Status_Busy ; break;
                    case ScannerBusy                 : st = Status_Busy ; break;
                    case TonerEnd1                   : st = Status_Ready; break;
                    case TonerEnd2                   : st = Status_Ready; break;
                    case TonerNearEnd                : st = Status_Ready; break;
                    case OPCNearEnd                  : st = Status_Ready; break;
                    case OPCEnd                      : st = Status_Error; break;
                    case ManualFeedRequired          : st = Status_Busy ; break;
                    case PaperNotReachDuplexEntrySensor : st = Status_Error; break;
                    case DuplexTrayNoFeedJam         : st = Status_Error; break;
                    case InitializeJam               : st = Status_Error; break;
                    case NofeedJam                   : st = Status_Error; break;
                    case JamAtRegistStayOn           : st = Status_Error; break;
                    case JamAtExitNotReach           : st = Status_Error; break;
                    case JamAtExitStayOn             : st = Status_Error; break;
                    case CoverOpen                   : st = Status_Error; break;
                    case NoTonerCartridge            : st = Status_Error; break;
                    case WasteTonerFull              : st = Status_Ready; break;
                    case PDLMemoryOver               : st = Status_Error; break;
                    case FWUpdate                    : st = Status_Busy ; break;
                    case OverHeat                    : st = Status_Busy ; break;
                    case PolygomotorOnTimeoutError   : st = Status_Error; break;
                    case PolygomotorOffTimeoutError  : st = Status_Error; break;
                    case PolygomotorLockSignalError  : st = Status_Error; break;
                    case BeamSynchronizeError        : st = Status_Error; break;
                    case BiasLeak                    : st = Status_Error; break;
                    case PlateActionError            : st = Status_Error; break;
                    case MainmotorError              : st = Status_Error; break;
                    case MainFanMotorEorror          : st = Status_Error; break;
                    case JoinerThermistorError       : st = Status_Error; break;
                    case JoinerReloadError           : st = Status_Error; break;
                    case HighTemperatureErrorSoft    : st = Status_Error; break;
                    case HighTemperatureErrorHard    : st = Status_Error; break;
                    case JoinerFullHeaterError       : st = Status_Error; break;
                    case Joiner3timesJamError        : st = Status_Error; break;
                    case LowVoltageJoinerReloadError : st = Status_Error; break;
                    case MotorThermistorError        : st = Status_Error; break;
                    case EEPROMCommunicationError    : st = Status_Error; break;
                    case CTL_PRREQ_NSignalNoCome     : st = Status_Error; break;
                    case SCAN_USB_Disconnect         : st = Status_Error; break;
                    case SCAN_NET_Disconnect         : st = Status_Error; break;
                    case ScanMotorError              : st = Status_Error; break;
                    case SCAN_DRV_CALIB_FAIL         : st = Status_Error; break;
                    case NetWirelessDongleCfgFail    : st = Status_Error; break;
                    case DMAError                    : st = Status_Error; break;

                    case Offline                     :
                    case PowerOff                    :
                    case Unknown                     : st = Status_Offline; break;

                    default                                     :
                                                                  st = Status_Offline;
                                                                  break;
    }
    return st;
}

QString UIConfig::getErrorMsg(EnumStatus status, EnumMachineJob job, bool isAbcPlusModel)
{
    QString errMsg = "";
//    LOGLOG("EnumMachineJob:%d",job);

    switch (status)
    {
        case Ready: errMsg = ""; break;
        case Printing: errMsg = tr("ResStr_Printing"); break;
        case PowerSaving: errMsg = ""; break;
        case WarmingUp: errMsg = tr("ResStr_WarmingUp"); break;
        case PrintCanceling: errMsg = tr("ResStr_Print_Cancelling"); break;
        case Processing: errMsg = tr("ResStr_Processing"); break;
        case CopyScanning: errMsg = tr("ResStr_Copying"); break;
        case CopyScanNextPage:
            if (job == IDCardCopyJob)
                errMsg = tr("ResStr_Turn_card_over_to_copy_the_reverse_");
            else if (job == Nin1CopyJob)
                errMsg = tr("ResStr_Place_Next_Page");
            else if (job == NormalCopyJob)
                errMsg = tr("ResStr_Place_Next_Page");
            else
                errMsg = "";
            break;
        case CopyPrinting: errMsg = tr("ResStr_Copying"); break;
        case CopyCanceling: errMsg = tr("ResStr_Copy_Cancelling"); break;
        case IDCardMode: errMsg = tr("ResStr_ID_Card_Mode"); break;
        case DuplexCopyMode: errMsg = tr("ResStr_Duplex_Copy_Mode"); break;
        case ScanScanning: errMsg = tr("ResStr_Scanning"); break;
        case ScanSending: errMsg = tr("ResStr_Scanning"); break;
        case ScanCanceling: errMsg = tr("ResStr_Scan_Cancelling"); break;
        case ScannerBusy: errMsg = tr("ResStr_Scanner_Busy"); break;
        case TonerEnd1: errMsg = tr("ResStr_Toner_End_ABC_Plus"); break;
//            {
//                if (isAbcPlusModel)
//                {
//                    errMsg = tr("ResStr_Toner_End_ABC_Plus"); break;
//                }
//                else
//                {
//                    errMsg = tr("ResStr_Toner_End_ABC"); break;
//                }
//            }
        case TonerEnd2: errMsg = tr("ResStr_Toner_End_ABC_Plus"); break;
//            {
//                if (isAbcPlusModel)
//                {
//                    errMsg = tr("ResStr_Toner_End_ABC_Plus"); break;
//                }
//                else
//                {
//                    errMsg = tr("ResStr_Toner_End_ABC"); break;
//                }
//            }
        case TonerNearEnd: errMsg = tr("ResStr_Toner_Near_End"); break;
        case OPCNearEnd: errMsg = tr("ResStr_OPC_Near_End"); break;
        case OPCEnd:errMsg = tr("ResStr_OPC_End_ABC_Plus"); break;
//            {
//                if (isAbcPlusModel)
//                {
//                    errMsg = tr("ResStr_OPC_End_ABC_Plus"); break;
//                }
//                else
//                {
//                    errMsg = tr("ResStr_OPC_End_ABC"); break;
//                }
//            }
        case ManualFeedRequired: errMsg = tr("ResStr_Waiting_2nd_pages_when_print_manual_duplex_job"); break;
        case PaperNotReachDuplexEntrySensor: errMsg = tr("ResStr_Paper_Jam_Duplex_Entry_Sensor"); break;
        case DuplexTrayNoFeedJam: errMsg = tr("ResStr_Paper_Jam_Duplex_Nofeed"); break;
        case InitializeJam: errMsg = tr("ResStr_Paper_Jam__Paper_Remained"); break;
        case NofeedJam: errMsg = tr("ResStr_Paper_Jam__Nofeed"); break;
        case JamAtRegistStayOn: errMsg = tr("ResStr_Paper_Jam__Regist"); break;
        case JamAtExitNotReach: errMsg = tr("ResStr_Paper_Jam__Exit_NotReach"); break;
        case JamAtExitStayOn: errMsg = tr("ResStr_Paper_Jam__Exit"); break;
        case CoverOpen: errMsg = tr("ResStr_Cover_Open"); break;
//        case NoTonerCartridge: errMsg = tr("ResStr_No_Toner_Cartridge"); break;
//        case WasteTonerFull: errMsg = tr("ResStr_Please_Replace_Toner"); break;
    case NoTonerCartridge: errMsg = QString::fromUtf8("没有墨粉盒"); break;
    case WasteTonerFull: errMsg = tr("ResStr_Toner_End_ABC_Plus"); break;
        case PDLMemoryOver: errMsg = tr("ResStr_PDL_Memory_Overflow"); break;
        case FWUpdate: errMsg = tr("ResStr_FW_Updating"); break;
        case OverHeat: errMsg = tr("ResStr_Overheat"); break;
        case PolygomotorOnTimeoutError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Polygon_Motor_on_timeout_error");
                 break;
        case PolygomotorOffTimeoutError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Polygon_Motor_off_timeout_error");
                break;
        case PolygomotorLockSignalError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Polygon_Motor_lock_signal_error");
                break;
        case BeamSynchronizeError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Beam_synchronize_error");
                break;
        case BiasLeak:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Bias_leak");
                break;
        case PlateActionError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_plate_action_error");
                break;
        case MainmotorError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
               tr("ResStr_Main_Motor_error");
                break;
        case MainFanMotorEorror:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Fan_Motor_error");
                break;
        case JoinerThermistorError:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
                tr("ResStr_Fuser_thermistor_error");
                break;
        case JoinerReloadError:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
                tr("ResStr_Fuser_reload_error");
                break;
        case HighTemperatureErrorSoft:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
               tr("ResStr_High_temperature_soft_check_error");
                break;
        case HighTemperatureErrorHard:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
                tr("ResStr_High_temperature_hard_check_error");
                break;
        case JoinerFullHeaterError:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
                tr("ResStr_Fuser_full_heater_error");
                break;
        case Joiner3timesJamError:
            errMsg = tr("ResStr_Please_contact_customer_support__SCxxx") +
                tr("ResStr_Fuser_3times_jam_error");
                break;
        case LowVoltageJoinerReloadError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Low_voltage_fuser_reload_error");
                break;
        case MotorThermistorError:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Machine_thermistor_error");
                break;
        case CTL_PRREQ_NSignalNoCome:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Print_request_signal_doesnt_come");
                break;

        case SCAN_USB_Disconnect:        errMsg = tr("ResStr_USB_write_failed_during_scan_job_cancelling"); break;
        case SCAN_NET_Disconnect:        errMsg = tr("ResStr_NET_write_failed_during_scan_job_cancelling"); break;

        case ScanMotorError:
             errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Scanner_not_found_home_position");
                break;
        case SCAN_DRV_CALIB_FAIL:
            errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Scan_Calibration_Error");
                break;
        case NetWirelessDongleCfgFail:
             errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
                tr("ResStr_Wireless_Dongle_Config_Fail");
                break;
        case DMAError:
            errMsg = tr("ResStr_DMA_Error_SCxxx") +
                tr("ResStr_DMA_Error");
                break;
        case Offline:
        case PowerOff:
        case Unknown: errMsg = ""; break;
        default:
            errMsg = "";
            break;
    }
    return errMsg;
}
