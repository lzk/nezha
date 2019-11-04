#include "settingsstackedwidget.h"
#include "ui_settingsstackedwidget.h"
#include "uinterface.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QTabWidget>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include "authenticationdlg.h"
#include "settingwarming.h"
#include "qdebug.h"
#include <qbuttongroup.h>
#include "promptdialog.h"

SettingsStackedWidget::SettingsStackedWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::SettingsStackedWidget)
{
    ui->setupUi(this);
    this->setCurrentIndex(0);
    ui->label_timeoutSettings->hide();

    scrollArea = new QScrollArea(ui->WiFi);
    scrollArea->setFocusPolicy(Qt::NoFocus);
    scrollArea->setGeometry(QRect(13, 14, 235, 274));
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setStyleSheet("QScrollBar:vertical"
                            "{"
                            "width:10px;"
                            "background:rgba(0,0,0,0%);"
                            "margin:0px,0px,0px,0px;"
                            "padding-top:12px;"
                            "padding-bottom:12px;"
                            "}"
                            "QScrollBar::handle:vertical"
                            "{"
                            "width:8px;"
                            "background:rgba(0,0,0,25%);"
                            " border-radius:5px;"
                            "min-height:20;"
                            "}"
                            "QScrollBar::handle:vertical:hover"
                            "{"
                            "width:8px;"
                            "background:rgba(0,0,0,50%);"
                            " border-radius:5px;"
                            "min-height:20;"
                            "}"
                            "QScrollBar::add-line:vertical"
                            "{"
                            "height:12px;width:12px;"
                            "subcontrol-position:bottom;"
                            "}"
                            "QScrollBar::sub-line:vertical"
                            "{"
                            "height:12px;width:12px;"
                            "subcontrol-position:top;"
                            "}"
                            "QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
                            "{"
                            "background:rgba(0,0,0,0%);"
                            "border-radius:4px;"
                            "}"
                            );
    isLogn = false;     //isLogn :default false; false:did not comfirm the pw in the device;
    titelCell = new WlanTitleCell(scrollArea,scrollArea->verticalScrollBar(), true, &isLogn);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(titelCell);
    timer1 = new QTimer(this);
    initAP();
    initIP();
    initPowerSave();
    initPassword();
    initAdvanceSetting(userconfig, true);
    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)), this ,SLOT(cmdResult(int,int,QVariant)));


    buttonGroupIP = new QButtonGroup(this);
    buttonGroupIP->addButton(ui->btDHCP,0);
    buttonGroupIP->addButton(ui->btStatic,1);
    connect(ui->btDHCP,SIGNAL(clicked(bool)),this,SLOT(onRadioClickIP(bool)));
    connect(ui->btStatic,SIGNAL(clicked(bool)),this,SLOT(onRadioClickIP(bool)));
    ui->btDHCP->setChecked(true);
    onbtDHCPtoggled(true);
    onbtStatictoggled(false);

    buttonGroupDNS = new QButtonGroup(this);
    buttonGroupDNS->addButton(ui->btAuto,0);
    buttonGroupDNS->addButton(ui->btManual,1);
    connect(ui->btAuto,SIGNAL(clicked(bool)),this,SLOT(onRadioClickDNS(bool)));
    connect(ui->btManual,SIGNAL(clicked(bool)),this,SLOT(onRadioClickDNS(bool)));
    ui->btAuto->setChecked(true);
    onbtAutotoggled(true);
    onbtManualtoggled(false);

    //hide the softAP close/open button
    ui->label_close_AP->hide();
    ui->label_open_AP->hide();
    ui->btAPOpen->hide();

    ui->lineEdit_IPAddressv4->setMaxLength(15);
    ui->lineEdit_Gatewayv4->setMaxLength(15);
    ui->lineEdit_Submaskv4->setMaxLength(15);
    ui->lineEdit_server->setMaxLength(15);
    ui->lineEdit_timeout->setMaxLength(2);

    QRegExp regexp1("^[.0-9]+$");
    QValidator *validator1 = new QRegExpValidator(regexp1, this);
    ui->lineEdit_IPAddressv4->setValidator(validator1);
    ui->lineEdit_Gatewayv4->setValidator(validator1);
    ui->lineEdit_Submaskv4->setValidator(validator1);
    ui->lineEdit_server->setValidator(validator1);

    QRegExp regexp2("[1-9][0-9]+$");
    QValidator *validator2 = new QRegExpValidator(regexp2, this);
    ui->lineEdit_timeout->setValidator(validator2);

    QRegExp regexp3("-{0,1}[0-2]{1,2}");
    QRegExp regexp4("-{0,1}[0-6]{1,2}");
    QRegExp regexp5("-{0,1}[0-3]{1,2}");
    QRegExpValidator *validator3 = new QRegExpValidator(regexp3,this);
    QRegExpValidator *validator4 = new QRegExpValidator(regexp4,this);
    QRegExpValidator *validator5 = new QRegExpValidator(regexp5,this);
    ui->lineEdit_TopMargin->setValidator(validator3);
    ui->lineEdit_LeftMargin->setValidator(validator4);
    ui->lineEdit_ImageDensity->setValidator(validator5);

    ui->btTonerReset->hide();
}

SettingsStackedWidget::~SettingsStackedWidget()
{
    delete timer1;
    delete ui;
}

