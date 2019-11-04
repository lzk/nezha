/////////////////////////////////////////
/// Author:Jacky Liang
/// Version:
/////////////////////////////////////////
#include <string.h>
#include "lshell.h"
#include "log.h"

#pragma pack(1)
typedef struct _COMM_HEADER
{
    UINT32 magic;
    UINT16 id;
    UINT16 len;     // lenght of data structure and its data

    // data structure
    UINT8 subid;
    UINT8 len2;

    // data append
    UINT8 subcmd;
}COMM_HEADER;
#pragma pack()

typedef enum _CMD_ID
{
    _LS_SEARCH		= 0x0000, 	// Search Machine
    _LS_CONNECT		= 0x0001,	// Connect Machine
    _LS_DISCONNECT	= 0x0002,	// Disconnect Machine
    _LS_CALLFUNC	= 0x0100,	// Call Function
    _LS_SHOWVAR		= 0x0101, 	// Show Variable/Structure
    _LS_MODIFYVAR	= 0x0102,	// Modify Variable/Structure
    _LS_PRIVEXEC	= 0x0103,	// Private Execution
    _LS_ENGCMD		= 0x0104,	// Engine Command
    _LS_NETCMD		= 0x0105,	// Network Command
    _LS_WIFICMD		= 0x0106,	// Wireless Command
    _LS_PRNCMD		= 0x0107,	// Print Command
    _LS_SCACMD		= 0x0108,	// Scan Command
    _LS_CPYCMD		= 0x0109,	// Copy Command
    _LS_FAXCMD		= 0x010A,	// Fax Command
    _LS_DBGMSG		= 0x0200, 	// Debug Message
    _LS_HEARTBEAT 	= 0x0201,	// Heart Beat, Null Packet, keep activated
    _LS_PANKEY		= 0x0300,	// Panel Key Simulation
    _LS_PANIMG		= 0x0301,	// Panel Frame & LED status
    _LS_DATADOWN	= 0x0400,	// Download Data
    _LS_DATAUPLD	= 0x0401	// Upload Data
}CMD_ID;

static copycmdset default_copy_parameter =
{
//    .Density   = 3,
//    .copyNum   = 1,
//    .scale     = 100
    3,//UINT8 Density         ; // 0  -   0~6
    1,//UINT8 copyNum         ; // 1  -   1~99
    0,//UINT8 scanMode        ; // 2  -   0: Photo, 1: Text, 2: ID card
    0,//UINT8 orgSize         ; // 3  -   0: A4, 1: A5, 2: B5, 3: Letter, 4: Executive
    1,//UINT8 paperSize       ; // 4  -   0: Letter, 1: A4, 2: A5, 3: A6, 4: B5, 5: B6, 6: Executive, 7: 16K
    0,//UINT8 nUp             ; // 5  -   0:1up, 1: 2up, 3: 4up, 4: 9up
    0,//UINT8 dpi             ; // 6  -   0: 300*300, 1: 600*600
    0,//UINT8 mediaType       ; // 7  -   0: plain paper 1: Recycled paper 2: Thick paper 3: Thin paper 4: Label
    100,//UINT16 scale          ; // 8  -   25~400, disabled for 2/4/9up
	0,//UINT8 duplexCopy; // 9  -   0: off 1: on – Long Edge 2: on – Short Edge
	0,//UINT8 IDCardMode; // 10  -   0: A4, Center 1: A4, Top 2: A4, Buttom 3: A5, Center
};

