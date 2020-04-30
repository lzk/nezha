#include "statuspaser.h"
#include "lld.h"

StatusPaser::StatusPaser(QObject *parent) :
    QObject(parent)
{
}

QString StatusPaser::GetStatusTypeString(int type)
{
    QString str = " ";
    switch (type) {
    case Status_Ready:
    case Status_Warning:
        str = tr("ResStr_Ready");
        break;
    case Status_Sleep:
        str = tr("ResStr_Sleep");
        break;
    case Status_Busy:
        str = tr("ResStr_Busy");
        break;
    case Status_Error:
        str = tr("ResStr_Error");
        break;
    case Status_Offline:
    default:
        str = tr("ResStr_Offline");
        break;
    }
    return str;
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

QString StatusPaser::getTrayMsg(int status)
{
    const char* str = NULL;
    switch (status)
    {
//        case Ready: //0x00
//        case Printing: //0x01
//        case PowerSaving: //0x02
//        case WarmingUp: //0x03
//        case PrintCanceling://0x04
//        case Processing: //0x07
//        case CopyScanning: //0x60
//        case CopyScanNextPage://0x61
//        case CopyPrinting:
//        case CopyCanceling:
//        case IDCardMode:
//        case DuplexCopyMode:
//        case ScanScanning:
//        case ScanSending:
//        case ScanCanceling:
//        case ScannerBusy:
        case TonerEnd1: //0x7f
            str = "墨粉空，请更换新的墨粉盒。点击查看>>";
        break;
//        case TonerEnd2:
//        case TonerNearEnd://0x81
//        case OPCNearEnd://0x86
        case OPCEnd://0xcc
        str = "感光鼓寿命已尽，点击查看>>";
    break;
//        case ManualFeedRequired:
        case PaperNotReachDuplexEntrySensor://0xba
        str = "双面入口处卡纸，点击查看>>";
    break;
        case DuplexTrayNoFeedJam://0xbb
        str = "双面单元卡纸，点击查看>>";
    break;
        case InitializeJam://0xbc
        str = "打印机内部卡纸，点击查看>>";
    break;
        case NofeedJam: //0xbd
        str = "缺纸或进纸异常，点击查看>>";
    break;
        case JamAtRegistStayOn://0xbe
        str = "进纸处卡纸，点击查看>>";
    break;
        case JamAtExitNotReach://0xbf
        str = "内部卡纸，点击查看>>";
    break;
        case JamAtExitStayOn://0xc0
        str = "出纸处卡纸，点击查看>>";
    break;
//        case CoverOpen: //0xc1
        case NoTonerCartridge://0xc5
        str = "没有墨粉盒，点击查看>>";
        break;
        case WasteTonerFull://0xc6
        str = "墨粉空，请更换新的墨粉盒。点击查看>>";
        break;
//        case PDLMemoryOver://0xc2
//        case FWUpdate://0xc7
//        case OverHeat://0xc8
        case PolygomotorOnTimeoutError://0xcd
        str = "多面镜马达启动超时，点击查看>>";
    break;
        case PolygomotorOffTimeoutError://0xce
        str = "多面镜马达停止超时，点击查看>>";
    break;
        case PolygomotorLockSignalError://0xcf
        str = "未获取多面镜马达稳定运行信号，点击查看>>";
    break;
        case BeamSynchronizeError://0xd1
        str = "LD同步信号错误，点击查看>>";
    break;
        case BiasLeak://0xd2
        str = "高压电源单元故障，点击查看>>";
    break;
        case PlateActionError://0xd3
        str = "底板动作异常，点击查看>>";
    break;
        case MainmotorError://0xd4
        str = "主马达故障，点击查看>>";
    break;
        case MainFanMotorEorror://0xd5
        str = "风扇故障，点击查看>>";
    break;
        case JoinerThermistorError://0xd6
        str = "定影单元温度传感器故障，点击查看>>";
    break;
        case JoinerReloadError://0xd7
        str = "定影单元升温检测异常，点击查看>>";
    break;
        case HighTemperatureErrorSoft://0xd8
        str = "定影单元高温软件检测异常，点击查看>>";
    break;
        case HighTemperatureErrorHard://0xd9
        str = "定影单元高温硬件检测异常，点击查看>>";
    break;
        case JoinerFullHeaterError://0xda
        str = "定影单元连续点灯故障，点击查看>>";
    break;
        case Joiner3timesJamError://0xdb
        str = "定影单元连续3次卡纸故障，点击查看>>";
    break;
        case LowVoltageJoinerReloadError://0xdc
        str = "低电压状态时启动温度异常，点击查看>>";
    break;
        case MotorThermistorError://0xdd
        str = "机内温度异常，点击查看>>";
    break;
    case EEPROMCommunicationError://0xde
        str = "打印机内部错误，点击查看>>";
    break;
        case CTL_PRREQ_NSignalNoCome://0xdf
        str = "PRREQ信号异常，点击查看>>";
    break;
        case SCAN_USB_Disconnect://0xe1
        case SCAN_NET_Disconnect://0xe4
        str = "打印机内部错误，点击查看>>";
    break;
        case ScanMotorError://0xe5
        str = "扫描单元未找到初始位置，点击查看>>";
    break;
        case SCAN_DRV_CALIB_FAIL://0xe9
        str = "扫描单元马达故障，点击查看>>";
    break;
        case NetWirelessDongleCfgFail://0xe8
        str = "无线模块配置失败，点击查看>>";
    break;
        case DMAError://0xef
        str = "DMA错误，点击查看>>";
    break;
    case TouchPanelError://0xea
        str = "触摸屏故障，点击查看>>";
    break;
    case Offline:
    case PowerOff:
        case Unknown:
        default:
            break;
    }
    if(!str){
        return QString();
    }
    return QString::fromUtf8(str);
}

QString StatusPaser::getErrorMsg(int status, int job, bool isAbcPlusModel)
{
    QString errMsg = "";

    switch (status)
    {
        case Ready: errMsg = tr("ResStr_Ready"); break;
        case Printing: errMsg = tr("ResStr_Busy"); break;
//        case Printing: errMsg = tr("ResStr_Printing"); break;
        case PowerSaving: errMsg = tr("ResStr_Sleep"); break;
        case WarmingUp: errMsg = tr("ResStr_WarmingUp"); break;
        case PrintCanceling: errMsg = tr("ResStr_Print_Cancelling"); break;
        case Processing: errMsg = tr("ResStr_Processing"); break;
        case CopyScanning: errMsg = tr("ResStr_Copying"); break;
        case CopyScanNextPage:
//            if (job == IDCardCopyJob)
                errMsg = tr("ResStr_Turn_card_over_to_copy_the_reverse_");
//            else if (job == Nin1CopyJob)
//                errMsg = tr("ResStr_Place_Next_Page");
//            else if (job == NormalCopyJob)
//                errMsg = tr("ResStr_Place_Next_Page");
//            else
//                errMsg = "";
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
//    case EEPROMCommunicationError:
//        errMsg = tr("ResStr_Turn_off_the_printer__and_turn_it_on_again_Contact_customer_support_if_this_failure_is_repeated_SCxxx") +
//            tr("ResStr_Machine_thermistor_error");
//            break;
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

bool StatusPaser::isAutoShow(int status)
{
    bool ias = false;

    switch (status)
    {
//        case Ready: //0x00
//        case Printing: //0x01
//        case PowerSaving: //0x02
//        case WarmingUp: //0x03
//        case PrintCanceling://0x04
//        case Processing: //0x07
//        case CopyScanning: //0x60
//        case CopyScanNextPage://0x61
//        case CopyPrinting:
//        case CopyCanceling:
//        case IDCardMode:
//        case DuplexCopyMode:
//        case ScanScanning:
//        case ScanSending:
//        case ScanCanceling:
//        case ScannerBusy:
        case TonerEnd1: //0x7f
//        case TonerEnd2:
//        case TonerNearEnd://0x81
//        case OPCNearEnd://0x86
        case OPCEnd://0xcc
//        case ManualFeedRequired:
        case PaperNotReachDuplexEntrySensor://0xba
        case DuplexTrayNoFeedJam://0xbb
        case InitializeJam://0xbc
        case NofeedJam: //0xbd
        case JamAtRegistStayOn://0xbe
        case JamAtExitNotReach://0xbf
        case JamAtExitStayOn://0xc0
//        case CoverOpen: //0xc1
        case NoTonerCartridge://0xc5
        case WasteTonerFull://0xc6
//        case PDLMemoryOver://0xc2
//        case FWUpdate://0xc7
//        case OverHeat://0xc8
        case PolygomotorOnTimeoutError://0xcd
        case PolygomotorOffTimeoutError://0xce
        case PolygomotorLockSignalError://0xcf
        case BeamSynchronizeError://0xd1
        case BiasLeak://0xd2
        case PlateActionError://0xd3
        case MainmotorError://0xd4
        case MainFanMotorEorror://0xd5
        case JoinerThermistorError://0xd6
        case JoinerReloadError://0xd7
        case HighTemperatureErrorSoft://0xd8
        case HighTemperatureErrorHard://0xd9
        case JoinerFullHeaterError://0xda
        case Joiner3timesJamError://0xdb
        case LowVoltageJoinerReloadError://0xdc
        case MotorThermistorError://0xdd
    case EEPROMCommunicationError://0xde
        case CTL_PRREQ_NSignalNoCome://0xdf
        case SCAN_USB_Disconnect://0xe1
        case SCAN_NET_Disconnect://0xe4
        case ScanMotorError://0xe5
        case SCAN_DRV_CALIB_FAIL://0xe9
        case NetWirelessDongleCfgFail://0xe8
        case DMAError://0xef
    case TouchPanelError://0xea
            ias = true;
            break;
    case Offline:
    case PowerOff:
        case Unknown:
        default:
            break;
    }
    return ias;
}