void SettingsStackedWidget::cmdResult(int cmd,int result,QVariant data)
{
//    if(this->isEnabled()){
        switch(cmd)
        {
        case UIConfig::LS_CMD_WIFI_Get_SoftAp:
            if(!result || UIConfig::WarmingUp == result)
            {
//                qDebug()<<QString(softap.ssid).left(32)<<":"<<softap.pwd;
                softap = data.value<cmdst_softap>();
                ui->lineEdit_SSID_AP->setText(QString(softap.ssid).left(32));
                ui->lineEdit_Password_AP->setText(QString(softap.pwd));
                isAPOpen = 6!=(softap.wifiEnable & 6);          //bit0: Wi-Fi Enable, bit1: GO Enable, bit2: P2P Enable
                on_btAPOpen_clicked();
            }
            else
            {
                ui->lineEdit_SSID_AP->clear();                  //clear the data last time;
                ui->lineEdit_Password_AP->clear();
            }
            gUInterface->emitEnableCycleAnimation(false);
            break;
        case UIConfig::LS_CMD_WIFI_Set_SoftAp:
        {
            QString deviceMsg;
            if(!result)
                deviceMsg = tr("ResStr_Msg_1");
            else
                deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;
        case UIConfig::LS_CMD_NET_GetV4:
            if(!result || UIConfig::WarmingUp == result)
            {
                info_ipv4 = data.value<net_info_st>();
                info_ipv4_orn = info_ipv4;
                initIP();
            }
            else
            {
                ui->lineEdit_IPAddressv4->setText("0.0.0.0");
                ui->lineEdit_Gatewayv4->setText("0.0.0.0");
                ui->lineEdit_Submaskv4->setText("0.0.0.0");
                ui->lineEdit_server->setText("0.0.0.0");
            }
            gUInterface->emitEnableCycleAnimation(false);
            break;

        case UIConfig::LS_CMD_NET_SetV4:
        {
            QString deviceMsg;
            if(!result )
            {
                info_ipv4 = info_ipv4_orn;
                deviceMsg = tr("ResStr_Msg_1");
            }
            else
                deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;

        case UIConfig::LS_CMD_PRN_PSaveTime_Get:
            if(!result || UIConfig::WarmingUp == result) {
                psavetm = data.value<cmdst_PSave_time>();
                int num;
                if(psavetm == 254){
                    num = 1;
                    ui->radio_default20->setChecked(true);
                    ui->widget_self_powersave->setEnabled(false);
                }else if(psavetm == 255){
                    num = 1;
                    ui->radio_never_powersave->setChecked(true);
                    ui->widget_self_powersave->setEnabled(false);
                }else{
                    num = psavetm;
                    ui->radio_self_powersave->setChecked(true);
                    ui->widget_self_powersave->setEnabled(true);
                }
                QString str;
                str.setNum(num);
                ui->lineEdit_timeout->setText(str);
            }
            gUInterface->emitEnableCycleAnimation(false);
            break;
        case UIConfig::LS_CMD_PRN_PSaveTime_Set:
        {
            QString deviceMsg;
            if(!result)
                deviceMsg = tr("ResStr_Setting_Successfully_");
            else
                deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;

        case UIConfig::LS_CMD_PRN_Get_UserConfig:
            if(!result || UIConfig::WarmingUp == result) {
                userconfig = data.value<cmdst_userconfig>();
                userconfig_orn = userconfig;
                initAdvanceSetting(userconfig, false);
            }
            else{
                initAdvanceSetting(userconfig, true);
            }
            gUInterface->emitEnableCycleAnimation(false);
            break;
        case UIConfig::LS_CMD_PRN_Set_UserConfig:
        {
            QString deviceMsg;
            if(!result)
            {
                userconfig = userconfig_orn;
                deviceMsg = tr("ResStr_Setting_Successfully_");
            }
            else {
                deviceMsg = tr("ResStr_Setting_Fail");
            }
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            gUInterface->emitEnableCycleAnimation(false);
        }

            break;
        case UIConfig::LS_CMD_PRN_FusingScReset:
        {
            QString deviceMsg;
            if(!result)
            {

                deviceMsg = tr("ResStr_Setting_Successfully_");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);

                SettingWarming *warming = new SettingWarming(this, tr("ResStr_Please_turn_off_the_printer_until_it_cools_to_room_temperature"), 1);
                warming->setWindowTitle(tr("ResStr_Prompt"));

                warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                warming->exec();
            }
            else {
                deviceMsg = tr("ResStr_Setting_Fail");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            }
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;
        case UIConfig::LS_CMD_PRN_DrumReset:
        {
            QString deviceMsg;
            if(!result)
            {
                deviceMsg = tr("ResStr_Setting_Successfully_");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);

                SettingWarming *warming = new SettingWarming(this, tr("ResStr_Please_turn_off_the_printer_until_it_cools_to_room_temperature"), 1);
                warming->setWindowTitle(tr("ResStr_Prompt"));

                warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                warming->exec();
            }
            else {
                deviceMsg = tr("ResStr_Setting_Fail");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            }
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;
        case UIConfig::LS_CMD_PRN_TonerReset:
        {
            QString deviceMsg;
            if(!result)
            {
                deviceMsg = tr("ResStr_Setting_Successfully_");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);

                SettingWarming *warming = new SettingWarming(this, tr("ResStr_Please_turn_off_the_printer_until_it_cools_to_room_temperature"), 1);
                warming->setWindowTitle(tr("ResStr_Prompt"));

                warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                warming->exec();
            }
            else {
                deviceMsg = tr("ResStr_Setting_Fail");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            }
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;
        case UIConfig::LS_CMD_PASSWD_set:
        {
            QString deviceMsg;
            if(!result)
                deviceMsg = tr("ResStr_Setting_Successfully_");
            else
                deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            gUInterface->emitEnableCycleAnimation(false);
        }
            break;
        default: break;
        }
}

void SettingsStackedWidget::changeStackIndex(int index)
{
    switch(index)
    {
    case 0:                                     //wifi setting
        {
            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");

            titelCell->setCurrentIndex(0);
            if(this->isEnabled() == true)
            {
                titelCell->on_btFlesh_clicked();
            }
            else
            {
                titelCell->emptyWifiList();
            }
        }
        break;
    case 1:                                     //softAP setting
        {
            ui->lineEdit_SSID_AP->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_Password_AP->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_SSID_AP->setFocus();

            ui->lineEdit_IPAddressv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Submaskv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Gatewayv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_server->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_timeout->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_TopMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_LeftMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_ImageDensity->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_newPassWord->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_confirm->setFocusPolicy(Qt::NoFocus);

            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");
            if(this->isEnabled() == true)
            {
                gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_Get_SoftAp);
                gUInterface->emitEnableCycleAnimation(true);
            }
            else
            {
                ui->lineEdit_SSID_AP->clear();                  //clear the data last time;
                ui->lineEdit_Password_AP->clear();
            }
        }
        break;
    case 2:                                    // ip settting
        {
            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");

            ui->lineEdit_SSID_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Password_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_IPAddressv4->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_Submaskv4->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_Gatewayv4->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_server->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_timeout->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_TopMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_LeftMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_ImageDensity->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_newPassWord->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_confirm->setFocusPolicy(Qt::NoFocus);

            ui->lineEdit_IPAddressv4->setText("0.0.0.0");
            ui->lineEdit_Gatewayv4->setText("0.0.0.0");
            ui->lineEdit_Submaskv4->setText("0.0.0.0");
            ui->lineEdit_server->setText("0.0.0.0");

            if(this->isEnabled() == true)
            {
                QVariant data;
                data.setValue<net_info_st>(info_ipv4);
                gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_NET_GetV4);
                gUInterface->emitEnableCycleAnimation(true);
            }
        }
        break;
    case 3:                                    // ip settting
        {
            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");
        }
        break;
    case 4:                                    // power save time setting
        {
            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");

            ui->lineEdit_SSID_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Password_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_IPAddressv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Submaskv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Gatewayv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_server->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_timeout->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_TopMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_LeftMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_ImageDensity->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_newPassWord->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_confirm->setFocusPolicy(Qt::NoFocus);

            if(this->isEnabled() == true)
            {
                gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_PSaveTime_Get);
                gUInterface->emitEnableCycleAnimation(true);
            }
            else
            {
                QRegExp regexp3("-{0,1}[0-2]{1,2}");
                QRegExp regexp4("-{0,1}[0-6]{1,2}");
                QRegExp regexp5("-{0,1}[0-3]{1,2}");
                QRegExpValidator *validator3 = new QRegExpValidator(regexp3,this);
                QRegExpValidator *validator4 = new QRegExpValidator(regexp4,this);
                QRegExpValidator *validator5 = new QRegExpValidator(regexp5,this);
                ui->lineEdit_TopMargin->setValidator(validator3);
                ui->lineEdit_LeftMargin->setValidator(validator4);
                ui->lineEdit_ImageDensity->setValidator(validator5);

                ui->lineEdit_TopMargin->setText("0");
                ui->lineEdit_LeftMargin->setText("0");
                ui->lineEdit_ImageDensity->setText("0");
                ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
                isLowHumidity = false;

                //ui->label_setting_error->hide();
                ui->label_TopMargin_error->hide();
                ui->label_LeftMargin_error->hide();
                ui->label_ImageDensity_error->hide();
                ui->lineEdit_TopMargin->installEventFilter(this);
                ui->lineEdit_LeftMargin->installEventFilter(this);
                ui->lineEdit_ImageDensity->installEventFilter(this);
            }
        }
        break;
    case 5:                                   // advance setting
        {
            //bms 7428
            ui->lineEdit_newPassWord->setText("");
            ui->lineEdit_confirm->setText("");

            ui->lineEdit_SSID_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Password_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_IPAddressv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Submaskv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Gatewayv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_server->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_timeout->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_TopMargin->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_LeftMargin->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_ImageDensity->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_TopMargin->setFocus();
            ui->lineEdit_newPassWord->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_confirm->setFocusPolicy(Qt::NoFocus);

            if(this->isEnabled() == true)
            {
                gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_Get_UserConfig);
                gUInterface->emitEnableCycleAnimation(true);
            }
        }
        break;
    case 6:                                   //new password setting
        {
            ui->lineEdit_SSID_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Password_AP->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_IPAddressv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Submaskv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_Gatewayv4->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_server->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_timeout->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_TopMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_LeftMargin->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_ImageDensity->setFocusPolicy(Qt::NoFocus);
            ui->lineEdit_newPassWord->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_confirm->setFocusPolicy(Qt::StrongFocus);
            ui->lineEdit_newPassWord->setFocus();
        }
        break;
    default:
        break;
    }
}