static int lshell_getCmdDirect(int cmd ,int sub_cmd ,int& direct ,int& data_buffer_size)
{
    int ret = 0;
    switch(cmd){
    case _LS_CPYCMD:        direct = sub_cmd;data_buffer_size = 128;        break;
    case _LS_WIFICMD:
        switch(sub_cmd){
        case 0:             direct = 0;data_buffer_size = 180;        break;//get
        case 1:             direct = 1;data_buffer_size = 180;        break;//set
        case 0x07:       direct = 0;data_buffer_size = 340;        break;//get aplist
        case 0x08:       direct = 0;data_buffer_size = 1;            break;//get wifi status
        case 0x10:             direct = 0;data_buffer_size = 180;        break;//get
        case 0x11:             direct = 1;data_buffer_size = 180;        break;//set
        default:ret=-1;break;
        }
        break;
    case _LS_PRNCMD:
        switch(sub_cmd){
        case 0x00:  direct = 0;data_buffer_size = 1;  break;//get psave time
        case 0x01:  direct = 1;data_buffer_size = 1;  break;//set psave time
        case 0x02:  direct = 0;data_buffer_size = 16;  break;//get user config
        case 0x03:  direct = 1;data_buffer_size = 16;  break;//set user config
        case 0x06:  direct = 1;data_buffer_size = 32;  break;//set passwd
        case 0x07:  direct = 0;data_buffer_size = 32; break;//get passwd
        case 0x08:  direct = 1;data_buffer_size = 32; break;//comfirm
        case 0x0e:  direct = 0;data_buffer_size = 1; break;//get power off time
        case 0x0f:  direct = 1;data_buffer_size = 1; break;//set power off time
        case 0x11:  direct = 0;data_buffer_size = 1; break;//get toner end
        case 0x12:  direct = 1;data_buffer_size = 1; break;//set toner end
        case 0x0b:  direct = 1;data_buffer_size = 1; break;//set fusing sc reset
        case 0x04:  direct = 0;data_buffer_size = 480; break;//get printer info
        case 0x05:  direct = 0;data_buffer_size = 480; break;//get printer info ext
        case 0x10:  direct = 0;data_buffer_size = 64; break;//get user center info
        case 0x19:  direct = 1;data_buffer_size = 1; break;//set toner reset
        case 0x1a:  direct = 1;data_buffer_size = 1; break;//set drum reset
        case 0x30:  direct = 1;data_buffer_size = 1; break;//power off
        case 0x42:  direct = 0;data_buffer_size = 28; break;//get counter print scan
        case 0x0c:  direct = 1;data_buffer_size = 1; break;//wifi set enable
        case 0x0d:  direct = 1;data_buffer_size = 0; break;//reboot
        case 0x45:  direct = 0;data_buffer_size = 40; break;//get consumable supply info
        default:ret=-1;break;
        }
        break;
    case _LS_NETCMD:
        switch(sub_cmd){
        case 0x00:  direct = 0;data_buffer_size = 128;  break;//get ipv4
        case 0x01:  direct = 1;data_buffer_size = 128;  break;//set ipv4
        case 0x02:  direct = 0;data_buffer_size = 360;  break;//get ipv6
        case 0x03:  direct = 1;data_buffer_size = 360;  break;//set ipv6
        default:ret=-1;break;
        }
        break;

    case _LS_SEARCH:
    case _LS_CONNECT	:
    case _LS_DISCONNECT:
    case _LS_CALLFUNC:
    case _LS_SHOWVAR:
    case _LS_MODIFYVAR:
    case _LS_PRIVEXEC:
    case _LS_ENGCMD:
    case _LS_SCACMD:
    case _LS_FAXCMD:
    case _LS_DBGMSG:
    case _LS_HEARTBEAT:
    case _LS_PANKEY:
    case _LS_PANIMG:
    case _LS_DATADOWN:
    case _LS_DATAUPLD:
    default:
        ret = -1;
        break;
    }
    return ret;
}

#define     MAGIC_NUM           0x1A2B3C4D
#define change_32bit_edian(x) (((x) << 24 & 0xff000000) | (((x) << 8) & 0x00ff0000) | (((x) >> 8) & 0x0000ff00) | (((x) >> 24) & 0xff))
#define change_16bit_edian(x) (((x) << 8) & 0xff00 | ((x) >> 8) & 0x00ff)
static const unsigned char INIT_VALUE = 0xfe;

