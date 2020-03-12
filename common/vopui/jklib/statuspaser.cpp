#include "statuspaser.h"
#include "lld.h"
StatusPaser::StatusPaser(QObject *parent) :
    QObject(parent)
{
}

bool StatusPaser::is_enable_scan(int status)
{
    bool enable = false;
    switch (status) {
    case UIConfig::WarmingUp:
        enable = true;
        break;
    default:
    {
        int status_type = StatusPaser::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Ready:
        case UIConfig::Status_Sleep:
            enable = true;
            break;
        default:
            break;
        }
    }
        break;
    }
    return enable;
}

bool StatusPaser::is_enable_copy(int status)
{
    bool enable = false;
    switch (status) {
    case UIConfig::WarmingUp:
        enable = true;
        break;
    default:
    {
        int status_type = StatusPaser::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Ready:
        case UIConfig::Status_Sleep:
            enable = true;
            break;
        default:
            break;
        }
    }
    }
    return enable;
}

bool StatusPaser::is_enable_setting(int status)
{
    bool enable = true;
    switch (status) {
    default:
    {
        int status_type = StatusPaser::GetStatusTypeForUI(status);
        switch (status_type) {
        case UIConfig::Status_Offline:
            enable = false;
            break;
        default:
            break;
        }
    }
    }
    return enable;
}

int StatusPaser::GetStatusTypeForUI(int status)
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
                    case TonerEnd1                   : st = Status_Warning; break;
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
                    case WasteTonerFull              : st = Status_Warning; break;
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
                    case TouchPanelError             : st = Status_Warning; break;

                    case Offline                     :
                    case PowerOff                    :
                    case Unknown                     : st = Status_Offline; break;

                    default                                     :
                                                                  st = Status_Offline;
                                                                  break;
    }
    return st;
}

QString StatusPaser::getErrorMsg(int status, int job, bool isAbcPlusModel)
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
    case TouchPanelError:
        errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
            tr("ResStr_Touch_Panel_Error");
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