void SettingsStackedWidget::setNetEnabled(bool enabled)
{
    ui->WiFi->setHidden(!enabled);
    ui->SoftAP->setHidden(!enabled);
//    ui->TCPIPV4->setHidden(false);
//    ui->TCPIPV6->setHidden(false);
    titelCell->setWifiEnabled(false);

    QWidget* widget = ui->background_WiFi2;
    int y = widget->y();
    if(enabled)
    {
        y += 80;
    }
    ui->background_TCPIPV42->setGeometry(widget->x(),y,widget->width(),widget->height());
    ui->background_TCPIPV62->setGeometry(widget->x(),y+40,widget->width(),widget->height());
    ui->background_PowerSave2->setGeometry(widget->x(),y+80,widget->width(),widget->height());
    ui->background_UserConfig2->setGeometry(widget->x(),y+120,widget->width(),widget->height());
    ui->background_Password2->setGeometry(widget->x(),y+160,widget->width(),widget->height());

}

void SettingsStackedWidget::setWifiEnabled(bool enabled)
{
    ui->WiFi->setHidden(!enabled);
    if(enabled)
    {
        ui->background_PowerSave2->setGeometry(ui->background_TCPIPV62->geometry().x(),ui->background_TCPIPV62->geometry().y() + 40,ui->background_TCPIPV62->geometry().width(),ui->background_TCPIPV62->height());
        ui->background_UserConfig2->setGeometry(ui->background_TCPIPV62->geometry().x(),ui->background_TCPIPV62->geometry().y() + 80,ui->background_TCPIPV62->geometry().width(),ui->background_TCPIPV62->height());
        ui->background_Password2->setGeometry(ui->background_TCPIPV62->geometry().x(),ui->background_TCPIPV62->geometry().y() + 120,ui->background_TCPIPV62->geometry().width(),ui->background_TCPIPV62->height());
        titelCell->setWifiEnabled(true);
    }
    else
    {
        ui->SoftAP->setHidden(true);
        ui->TCPIPV4->setHidden(true);
        ui->TCPIPV6->setHidden(true);
        ui->background_PowerSave2->setGeometry(ui->background_WiFi2->geometry());
        ui->background_UserConfig2->setGeometry(ui->background_SoftAP2->geometry());
        ui->background_Password2->setGeometry(ui->background_TCPIPV42->geometry());
        titelCell->setWifiEnabled(false);
    }

}

void SettingsStackedWidget::initAP()
{
   // if(NULL != device)
    {
       // device->
    }
    isAPOpen = true;
    ui->btAPOpen->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
    ui->lineEdit_SSID_AP->setText(tr(""));
    ui->lineEdit_SSID_AP->setCursorPosition(1);
    ui->lineEdit_Password_AP->clear();

    QRegExp rx1("^[\\x0020-\\x007e]{1,32}$");
    QRegExp rx2("^[\\x0020-\\x007e]{8,64}$");
    QRegExpValidator *validator1 = new QRegExpValidator(rx1, this);
    QRegExpValidator *validator2 = new QRegExpValidator(rx2, this);
    ui->lineEdit_SSID_AP->setValidator(validator1);
    ui->lineEdit_Password_AP->setValidator(validator2);
}

void SettingsStackedWidget::initIP()
{
    ui->label_ipaddr_error->hide();
    ui->label_gateway_error->hide();
    ui->label_sumask_error->hide();
    ui->label_server_error->hide();
    ui->lineEdit_IPAddressv4->installEventFilter(this);
    ui->lineEdit_Gatewayv4->installEventFilter(this);
    ui->lineEdit_Submaskv4->installEventFilter(this);
    ui->lineEdit_server->installEventFilter(this);

    if(0 == info_ipv4.SubnetMask[0])            //if true, that means cmd_get_ipv4 did not success;
    {
        info_ipv4.IPAddressMode = 3;            //to set default value;
        info_ipv4.IPMode = 1;
    }
//    if(0==info_ipv4.IPMode)                   /*****IPV4 mode *******/
//    {
        if(info_ipv4.IPAddressMode < 4)         //DHCP mode
        {
             ui->btDHCP->setChecked(true);
             onbtDHCPtoggled(true);
             onbtStatictoggled(false);
             QString text = "%1.%2.%3.%4";
             text = text.arg(info_ipv4.IPAddress[0]).arg(info_ipv4.IPAddress[1]).\
                                        arg(info_ipv4.IPAddress[2]).arg(info_ipv4.IPAddress[3]);
             ui->lineEdit_IPAddressv4->setText(tr(text.toLatin1()));
             text = "%1.%2.%3.%4";
             text = text.arg(info_ipv4.GatewayAddress[0]).arg(info_ipv4.GatewayAddress[1]).\
                                        arg(info_ipv4.GatewayAddress[2]).arg(info_ipv4.GatewayAddress[3]);
             ui->lineEdit_Gatewayv4->setText(tr(text.toLatin1()));
             text = "%1.%2.%3.%4";
             text = text.arg(info_ipv4.SubnetMask[0]).arg(info_ipv4.SubnetMask[1]).\
                                        arg(info_ipv4.SubnetMask[2]).arg(info_ipv4.SubnetMask[3]);
             ui->lineEdit_Submaskv4->setText(tr(text.toLatin1()));
        }
        else if(4 == info_ipv4.IPAddressMode)       //static ip mode
        {
            ui->btStatic->setChecked(true);
            onbtStatictoggled(true);
            onbtDHCPtoggled(false);
            QString text = "%1.%2.%3.%4";
            text = text.arg(info_ipv4.IPAddress[0]).arg(info_ipv4.IPAddress[1]).\
                                       arg(info_ipv4.IPAddress[2]).arg(info_ipv4.IPAddress[3]);
            ui->lineEdit_IPAddressv4->setText(tr(text.toLatin1()));
            text = "%1.%2.%3.%4";
            text = text.arg(info_ipv4.GatewayAddress[0]).arg(info_ipv4.GatewayAddress[1]).\
                                       arg(info_ipv4.GatewayAddress[2]).arg(info_ipv4.GatewayAddress[3]);
            ui->lineEdit_Gatewayv4->setText(tr(text.toLatin1()));
            text = "%1.%2.%3.%4";
            text = text.arg(info_ipv4.SubnetMask[0]).arg(info_ipv4.SubnetMask[1]).\
                                       arg(info_ipv4.SubnetMask[2]).arg(info_ipv4.SubnetMask[3]);
            ui->lineEdit_Submaskv4->setText(tr(text.toLatin1()));
        }

        if(info_ipv4.UseManualDNS == 1)
        {
            ui->btManual->setChecked(true);
            onbtManualtoggled(true);
            onbtAutotoggled(false);
            QString text = "%1.%2.%3.%4";
            text = text.arg(info_ipv4.DNSAddress[0]).arg(info_ipv4.DNSAddress[1]).\
                                       arg(info_ipv4.DNSAddress[2]).arg(info_ipv4.DNSAddress[3]);
            ui->lineEdit_server->setText(tr(text.toLatin1()));
        }
        else if(info_ipv4.UseManualDNS == 0)
        {
            ui->btAuto->setChecked(true);
            onbtAutotoggled(true);
            onbtManualtoggled(false);
            QString text = "%1.%2.%3.%4";
            text = text.arg(info_ipv4.DNSAddress[0]).arg(info_ipv4.DNSAddress[1]).\
                                       arg(info_ipv4.DNSAddress[2]).arg(info_ipv4.DNSAddress[3]);
            ui->lineEdit_server->setText(tr(text.toLatin1()));
        }
//    }
}