int LShell::lshell_cmd(int cmd ,int sub_cmd, void* data ,int data_size)
{
    if(!device && !(device))
        return ERR_no_device;

    int direct=0 ,data_buffer_size=0;
    int real_sub_cmd = sub_cmd;
    if(cmd == _LS_WIFICMD  && sub_cmd == 0xff)
        real_sub_cmd = 0x01;
    int err = lshell_getCmdDirect(cmd ,real_sub_cmd ,direct ,data_buffer_size);
    if(err)
        return ERR_lshell_cannot_support;//not support

    int device_cmd_len = sizeof(COMM_HEADER)+data_buffer_size;
    char* buffer = new char[device_cmd_len];
    memset( buffer, INIT_VALUE, sizeof(device_cmd_len));
    COMM_HEADER* ppkg = reinterpret_cast<COMM_HEADER*>( buffer );

    ppkg->magic = MAGIC_NUM ;
    ppkg->id = cmd;
    ppkg->len = 3+data_buffer_size * direct;

    // For the simple data setting, e.g. copy/scan/prn/wifi/net, SubID is always 0x13, len is always 0x01,
    // it just stand for the sub id. The real data length is defined by the lib
    ppkg->subid = 0x13;
    ppkg->len2 = 1;
    ppkg->subcmd = real_sub_cmd;
    if(data){
        memcpy(buffer + sizeof(COMM_HEADER) ,data ,data_size);
    }

    err = writeThenRead(buffer ,sizeof(COMM_HEADER)+data_buffer_size * direct
                                           ,buffer ,sizeof(COMM_HEADER)+data_buffer_size * (1 - direct));
    //check result
    if(!err && MAGIC_NUM == ppkg->magic){//ACK
        if(!direct){//get
            if(data){
                memcpy(data ,buffer + sizeof(COMM_HEADER) ,data_size);
            }
        }else{//set
            err = ppkg->subcmd;
        }
    }else
        err = -1;
    delete [] buffer;
    return err;
}

#define lshell_getCopy(buffer ,bufsize)                     lshell_cmd(_LS_CPYCMD ,0 ,buffer ,bufsize)
#define lshell_copy(buffer ,bufsize)                        lshell_cmd(_LS_CPYCMD ,1 ,buffer ,bufsize)
#define lshell_getWifiInfo(buffer ,bufsize)                 lshell_cmd(_LS_WIFICMD ,0 ,buffer ,bufsize)
#define lshell_setWifiInfo(buffer ,bufsize)                 lshell_cmd(_LS_WIFICMD ,1 ,buffer ,bufsize)
#define lshell_getApList(buffer ,bufsize)                   lshell_cmd(_LS_WIFICMD ,0x07 ,buffer ,bufsize)
#define lshell_getWifiStatus(buffer ,bufsize)               lshell_cmd(_LS_WIFICMD ,0x08 ,buffer ,bufsize)
#define lshell_setPasswd(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x06 ,buffer ,bufsize)
#define lshell_getPasswd(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x07 ,buffer ,bufsize)
#define lshell_confirmPasswd(buffer ,bufsize)               lshell_cmd(_LS_PRNCMD ,0x08 ,buffer ,bufsize)
#define lshell_getTonerEnd(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x11 ,buffer ,bufsize)
#define lshell_setTonerEnd(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x12 ,buffer ,bufsize)
#define lshell_getPsaveTime(buffer ,bufsize)                lshell_cmd(_LS_PRNCMD ,0x00 ,buffer ,bufsize)
#define lshell_setPsaveTime(buffer ,bufsize)                lshell_cmd(_LS_PRNCMD ,0x01 ,buffer ,bufsize)
#define lshell_getPowerOff(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x0e ,buffer ,bufsize)
#define lshell_setPowerOff(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x0f ,buffer ,bufsize)
#define lshell_getv4(buffer ,bufsize)                       lshell_cmd(_LS_NETCMD ,0x00 ,buffer ,bufsize)
#define lshell_setv4(buffer ,bufsize)                       lshell_cmd(_LS_NETCMD ,0x01 ,buffer ,bufsize)
#define lshell_getv6(buffer ,bufsize)                       lshell_cmd(_LS_NETCMD ,0x02 ,buffer ,bufsize)
#define lshell_setv6(buffer ,bufsize)                       lshell_cmd(_LS_NETCMD ,0x03 ,buffer ,bufsize)
#define lshell_set_userConfig(buffer ,bufsize)              lshell_cmd(_LS_PRNCMD ,0x03 ,buffer ,bufsize)
#define lshell_get_userConfig(buffer ,bufsize)              lshell_cmd(_LS_PRNCMD ,0x02 ,buffer ,bufsize)
#define lshell_set_softAp(buffer ,bufsize)                  lshell_cmd(_LS_WIFICMD ,0x11 ,buffer ,bufsize)
#define lshell_get_softAp(buffer ,bufsize)                  lshell_cmd(_LS_WIFICMD ,0x10 ,buffer ,bufsize)
#define lshell_fusingScReset(buffer ,bufsize)               lshell_cmd(_LS_PRNCMD ,0x0b ,buffer ,bufsize)

