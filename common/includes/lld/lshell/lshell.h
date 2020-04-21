/////////////////////////////////////////
/// Author:Jacky Liang
/// Version:
/////////////////////////////////////////
#ifndef LSHELL
#define LSHELL

#include "lshellstruct.h"
#include "jkinterface.h"
class LShell
{
public:
    LShell(DeviceIOManager* _device_manager)
        :device_manager(_device_manager){}

public:
    enum{
        ERR_ACK = 0,
        ERR_CMD_invalid = 1,
        ERR_Parameter_invalid = 2,
        ERR_Do_not_support = 3,
        ERR_Printer_busy = 4,
        ERR_Printer_error = 5,
        ERR_Set_parameter_error = 6,
        ERR_Get_parameter_error = 7,
        ERR_Printer_is_Sleeping = 8,
        ERR_Printer_is_in_error = 9,
        ERR_Password_incorrect = 0x0a,
        ERR_Scanner_operation_NG =0x0b,

        //err self defined
        ERR_communication = -1,
        ERR_wrong_para = -2,
        ERR_no_device = -3,
        ERR_wifi_have_not_been_inited = -4,
        ERR_WIFI_SET_SSID = -5,
        ERR_lshell_cannot_support = -6
    };

public:
    int copy(copycmdset* para);
    int copy_get_para(copycmdset* para);
    int wifi_apply(cmdst_wifi_get* para);
    int wifi_get_para(cmdst_wifi_get* para);
    int wifi_get_aplist(cmdst_aplist_get* para);
    int wifi_get_status(cmdst_wifi_status* para);
    int wifi_softap_set(cmdst_softap* para);
    int wifi_softap_get(cmdst_softap* para);
    int password_set(cmdst_passwd* para);
    int password_get(cmdst_passwd* para);
    int password_confirm(cmdst_passwd* para);
    int tonerend_set(cmdst_tonerEnd* para);
    int tonerend_get(cmdst_tonerEnd* para);
    int savetime_set(cmdst_PSave_time* para);
    int savetime_get(cmdst_PSave_time* para);
    int poweroff_set(cmdst_powerOff_time* para);
    int poweroff_get(cmdst_powerOff_time* para);
    int ipv4_set(net_info_st* para);
    int ipv4_get(net_info_st* para);
    int ipv6_set(net_ipv6_st* para);
    int ipv6_get(net_ipv6_st* para);
    int userconfig_set(cmdst_userconfig* para);
    int userconfig_get(cmdst_userconfig* para);
    int fusingsc_reset();

    int toner_reset();
    int drum_reset();
	int usercenterinfo_get(cmdst_user_center* para);
	int printerinfo_get(fw_info_st* para);

    int poweroff();
    int get_counter_print_scan(Counter_printer_scan* para);
    int wifi_set_enable(unsigned char* para);
    int printer_reboot();

    int get_consumable_supply_info(Consumabale_supply_info* para);
public:
    static void copy_get_defaultPara(copycmdset* p);
    int open(Printer_struct* printer);
    int close(void);

private:
    int lshell_cmd(int cmd ,int sub_cmd, void* data ,int data_size);
    int writeThenRead(char* wrBuffer ,int wrSize ,char* rdBuffer ,int rdSize);
    DeviceIO* device;
    DeviceIOManager* device_manager;
};
#endif // LSHELL