void SettingsStackedWidget::initPowerSave()
{
    ui->lineEdit_timeout->setText("1");
//    QRegExp rx4("^[0-9]{0,3}$");
    QRegExp rx4("[1-9][0-9]+$");
    QRegExpValidator *validator4 = new QRegExpValidator(rx4, this);
    ui->lineEdit_timeout->setValidator(validator4);
    ui->lineEdit_timeout->installEventFilter(this);

    ui->label_timeout_error->hide();
}

void SettingsStackedWidget::initPassword()
{
//    QRegExp rx4("^[\\x0020-\\x0076]{0,32}$");

    QRegExp rx4("^[0-9a-zA-Z]{0,32}$");
    QRegExpValidator *validator4 = new QRegExpValidator(rx4, this);
    ui->lineEdit_newPassWord->setValidator(validator4);
    ui->lineEdit_confirm->setValidator(validator4);
}


void SettingsStackedWidget::on_btTimeReduce_clicked()
{
    int val = ui->lineEdit_timeout->text().toInt();
    if(val>1)
    {
        val--;
        ui->lineEdit_timeout->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btTimeAdd_clicked()
{
    int val = ui->lineEdit_timeout->text().toInt();
    if(val<30)
    {
        val++;
        ui->lineEdit_timeout->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_lineEdit_timeout_textEdited(const QString &arg1)
{
    QRegExp rx2("^[0]{0,1}[0-2]{0,1}[1-9]{1}|10|20|30$");
    if( !rx2.exactMatch(arg1) )
    {
        ui->label_timeout_error->show();
        ui->lineEdit_timeout->setStyleSheet("QLineEdit{border:2px solid red;border-radius:15px;}");
        ui->btTimeAdd->setDisabled(true);
        ui->btTimeReduce->setDisabled(true);
        ui->btApply_Timeout->setDisabled(true);

    }
    else
    {
        ui->label_timeout_error->hide();
        ui->lineEdit_timeout->setStyleSheet("QLineEdit{border:transparent;}");
        ui->btTimeAdd->setEnabled(true);
        ui->btTimeReduce->setEnabled(true);
        ui->btApply_Timeout->setEnabled(true);
    }
}

void SettingsStackedWidget::initAdvanceSetting(cmdst_userconfig config, bool isFirst)
{
    if(isFirst)             //if ture, set the default value;
    {
        QRegExp regexp3("-{0,1}[0-2]{1,2}");
        QRegExp regexp4("-{0,1}[0-6]{1,2}");
        QRegExp regexp5("-{0,1}[0-3]{1,2}");
        QRegExpValidator *validator3 = new QRegExpValidator(regexp3,this);
        QRegExpValidator *validator4 = new QRegExpValidator(regexp4,this);
        QRegExpValidator *validator5 = new QRegExpValidator(regexp5,this);
        ui->lineEdit_TopMargin->setValidator(validator3);
        ui->lineEdit_LeftMargin->setValidator(validator4);
        ui->lineEdit_ImageDensity->setValidator(validator5);

        ui->lineEdit_TopMargin->setText("0");
        ui->lineEdit_LeftMargin->setText("0");
        ui->lineEdit_ImageDensity->setText("0");
        ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        isLowHumidity = false;

        //ui->label_setting_error->hide();
        ui->label_TopMargin_error->hide();
        ui->label_LeftMargin_error->hide();
        ui->label_ImageDensity_error->hide();
        ui->lineEdit_TopMargin->installEventFilter(this);
        ui->lineEdit_LeftMargin->installEventFilter(this);
        ui->lineEdit_ImageDensity->installEventFilter(this);
    }
    else                //if get the value form device, set the value got;
    {
        ui->lineEdit_ImageDensity->setText(QString::number(config.ImageDensity));
        ui->lineEdit_LeftMargin->setText(QString::number(config.SideToSide));
        ui->lineEdit_TopMargin->setText(QString::number(config.LeadingEdge));

        isLowHumidity = (0 == config.LowHumidityMode ? false : true);
        if(isLowHumidity)
            ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
        else
            ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");

    }
}

void SettingsStackedWidget::on_btAPOpen_clicked()
{
    isAPOpen = !isAPOpen;
    if(isAPOpen)
        ui->btAPOpen->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
    else
        ui->btAPOpen->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
}

void SettingsStackedWidget::on_btApply_AP_clicked()
{
    int ssid_len = ui->lineEdit_SSID_AP->text().length();
    int psd_len = ui->lineEdit_Password_AP->text().length();

    if(ssid_len < 1)
    {
        SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_Msg_9"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
        msgWarm->exec();
    }
    else if(psd_len < 8)
    {
        SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_Msg_3"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
    }
    else
    {
        if(!isLogn)                                     //if have not confirmed, show the authentication dialog;
        {
            AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
            dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
            dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
            dlg->exec();
        }
        if(isLogn )//&& CMD_STATUS_COMPLETE == device->get_cmdStatus())
        {
            QString ssid = ui->lineEdit_SSID_AP->text();
            memset(softap.ssid, 0, 32);
            memcpy(softap.ssid, ssid.left(32).toLatin1(), 32);
            QString psw = ui->lineEdit_Password_AP->text();
            memset(softap.pwd, 0, 64);
            memcpy(softap.pwd, psw.left(32).toLatin1(), 64);
            softap.wifiEnable = isAPOpen ? 7 : 1 ;
            QVariant data;
            data.setValue<cmdst_softap>(softap);
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_Set_SoftAp,data);     //setting softap
            gUInterface->emitEnableCycleAnimation(true);
            LOGLOG("customer setting soft AP");
        }
        else
        {
            QString deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
        }
    }
}

//set ipv4 parameters
void SettingsStackedWidget::on_btApply_IPConfig_clicked()
{
    QStringList ipaddress = ui->lineEdit_IPAddressv4->text().split(".");
    QStringList ipgateway = ui->lineEdit_Gatewayv4->text().split(".");
    QStringList ipsubmask = ui->lineEdit_Submaskv4->text().split(".");
    QStringList serverAddress = ui->lineEdit_server->text().split(".");

    if(ui->btDHCP->isChecked())             //using DHCP of ipv4
    {
        info_ipv4.IPMode = 0;
        info_ipv4.IPAddressMode = 3;
    }
    if(ui->btStatic->isChecked())           //setting static ipv4 address
    {
        info_ipv4.IPMode = 0;
        info_ipv4.IPAddressMode = 4;
        for(int i = 0; i<4; i++)
        {
            info_ipv4.IPAddress[i] = ipaddress.at(i).toInt();
            info_ipv4.GatewayAddress[i] = ipgateway.at(i).toInt();
            info_ipv4.SubnetMask[i] = ipsubmask.at(i).toInt();
        }
        if(info_ipv4.IPAddress[0] >= 224 ||info_ipv4.IPAddress[0] == 127 )
        {
            SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_The_entered_IP_address__is_wrong__please_confirm_and_enter_again_"));
            msgWarm->setWindowTitle(tr("ResStr_Warning"));
            msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
            msgWarm->exec();

            return;
        }
        if(info_ipv4.GatewayAddress[0] >= 224 ||info_ipv4.GatewayAddress[0] == 127 )
        {
            SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_The_entered_Gateway_is_wrong__please_confirm_and_enter_again_"));
            msgWarm->setWindowTitle(tr("ResStr_Warning"));
            msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
            msgWarm->exec();

            return;
        }

    }
    if(ui->btAuto->isChecked())
    {
        info_ipv4.UseManualDNS = 0;
    }
    if(ui->btManual->isChecked())
    {
        info_ipv4.UseManualDNS = 1;
        for(int i = 0; i<4; i++)
        {
            info_ipv4.DNSAddress[i] = serverAddress.at(i).toInt();
        }
        if(info_ipv4.DNSAddress[0] >= 224 ||info_ipv4.DNSAddress[0] == 127 )
        {
            SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_The_entered_DNS_is_wrong__please_confirm_and_enter_again_"));
            msgWarm->setWindowTitle(tr("ResStr_Warning"));
            msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
            msgWarm->exec();

            return;
        }

    }
    if(!isLogn)
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(isLogn )//&& CMD_STATUS_COMPLETE == device->get_cmdStatus())
    {
        QVariant data;
        data.setValue<net_info_st>(info_ipv4);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_NET_SetV4,data);
        gUInterface->emitEnableCycleAnimation(true);
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }

    ui->btApply_IPConfig->setFocus();
}

void SettingsStackedWidget::onRadioClickIP(bool checked)
{
//    qDebug()<<"4"<<checked;
    if(buttonGroupIP->checkedId() == 0)
    {
//        qDebug()<<"0";
        onbtDHCPtoggled(checked);
        onbtStatictoggled(!checked);
    }
    else
    {
//        qDebug()<<"1";
        onbtStatictoggled(checked);
        onbtDHCPtoggled(!checked);
    }
}

void SettingsStackedWidget::onRadioClickDNS(bool checked)
{
    if(buttonGroupDNS->checkedId() == 0)
    {
        onbtAutotoggled(checked);
        onbtManualtoggled(!checked);
    }
    else
    {
        onbtAutotoggled(!checked);
        onbtManualtoggled(checked);
    }
}

void SettingsStackedWidget::onbtAutotoggled(bool checked)
{
    if(checked)
    {
        ui->lineEdit_server->setDisabled(true);
    }
    else
    {
        ui->lineEdit_server->setEnabled(true);
    }
}

void SettingsStackedWidget::onbtManualtoggled(bool checked)
{
    if(checked)
    {
        ui->lineEdit_server->setStyleSheet("QLineEdit{\
                                              border:2px solid rgb(170, 170, 170);\
                                              padding: 0 8px;\
                                              border-radius:12px;}");
    }
    else
    {
        ui->lineEdit_server->setStyleSheet("QLineEdit{\
                                          border:2px solid rgb(170, 170, 170);\
                                          border-radius:12px;\
                                          padding: 0 8px;\
                                          background-color: rgb(189, 189, 189);\
                                          color: rgb(255, 255, 255);}");
    }
}

void SettingsStackedWidget::onbtDHCPtoggled(bool checked)
{
    if(checked)
    {
        ui->lineEdit_IPAddressv4->setDisabled(true);
        ui->lineEdit_Gatewayv4->setDisabled(true);
        ui->lineEdit_Submaskv4->setDisabled(true);

//        ui->lineEdit_IPAddressv4->setStyleSheet("");
    }
    else
    {
        ui->lineEdit_IPAddressv4->setEnabled(true);
        ui->lineEdit_Gatewayv4->setEnabled(true);
        ui->lineEdit_Submaskv4->setEnabled(true);
    }
}

void SettingsStackedWidget::onbtStatictoggled(bool checked)
{
    if(checked)
    {
        ui->lineEdit_IPAddressv4->setStyleSheet("QLineEdit{\
                                                border:2px solid rgb(170, 170, 170);\
                                                padding: 0 8px;\
                                                border-radius:12px;}");
        ui->lineEdit_Gatewayv4->setStyleSheet("QLineEdit{\
                                              border:2px solid rgb(170, 170, 170);\
                                              padding: 0 8px;\
                                              border-radius:12px;}");
        ui->lineEdit_Submaskv4->setStyleSheet("QLineEdit{\
                                              border:2px solid rgb(170, 170, 170);\
                                              padding: 0 8px;\
                                              border-radius:12px;}");
    }
    else
    {
        ui->lineEdit_IPAddressv4->setStyleSheet("QLineEdit{\
                                                border:2px solid rgb(170, 170, 170);\
                                                padding: 0 8px;\
                                                border-radius:12px;\
                                                background-color: rgb(189, 189, 189);\
                                                color: rgb(255, 255, 255);}");
        ui->lineEdit_Gatewayv4->setStyleSheet("QLineEdit{\
                                              border:2px solid rgb(170, 170, 170);\
                                              padding: 0 8px;\
                                              border-radius:12px;\
                                              background-color: rgb(189, 189, 189);\
                                              padding: 0 8px;\
                                              color: rgb(255, 255, 255);}");
        ui->lineEdit_Submaskv4->setStyleSheet("QLineEdit{\
                                              border:2px solid rgb(170, 170, 170);\
                                              padding: 0 8px;\
                                              border-radius:12px;\
                                              background-color: rgb(189, 189, 189);\
                                              color: rgb(255, 255, 255);}");
    }
}

void SettingsStackedWidget::on_btIPv6Setting_clicked()
{
    settingIPv6Widget = new SettingForIPv6(this->parentWidget(), &isLogn);
    settingIPv6Widget->setWindowTitle(tr("ResStr_Setting"));
    settingIPv6Widget->exec();
}

void SettingsStackedWidget::on_btIPv6Status_clicked()
{
    ipv6Status = new IPv6Status(this);
    ipv6Status->setWindowTitle(tr("ResStr_Status"));
    ipv6Status->exec();
}

/**************
 *to setting userconfig value
*****************/
void SettingsStackedWidget::on_btApply_userconfig_clicked()
{
    if(!isLogn )//&& CMD_STATUS_COMPLETE == device->get_cmdStatus())
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(isLogn)
    {
        //if(CMD_STATUS_COMPLETE == device->get_cmdStatus())  {
            userconfig.LeadingEdge = ui->lineEdit_TopMargin->text().toInt();
            userconfig.SideToSide = ui->lineEdit_LeftMargin->text().toInt();
            userconfig.ImageDensity = ui->lineEdit_ImageDensity->text().toInt();
            userconfig.LowHumidityMode = (isLowHumidity ? 1 : 0);
//            userconfig.PlateControlMode = (isConrtolM ? 0 : 2);
//            userconfig.PrimaryCoolingMode = (isCoolingM ? 1 : 0);
            QVariant data;
            data.setValue<cmdst_userconfig>(userconfig);
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_Set_UserConfig,data);
            gUInterface->emitEnableCycleAnimation(true);
        //}
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}

/**************
 *to set the power save time
*****************/
void SettingsStackedWidget::on_btApply_Timeout_clicked()
{
    if(!isLogn )//&& CMD_STATUS_COMPLETE == device->get_cmdStatus())
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(isLogn)
    {
        if(ui->radio_default20->isChecked()){
            psavetm = 254;
        }else if(ui->radio_never_powersave->isChecked()){
            psavetm = 255;
        }else{
            psavetm = ui->lineEdit_timeout->text().toInt();
        }
        //if(CMD_STATUS_COMPLETE == device->get_cmdStatus())  {
            QVariant data;
            data.setValue<cmdst_PSave_time>(psavetm);
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_PSaveTime_Set,data);
            gUInterface->emitEnableCycleAnimation(true);
        //}
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}

void SettingsStackedWidget::on_lineEdit_TopMargin_textEdited(const QString &arg1)
{
    int val = arg1.toInt();
    if(val>2 || val<-2 || arg1 == ""|| arg1 =="-")
    {
//        ui->label_setting_error->setGeometry(47,ui->lineEdit_TopMargin->y()+20,201,35);
//        ui->label_setting_error->setText(tr("The valid range is from -2 to 2, please confirm and enter again"));
//        ui->label_setting_error->show();
        ui->label_TopMargin_error->show();
        ui->lineEdit_TopMargin->setStyleSheet("QLineEdit{border:2px solid red;}");
        ui->btTopAdd->setDisabled(true);
        ui->btTopReduce->setDisabled(true);
        ui->btApply_userconfig->setDisabled(true);
    }
    else
    {
        if(arg1 =="-0"||arg1 =="00")
            ui->lineEdit_TopMargin->setText("0");
        else if(arg1 =="01")
            ui->lineEdit_TopMargin->setText("1");
        else if(arg1 =="02")
            ui->lineEdit_TopMargin->setText("2");

        ui->lineEdit_TopMargin->setStyleSheet("QLineEdit{border:transparent;}");
        ui->label_TopMargin_error->hide();
        //ui->label_setting_error->hide();
        ui->btTopAdd->setEnabled(true);
        ui->btTopReduce->setEnabled(true);
        ui->btApply_userconfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_lineEdit_LeftMargin_textEdited(const QString &arg1)
{
    int val = arg1.toInt();
    if(val>6 || val<-6 || arg1 == ""|| arg1 =="-")
    {
//        ui->label_setting_error->setGeometry(47,ui->lineEdit_LeftMargin->y()+20,201,35);
//        ui->label_setting_error->setText(tr("The valid range is from -6 to 6, please confirm and enter again"));
//        ui->label_setting_error->show();
        ui->label_LeftMargin_error->show();
        ui->lineEdit_LeftMargin->setStyleSheet("QLineEdit{border:2px solid red;}");
        ui->btLeftAdd->setDisabled(true);
        ui->btLeftReduce->setDisabled(true);
        ui->btApply_userconfig->setDisabled(true);
    }
    else
    {
        if(arg1 =="-0"||arg1 =="00")
            ui->lineEdit_LeftMargin->setText("0");
        else if(arg1 =="01")
            ui->lineEdit_LeftMargin->setText("1");
        else if(arg1 =="02")
            ui->lineEdit_LeftMargin->setText("2");
        else if(arg1 =="03")
            ui->lineEdit_LeftMargin->setText("3");
        else if(arg1 =="04")
            ui->lineEdit_LeftMargin->setText("4");
        else if(arg1 =="05")
            ui->lineEdit_LeftMargin->setText("5");
        else if(arg1 =="06")
            ui->lineEdit_LeftMargin->setText("6");

        ui->lineEdit_LeftMargin->setStyleSheet("QLineEdit{border:transparent;}");
        ui->label_LeftMargin_error->hide();
        //ui->label_setting_error->hide();
        ui->btLeftAdd->setEnabled(true);
        ui->btLeftReduce->setEnabled(true);
        ui->btApply_userconfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_lineEdit_ImageDensity_textEdited(const QString &arg1)
{
    int val = arg1.toInt();
    if(val>3 || val<-3 || arg1 == ""|| arg1 =="-")
    {
//        ui->label_setting_error->setGeometry(47,ui->lineEdit_ImageDensity->y()+20,201,35);
//        ui->label_setting_error->setText(tr("The valid range is from -3 to 3, please confirm and enter again"));
//        ui->label_setting_error->show();
        ui->label_ImageDensity_error->show();
        ui->lineEdit_ImageDensity->setStyleSheet("QLineEdit{border:2px solid red;}");
        ui->btDensityAdd->setDisabled(true);
        ui->btDensityReduce->setDisabled(true);
        ui->btApply_userconfig->setDisabled(true);
    }
    else
    {
        if(arg1 =="-0"||arg1 =="00")
            ui->lineEdit_ImageDensity->setText("0");
        else if(arg1 =="01")
            ui->lineEdit_ImageDensity->setText("1");
        else if(arg1 =="02")
            ui->lineEdit_ImageDensity->setText("2");
        else if(arg1 =="03")
            ui->lineEdit_ImageDensity->setText("3");

        ui->lineEdit_ImageDensity->setStyleSheet("QLineEdit{border:transparent;}");
        ui->label_ImageDensity_error->hide();
        //ui->label_setting_error->hide();
        ui->btDensityAdd->setEnabled(true);
        ui->btDensityReduce->setEnabled(true);
        ui->btApply_userconfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_btDensityAdd_clicked()
{
    int val = ui->lineEdit_ImageDensity->text().toInt();
    if(val<3)
    {
        val++;
        ui->lineEdit_ImageDensity->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btDensityReduce_clicked()
{
    int val = ui->lineEdit_ImageDensity->text().toInt();
    if(val>-3)
    {
        val--;
        ui->lineEdit_ImageDensity->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btDrumReset_clicked()
{
    PromptDialog *pDialog = new PromptDialog(this);
    pDialog->setDialogMsg(tr("ResStr_This_operation_will_reset_the_drum"));
    pDialog->setDialogMsgAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    if (pDialog->exec() == QDialog::Accepted)
    {
        if(!isLogn)// && CMD_STATUS_COMPLETE == device->get_cmdStatus())
        {
            AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
            dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
            dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
            dlg->exec();
        }
        if(isLogn)
        {
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_DrumReset);
            gUInterface->emitEnableCycleAnimation(true);
        }
        else
        {
            QString deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
        }
    }
}

void SettingsStackedWidget::on_btErrorClear_clicked()
{
    if(!isLogn)// && CMD_STATUS_COMPLETE == device->get_cmdStatus())
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(isLogn)
    {
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_FusingScReset);
        gUInterface->emitEnableCycleAnimation(true);
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}

void SettingsStackedWidget::on_btLeftAdd_clicked()
{
    int val = ui->lineEdit_LeftMargin->text().toInt();
    if(val<6)
    {
        val++;
        ui->lineEdit_LeftMargin->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btLeftReduce_clicked()
{
    int val = ui->lineEdit_LeftMargin->text().toInt();
    if(val>-6)
    {
        val--;
        ui->lineEdit_LeftMargin->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btLowHumidity_clicked()
{
    isLowHumidity = !isLowHumidity;
    if(isLowHumidity)
        ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
    else
        ui->btLowHumidity->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
}

void SettingsStackedWidget::on_btTonerReset_clicked()
{
    PromptDialog *pDialog = new PromptDialog(this);
    pDialog->setDialogMsg(tr("ResStr_This_operation_will_reset_the_toner"));
    pDialog->setDialogMsgAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    if (pDialog->exec() == QDialog::Accepted)
    {
        if(!isLogn)// && CMD_STATUS_COMPLETE == device->get_cmdStatus())
        {
            AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
            dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
            dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
            dlg->exec();
        }
        if(isLogn)
        {
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_TonerReset);
            gUInterface->emitEnableCycleAnimation(true);
        }
        else
        {
            QString deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
        }
    }
}

void SettingsStackedWidget::on_btTopAdd_clicked()
{
    int val = ui->lineEdit_TopMargin->text().toInt();
    if(val<2)
    {
        val++;
        ui->lineEdit_TopMargin->setText(QString::number(val));
    }
}

void SettingsStackedWidget::on_btTopReduce_clicked()
{
    int val = ui->lineEdit_TopMargin->text().toInt();
    if(val>-2)
    {
        val--;
        ui->lineEdit_TopMargin->setText(QString::number(val));
    }
}

/**************
 *to set the new password
*****************/
void SettingsStackedWidget::on_btApply_clicked()
{
    if(ui->lineEdit_newPassWord->text().isEmpty())
    {
        SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_The_new_password_can_not_be_empty_"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
    }
    else if(ui->lineEdit_confirm->text() != ui->lineEdit_newPassWord->text())
    {
        SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_The_passwords_you_entered__are_different__please_try_again_"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
    }
    else
    {
        if(!isLogn)// && CMD_STATUS_COMPLETE == device->get_cmdStatus())
        {
            AuthenticationDlg *dlg = new AuthenticationDlg(this, &isLogn);
            dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
            dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
            dlg->exec();
        }
        if(isLogn)
        {
            cmdst_passwd passwd;
            QString text = ui->lineEdit_newPassWord->text();
            memcpy(passwd.passwd,text.left(32).toLatin1(),32);
            QVariant data;
            data.setValue<cmdst_passwd>(passwd);
            gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PASSWD_set,data);
            gUInterface->emitEnableCycleAnimation(true);
        }
        else
        {
            QString deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
        }
    }
}

void SettingsStackedWidget::on_lineEdit_IPAddressv4_textEdited(const QString &arg1)
{
    QRegExp rx2("^([1]?\\d\\d?|2[0-1]\\d|22[0-3])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
    if( !rx2.exactMatch(arg1) || (arg1.length() >= 3 && arg1.left(3) == "127"))
    {
         ui->label_ipaddr_error->show();
         ui->lineEdit_IPAddressv4->setStyleSheet("QLineEdit{\
                                                 border:2px solid red;\
                                                 padding: 0 8px;\
                                                 border-radius:12px;}");
        ui->btApply_IPConfig->setDisabled(true);
    }
    else
    {
        ui->label_ipaddr_error->hide();
        ui->lineEdit_IPAddressv4->setStyleSheet("QLineEdit{\
                                                padding: 0 8px;\
                                                border:2px solid rgb(170, 170, 170);\
                                                border-radius:12px;}");
        ui->btApply_IPConfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_lineEdit_Gatewayv4_textEdited(const QString &arg1)
{
    QRegExp rx2("^([1]?\\d\\d?|2[0-1]\\d|22[0-3])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
    if( !rx2.exactMatch(arg1) || (arg1.length() >= 3 && arg1.left(3) == "127"))
    {
         ui->label_gateway_error->show();
         ui->lineEdit_Gatewayv4->setStyleSheet("QLineEdit{\
                                               padding: 0 8px;\
                                               border:2px solid red;\
                                               border-radius:12px;}");
        ui->btApply_IPConfig->setDisabled(true);
    }
    else
    {
        ui->label_gateway_error->hide();
        ui->lineEdit_Gatewayv4->setStyleSheet("QLineEdit{\
                                              padding: 0 8px;\
                                              border:2px solid rgb(170, 170, 170);\
                                              border-radius:12px;}");
        ui->btApply_IPConfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_lineEdit_Submaskv4_textEdited(const QString &arg1)
{
    QRegExp rx2("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.0$");
    if( !rx2.exactMatch(arg1) ||(arg1.length() >= 3 && arg1.left(3) != "255"))
    {
         ui->label_sumask_error->show();
         ui->lineEdit_Submaskv4->setStyleSheet("QLineEdit{\
                                               padding: 0 8px;\
                                               border:2px solid red;\
                                               border-radius:12px;}");
        ui->btApply_IPConfig->setDisabled(true);
    }
    else
    {
        ui->label_sumask_error->hide();
        ui->lineEdit_Submaskv4->setStyleSheet("QLineEdit{\
                                              padding: 0 8px;\
                                              border:2px solid rgb(170, 170, 170);\
                                              border-radius:12px;}");
        ui->btApply_IPConfig->setEnabled(true);
    }
}

void SettingsStackedWidget::on_lineEdit_server_textEdited(const QString &arg1)
{
    QRegExp rx2("^([1]?\\d\\d?|2[0-1]\\d|22[0-3])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
    if( !rx2.exactMatch(arg1) || (arg1.length() >= 3 && arg1.left(3) == "127"))
    {
         ui->label_server_error->show();
         ui->lineEdit_server->setStyleSheet("QLineEdit{\
                                               padding: 0 8px;\
                                               border:2px solid red;\
                                               border-radius:12px;}");
        ui->btApply_IPConfig->setDisabled(true);
    }
    else
    {
        ui->label_server_error->hide();
        ui->lineEdit_server->setStyleSheet("QLineEdit{\
                                              padding: 0 8px;\
                                              border:2px solid rgb(170, 170, 170);\
                                              border-radius:12px;}");
        ui->btApply_IPConfig->setEnabled(true);
    }
}

/****************
 * this event filter is work for the lineEdit where need to enter value
 * it get the focus lost event of those lineEdit.
**************************/
bool SettingsStackedWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit_IPAddressv4)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_ipaddr_error->isHidden()))
            {
                ui->lineEdit_IPAddressv4->setText("0.0.0.0");
                ui->label_ipaddr_error->hide();
                ui->lineEdit_IPAddressv4->setStyleSheet("QLineEdit{\
                                                        padding: 0 8px;\
                                                      border:2px solid rgb(170, 170, 170);\
                                                      border-radius:12px;}");
                ui->btApply_IPConfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_Gatewayv4)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_gateway_error->isHidden()))
            {
                ui->lineEdit_Gatewayv4->setText("0.0.0.0");
                ui->label_gateway_error->hide();
                ui->lineEdit_Gatewayv4->setStyleSheet("QLineEdit{\
                                                      padding: 0 8px;\
                                                      border:2px solid rgb(170, 170, 170);\
                                                      border-radius:12px;}");
                ui->btApply_IPConfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_Submaskv4)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_sumask_error->isHidden()))
            {
                ui->lineEdit_Submaskv4->setText("0.0.0.0");
                ui->label_sumask_error->hide();
                ui->lineEdit_Submaskv4->setStyleSheet("QLineEdit{\
                                                      padding: 0 8px;\
                                                      border:2px solid rgb(170, 170, 170);\
                                                      border-radius:12px;}");
                ui->btApply_IPConfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_server)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_server_error->isHidden()))
            {
                ui->lineEdit_server->setText("0.0.0.0");
                ui->label_server_error->hide();
                ui->lineEdit_server->setStyleSheet("QLineEdit{\
                                                   padding: 0 8px;\
                                                  border:2px solid rgb(170, 170, 170);\
                                                  border-radius:12px;}");
                ui->btApply_IPConfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_timeout)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_timeout_error->isHidden()))
            {
                ui->lineEdit_timeout->setText("0");
                ui->label_timeout_error->hide();
                ui->lineEdit_timeout->setStyleSheet("QLineEdit{border:transparent;}");
                ui->btTimeAdd->setEnabled(true);
                ui->btTimeReduce->setEnabled(true);
                ui->btApply_Timeout->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_TopMargin)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_TopMargin_error->isHidden()) || ui->lineEdit_TopMargin->text().isEmpty())
            {
                ui->lineEdit_TopMargin->setText("0");
                ui->lineEdit_TopMargin->setStyleSheet("QLineEdit{border:transparent;}");
                ui->label_TopMargin_error->hide();
                ui->btTopAdd->setEnabled(true);
                ui->btTopReduce->setEnabled(true);
                ui->btApply_userconfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_LeftMargin)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_LeftMargin_error->isHidden()) || ui->lineEdit_LeftMargin->text().isEmpty())
            {
                ui->lineEdit_LeftMargin->setText("0");
                ui->lineEdit_LeftMargin->setStyleSheet("QLineEdit{border:transparent;}");
                ui->label_LeftMargin_error->hide();
                ui->btLeftAdd->setEnabled(true);
                ui->btLeftReduce->setEnabled(true);
                ui->btApply_userconfig->setEnabled(true);
            }
        }
    }
    if(watched == ui->lineEdit_ImageDensity)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(!(ui->label_ImageDensity_error->isHidden()) || ui->lineEdit_ImageDensity->text().isEmpty())
            {
                ui->lineEdit_ImageDensity->setText("0");
                ui->lineEdit_ImageDensity->setStyleSheet("QLineEdit{border:transparent;}");
                ui->label_ImageDensity_error->hide();
                ui->btDensityAdd->setEnabled(true);
                ui->btDensityReduce->setEnabled(true);
                ui->btApply_userconfig->setEnabled(true);
//                qDebug()<<"evenfilter here for imageDensity";
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

/******
 *when the user leave this page, it would need to confirm when setting values next time;
 *this function to reaction hide event of this page;
***********/
void SettingsStackedWidget::hideEvent(QHideEvent *e)
{
    isLogn = false;
    QWidget::hideEvent(e);
}

//void SettingsStackedWidget::setEnabled(bool enabled)
//{
//    if(enabled == false)
//    {
//        isLogn = false;
//    }
//    QWidget::setEnabled(enabled);
//}


void SettingsStackedWidget::onTimer1Timeout_Add()
{
    if(timercounter > 9)
    {
        switch(pressBtn)
        {
        case TimeoutSetting:
            on_btTimeAdd_clicked();
            break;
        case TopMargin:
            on_btTopAdd_clicked();
            break;
        case LeftMargin:
            on_btLeftAdd_clicked();
            break;
        case ImageDensity:
            on_btDensityAdd_clicked();
            break;
        default:
            break;
        }
    }
    timercounter++;
}

void SettingsStackedWidget::onTimer1Timeout_Reduce()
{
    if(timercounter > 9)
    {
        switch(pressBtn)
        {
        case TimeoutSetting:
            on_btTimeReduce_clicked();
            break;
        case TopMargin:
            on_btTopReduce_clicked();
            break;
        case LeftMargin:
            on_btLeftReduce_clicked();
            break;
        case ImageDensity:
            on_btDensityReduce_clicked();
            break;
        default:
            break;
        }
    }
    timercounter++;
}

void SettingsStackedWidget::on_btTimeReduce_pressed()
{
    pressBtn = TimeoutSetting;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btTimeReduce_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
}

void SettingsStackedWidget::on_btTimeAdd_pressed()
{
    pressBtn = TimeoutSetting;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btTimeAdd_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
}

void SettingsStackedWidget::on_btTopReduce_pressed()
{
    pressBtn = TopMargin;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btTopReduce_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
}

void SettingsStackedWidget::on_btTopAdd_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
}

void SettingsStackedWidget::on_btTopAdd_pressed()
{
    pressBtn = TopMargin;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btLeftReduce_pressed()
{
    pressBtn = LeftMargin;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btLeftReduce_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
}

void SettingsStackedWidget::on_btLeftAdd_pressed()
{
    pressBtn = LeftMargin;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btLeftAdd_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
}

void SettingsStackedWidget::on_btDensityReduce_pressed()
{
    pressBtn = ImageDensity;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btDensityReduce_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Reduce()));
}

void SettingsStackedWidget::on_btDensityAdd_pressed()
{
    pressBtn = ImageDensity;
    timercounter = 0;
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
    timer1->start(60);
}

void SettingsStackedWidget::on_btDensityAdd_released()
{
    timer1->stop();
    disconnect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1Timeout_Add()));
}