#define lshell_tonerReset(buffer ,bufsize)                  lshell_cmd(_LS_PRNCMD ,0x19 ,buffer ,bufsize)
#define lshell_drumScReset(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x1a ,buffer ,bufsize)
#define lshell_getUserInfo(buffer ,bufsize)                 lshell_cmd(_LS_PRNCMD ,0x10 ,buffer ,bufsize)
#define lshell_getFWInfo(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x04 ,buffer ,bufsize)

#define lshell_poweroff(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x30 ,buffer ,bufsize)
#define lshell_getCounterPrintScan(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x42 ,buffer ,bufsize)
#define lshell_wifi_set_enable(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x0c ,buffer ,bufsize)
#define lshell_prn_reboot(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x0d ,buffer ,bufsize)
#define lshell_getConsumableSupplyInfo(buffer ,bufsize)                   lshell_cmd(_LS_PRNCMD ,0x45 ,buffer ,bufsize)

int LShell::copy(copycmdset* para)
{
//    LOGLOG("lshell cmd: copy set");
    int err;
    err = lshell_copy(para ,sizeof(*para));
    LOGLOG("LShell:%d",err);
    return err;
}

int LShell::copy_get_para(copycmdset* para)
{
//    LOGLOG("lshell cmd: copy get");
    int err;
    err = lshell_getCopy(para ,sizeof(*para));
    return err;
}

int LShell::wifi_apply(cmdst_wifi_get* para)
{
//    LOGLOG("lshell cmd: wifi apply");
    int err;
    err = lshell_setWifiInfo(para ,sizeof(*para));
    return err;
}

int LShell::wifi_get_para(cmdst_wifi_get* para)
{
//    LOGLOG("lshell cmd: wifi get");
    int err;
    err = lshell_getWifiInfo(para ,sizeof(*para));
    return err;
}

int LShell::wifi_get_aplist(cmdst_aplist_get* para)
{
//    LOGLOG("lshell cmd: wifi get aplist");
    int err;
    err = lshell_getApList(para ,sizeof(*para));
    return err;
}

int LShell::wifi_get_status(cmdst_wifi_status* para)
{
//    LOGLOG("lshell cmd: wifi get status");
    int err;
    err = lshell_getWifiStatus(para ,sizeof(*para));
    return err;
}

int LShell::wifi_softap_set(cmdst_softap* para)
{
//    LOGLOG("lshell cmd: wifi set soft ap");
    int err;
    err = lshell_set_softAp(para ,sizeof(*para));
    return err;
}

int LShell::wifi_softap_get(cmdst_softap* para)
{
//    LOGLOG("lshell cmd: wifi get softap");
    int err;
    err = lshell_get_softAp(para ,sizeof(*para));
    return err;
}

int LShell::password_set(cmdst_passwd* para)
{
//    LOGLOG("lshell cmd: password set");
    int err;
    err = lshell_setPasswd(para ,sizeof(*para));
    return err;
}

int LShell::password_get(cmdst_passwd* para)
{
//    LOGLOG("lshell cmd: password get");
    int err;
    err = lshell_getPasswd(para ,sizeof(*para));
    return err;
}

int LShell::password_confirm(cmdst_passwd* para)
{
//    LOGLOG("lshell cmd: password confirm");
    int err;
    err = lshell_confirmPasswd(para ,sizeof(*para));
    return err;
}

int LShell::tonerend_set(cmdst_tonerEnd* para)
{
//    LOGLOG("lshell cmd: toner end set");
    int err;
    err = lshell_setTonerEnd(para ,sizeof(*para));
    return err;
}

int LShell::tonerend_get(cmdst_tonerEnd* para)
{
//    LOGLOG("lshell cmd: toner end get");
    int err;
    err = lshell_getTonerEnd(para ,sizeof(*para));
    return err;
}

int LShell::savetime_set(cmdst_PSave_time* para)
{
//    LOGLOG("lshell cmd: save time set");
    int err;
    err = lshell_setPsaveTime(para ,sizeof(*para));
    return err;
}

int LShell::savetime_get(cmdst_PSave_time* para)
{
//    LOGLOG("lshell cmd: save time get");
    int err;
    err = lshell_getPsaveTime(para ,sizeof(*para));
    return err;
}

int LShell::poweroff_set(cmdst_powerOff_time* para)
{
//    LOGLOG("lshell cmd: power off set");
    int err;
    err = lshell_setPowerOff(para ,sizeof(*para));
    return err;
}

int LShell::poweroff_get(cmdst_powerOff_time* para)
{
//    LOGLOG("lshell cmd: power off get");
    int err;
    err = lshell_getPowerOff(para ,sizeof(*para));
    return err;
}

int LShell::ipv4_set(net_info_st* para)
{
//    LOGLOG("lshell cmd: ipv4 set");
    int err;
    err = lshell_setv4(para ,sizeof(*para));
    return err;
}

int LShell::ipv4_get(net_info_st* para)
{
//    LOGLOG("lshell cmd: ipv4 get");
    int err;
    err = lshell_getv4(para ,sizeof(*para));
    return err;
}

int LShell::ipv6_set(net_ipv6_st* para)
{
//    LOGLOG("lshell cmd: ipv6 set");
    int err;
    err = lshell_setv6(para ,sizeof(*para));
    return err;
}

int LShell::ipv6_get(net_ipv6_st* para)
{
//    LOGLOG("lshell cmd: ipv6 get");
    int err;
    err = lshell_getv6(para ,sizeof(*para));
    return err;
}

int LShell::userconfig_set(cmdst_userconfig* para)
{
//    LOGLOG("lshell cmd: user config set");
    int err;
    err = lshell_set_userConfig(para ,sizeof(*para));
    return err;
}

int LShell::userconfig_get(cmdst_userconfig* para)
{
//    LOGLOG("lshell cmd: userconfig get");
    int err;
    err = lshell_get_userConfig(para ,sizeof(*para));
    return err;
}

int LShell::fusingsc_reset()
{
//    LOGLOG("lshell cmd: fusingsc reset");
    int err;
    cmdst_fusingScReset val = 0;
    cmdst_fusingScReset* para = &val;
    err = lshell_fusingScReset(para ,sizeof(*para));
    return err;
}

int LShell::toner_reset()
{
//    LOGLOG("lshell cmd: toner reset");
	int err;
    cmdst_tonerReset val = 0;
    cmdst_tonerReset* para = &val;
	err = lshell_tonerReset(para, sizeof(*para));
	return err;
}
int LShell::drum_reset()
{
//    LOGLOG("lshell cmd: drum reset");
    int err;
    cmdst_drumReset val = 0;
    cmdst_drumReset* para = &val;
	err = lshell_drumScReset(para, sizeof(*para));
	return err;
}
int LShell::usercenterinfo_get(cmdst_user_center* para)
{
//    LOGLOG("lshell cmd: usercenter info get");
	int err;
	err = lshell_getUserInfo(para, sizeof(*para));
	return err;
}
int LShell::printerinfo_get(fw_info_st* para)
{
//    LOGLOG("lshell cmd: printer info get");
	int err;
	err = lshell_getFWInfo(para, sizeof(*para));
	return err;
}

int LShell::poweroff()
{
    int err;
    unsigned char val = 1;
    unsigned char* para = &val;
    err = lshell_poweroff(para, sizeof(*para));
    return err;

}
int LShell::get_counter_print_scan(Counter_printer_scan* para)
{
    int err;
    err = lshell_getCounterPrintScan(para, sizeof(*para));
    return err;
}

int LShell::wifi_set_enable(unsigned char* para)
{
    int err;
    err = lshell_wifi_set_enable(para, sizeof(*para));
    return err;
}
int LShell::printer_reboot()
{
    int err;
//    unsigned char val = 0;
//    unsigned char* para = &val;
//    err = lshell_prn_reboot(para, sizeof(*para));
    err = lshell_prn_reboot(NULL, 0);
    return err;
}

int LShell::get_consumable_supply_info(Consumabale_supply_info* para)
{
    int err;
    err = lshell_getConsumableSupplyInfo(para, sizeof(*para));
    return err;
}
void LShell::copy_get_defaultPara(copycmdset* p)
{
    memcpy(p ,&default_copy_parameter ,sizeof(default_copy_parameter));
}

int LShell::open(Printer_struct* printer)
{
    int port = -1;
    device = device_manager->getDevice(printer);
    if(!device)
    {
        LOGLOG("get device fail");
        return -1;
    }
    if(device->type() == DeviceIO::Type_net){
        port = 9100;
    }
    return device->open(printer ,port);
}

int LShell::close(void)
{
    if(!device)
        return -1;
    return device->close();
}

int device_init(DeviceIO* device)
{
    if(!device)
        return -1;
    int ret = 0;
    int device_type = device->type();
    if(device_type == DeviceIO::Type_net){
        return 0;
    }else if(device_type == DeviceIO::Type_unknown){
        return -1;
    }

    char buffer[1460];
    ret = device->getDeviceId_without_open(buffer ,sizeof(buffer));
//    if(ret){
//        char inBuffer[522] = {0};
//        char outBuffer[12] = {0};

//        inBuffer[0] = 0x1b;
//        inBuffer[1] = 0x4d;
//        inBuffer[2] = 0x53;
//        inBuffer[3] = 0x55;
//        inBuffer[4] = 0xe0;
//        inBuffer[5] = 0x2b;
//        device->write_bulk(inBuffer ,10 ,1);
//        device->write_bulk(inBuffer+10 ,512 ,1);

//        device->read_bulk(outBuffer ,sizeof(outBuffer) ,1);
//    }

    if(ret)
        device->read(buffer ,sizeof(buffer));
    return 0;
}

#include <unistd.h>
int LShell::writeThenRead(char* wrBuffer ,int wrSize ,char* rdBuffer ,int rdSize)
{
    if(!device)
        return -1;
    int ret = 0;
    char write_buffer[wrSize];
    memcpy(write_buffer ,wrBuffer ,wrSize);
    ret = device_init(device);
    int sleep_time = 0;
    if(device->type() == DeviceIO::Type_usb)
        sleep_time = 1;
    if(!ret){
//        LOGLOG("lshell write cmd:"
//               "0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x"
//               " ,0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x"
//               ,write_buffer[0] ,write_buffer[1] ,write_buffer[2] ,write_buffer[3] ,write_buffer[4] ,write_buffer[5]
//                ,write_buffer[6] ,write_buffer[7] ,write_buffer[8] ,write_buffer[9] ,write_buffer[10]);

        ret = device->write(write_buffer ,wrSize);

        if(ret == wrSize || device->type() == DeviceIO::Type_usb){
            for(int i = 0 ;i < 20 ;i++){
                ret = device->read(rdBuffer ,rdSize);
                if(ret == rdSize){
                    if( (rdBuffer[0] == 0x4d)
                    && (rdBuffer[1] == 0x3c)
                    && (rdBuffer[2] == 0x2b)
                    && (rdBuffer[3] == 0x1a)){
                        ret = 0;
                        break;
                    }
                    ret = -1;
                }else{
//                    LOGLOG("read wrong");
                    ret = -1;
                }
                usleep(sleep_time  * 250 * 1000);
            }
        }
    }
//    LOGLOG("lshell read cmd:"
//           "0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x"
//           " ,0x%02x ,0x%02x ,0x%02x ,0x%02x ,0x%02x"
//           ,rdBuffer[0] ,rdBuffer[1] ,rdBuffer[2] ,rdBuffer[3] ,rdBuffer[4] ,rdBuffer[5]
//            ,rdBuffer[6] ,rdBuffer[7] ,rdBuffer[8] ,rdBuffer[9] ,rdBuffer[10]);
    return ret;
}
