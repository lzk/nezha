#include "wifisettingcell.h"
#include "ui_wifisettingcell.h"
#include "settingwarming.h"
#include "authenticationdlg.h"
#include "uinterface.h"
#include <qdebug.h>

WiFiSettingCell::WiFiSettingCell(QWidget *parent, APInfo *info, bool *_islogin, bool isconnected) :
    QStackedWidget(parent),
    ui(new Ui::WiFiSettingCell),
    isConnected(isconnected),
    m_isLogin(false)
{
    ui->setupUi(this);
    this->setCurrentIndex(0);
    if(info != NULL)
    {
        apInfo = *info;
        switch(apInfo.encryType)
        {
        case NO_Securty:
            apInfo.APStatus = tr("ResStr_No_Security"); break;
        case WPA_PSK_TKIP:
            apInfo.APStatus = tr("ResStr_Protected_by_WEP"); break;
        case WPA2_PSK_AES:
            apInfo.APStatus = tr("ResStr_Protected_by_WPA2"); break;
        case Mixed_Mode_PSK:
            apInfo.APStatus = tr("ResStr_Protected_by_Mixed_Mode_PSK"); break;
        default:    break;
        }
    }       //UINT8 encryption ; // 0:No Security 1:WEP 64/128 bit 2.WPA-PSK-TKIP  3. WPA2-PSK-AES 4.Mixed Mode PSK
    else
    {
        apInfo.SSID = tr("SWQA");
        apInfo.APStatus = tr("ResStr_No_Security");
        apInfo.encryType = NO_Securty;
        apInfo.Password = "";
        apInfo.wepKeyID = 0;
    }

    isDisplayPW = false;
    ui->checkBox_visiable->setChecked(isDisplayPW);
    isConnected = isconnected;
    initStatus();
    if(NULL != _islogin)
    {
        islogin = _islogin;
    }
    else
    {
        islogin = &m_isLogin;
    }
    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));
}

WiFiSettingCell::~WiFiSettingCell()
{
    delete ui;
}

void WiFiSettingCell::on_btCancel_clicked()
{
    this->setCurrentIndex(0);
    this->resize(QSize(220, 51));
    this->setMinimumHeight(51);
    emit SizeChanged( QSize(220, 154), QSize(211, 51));
}

//
void WiFiSettingCell::on_pushButton_clicked()
{
    this->setCurrentIndex(1);
    this->resize(QSize(220, 154));
    this->setMinimumHeight(154);
    emit SizeChanged( QSize(220, 51), QSize(211, 154));
}

void WiFiSettingCell::on_pushButton_2_clicked()
{
    on_btCancel_clicked();
}

void WiFiSettingCell::on_checkBox_visiable_clicked()
{
    if(ui->checkBox_visiable->isChecked())
    {
        isDisplayPW = true;
        ui->lineEdit_Password->setEchoMode(QLineEdit::Normal);
        QRegExp regexp2("^[\\x0020-\\x007e]{1,64}$");
        QValidator *validator2 = new QRegExpValidator(regexp2, this);
        ui->lineEdit_Password->setValidator(validator2);
    }
    else
    {
        isDisplayPW = false;
        ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
        QRegExp regexp2("^[\\x0020-\\x007e]{1,64}$");
        QValidator *validator2 = new QRegExpValidator(regexp2, this);
        ui->lineEdit_Password->setValidator(validator2);
    }
}

void WiFiSettingCell::initStatus()
{
    QRegExp regexp("^[\\x0020-\\x007e]{1,64}$");
    QValidator *validator = new QRegExpValidator(regexp, this);
    ui->lineEdit_Password->setValidator(validator);

    QFont ft = ui->label_SSID1->font();
    QFontMetrics fm(ft);
    QString ssid1 = fm.elidedText(apInfo.SSID, Qt::ElideRight, ui->label_SSID1->width());
    QString ssid2 = fm.elidedText(apInfo.SSID, Qt::ElideRight, ui->label_SSID2->width());
    ui->label_SSID1->setText(ssid1);
    ui->label_SSID2->setText(ssid2);
    ui->label_APStatus->setText(apInfo.APStatus);
    ui->label_APStatus_2->setText(apInfo.APStatus);
    ui->lineEdit_Password->setText(apInfo.Password);

    if(apInfo.encryType == NO_Securty)
    {
        ui->lineEdit_Password->setDisabled(true);
    }

    if(isConnected)
    {
        ui->label_APStatus->setText(tr("ResStr_Connected"));
        ui->label_APStatus_2->setText(tr("ResStr_Connected"));
        ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Connect.png);}");
        ui->label_line1->setStyleSheet("QLabel {border: 1px solid  rgb(105, 185, 18);}");
        ui->label_line2->setStyleSheet("QLabel {border: 1px solid  rgb(105, 185, 18);}");
        apInfo.APStatus.clear();
        apInfo.APStatus = tr("ResStr_Connected");
        ui->btConnect->setDisabled(true);
    }
}

void WiFiSettingCell::on_btConnect_clicked()
{
    int len = ui->lineEdit_Password->text().length();
    if(len<8 && apInfo.encryType != NO_Securty)
    {
        SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_Msg_3"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint );
        msgWarm->exec();
    }
    else
    {
        if(!(*islogin) )
        {
            AuthenticationDlg *dlg = new AuthenticationDlg(this, islogin);
            dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint );
            dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
            dlg->exec();
        }
        if(*islogin)
        {
            apInfo.Password = ui->lineEdit_Password->text();
            tryConnect(apInfo);
        }
        else
        {
            QString deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
        }
    }
    //do something here; it is a short time that you can not saw the busyRefresh Label;
}

void WiFiSettingCell::changeStatus()
{
    switch(apInfo.encryType)
    {
    case NO_Securty:
        apInfo.APStatus = tr("ResStr_No_Security"); break;
    case WEP:
        apInfo.APStatus = tr("ResStr_Protected_by_WEP"); break;
    case WPA2_PSK_AES:
        apInfo.APStatus = tr("ResStr_Protected_by_WPA2"); break;
    case Mixed_Mode_PSK:
        apInfo.APStatus = tr("ResStr_Protected_by_Mixed_Mode_PSK"); break;
//    case WPA_PSK_TKIP:
//        apInfo.APStatus = tr("Protected by WPA_PSK_TKIP"); break;

    default:
        apInfo.APStatus = tr("Protected by ");   break;
    }
    isConnected = false;
    initStatus();
    ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Enable.png);}");
    ui->label_line1->setStyleSheet("QLabel { border:1px solid  rgb(181, 181, 181);}");
    ui->label_line2->setStyleSheet("QLabel { border:1px solid  rgb(181, 181, 181);}");
    ui->btConnect->setEnabled(true);
}


APInfo WiFiSettingCell::getAPInfo()
{
    return apInfo;
}

void WiFiSettingCell::tryConnect(APInfo ap)
{
    cmdst_wifi_get wifi_para;
    QString ssid;
    QString passwd;
    char tmp_char[65];
    if(ap.encryType == NO_Securty)
    {
        ssid = ap.SSID;
        passwd = ap.Password;
        wifi_para.encryption = ap.encryType;
        wifi_para.wepKeyId = ap.wepKeyID;
       //ssid
        memset(tmp_char ,0 ,65);
        strcpy(tmp_char ,ssid.toUtf8().constData());
        memcpy(wifi_para.ssid ,tmp_char ,32);
//        if(strlen(ssid.toUtf8()) >= 32)
//            memcpy(wifi_para.ssid ,ssid.toUtf8() ,32);
//        else{
//            memset(wifi_para.ssid ,0 ,32);
//            strcpy(wifi_para.ssid ,ssid.toUtf8());
//        }

        //passwd
        memset(tmp_char ,0 ,65);
        strcpy(tmp_char ,passwd.toUtf8().constData());
        memcpy(wifi_para.pwd ,tmp_char ,64);
//        if(strlen(passwd.toLatin1()) >= 64)
//            memcpy(wifi_para.pwd ,passwd.toLatin1() ,64);
//        else{
//            memset(wifi_para.pwd ,0 ,64);
//            strcpy(wifi_para.pwd ,passwd.toLatin1());
//        }

        //encryption type
       // C_LOG("wifi apply: wifi_para.encryption:%d" ,wifi_para.encryption);

        //wep key id
        if(!(wifi_para.wifiEnable & 1)){
            //_Q_LOG("wifi apply:wifi have not been enable ,enable it");
            wifi_para.wifiEnable &= ~1;
            wifi_para.wifiEnable |= 1;//bit 0
        }

//        connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));

        QVariant value;
        value.setValue(wifi_para);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_apply,value);
        emit doingConnect(this);
    }
    else //if(apInfo.encryType == WPA2_PSK_AES || apInfo.encryType == Mixed_Mode_PSK)
    {
        ssid = ap.SSID;
        passwd = ap.Password;
        //ssid
        memset(tmp_char ,0 ,65);
        strcpy(tmp_char ,ssid.toUtf8().constData());
        memcpy(wifi_para.ssid ,tmp_char ,32);
//         if(strlen(ssid.toUtf8()) >= 32)
//             memcpy(wifi_para.ssid ,ssid.toUtf8() ,32);
//         else{
//             memset(wifi_para.ssid ,0 ,32);
//             strcpy(wifi_para.ssid ,ssid.toUtf8());
//         }

         //passwd
         memset(tmp_char ,0 ,65);
         strcpy(tmp_char ,passwd.toLatin1().constData());
         memcpy(wifi_para.pwd ,tmp_char ,64);
//         if(strlen(passwd.toLatin1()) >= 64)
//             memcpy(wifi_para.pwd ,passwd.toLatin1() ,64);
//         else{
//             memset(wifi_para.pwd ,0 ,64);
//             strcpy(wifi_para.pwd ,passwd.toLatin1());
//         }

        //encryption type
        wifi_para.encryption = ap.encryType;

        //C_LOG("wifi apply: wifi_para.encryption:%d" ,wifi_para.encryption);
        wifi_para.wepKeyId = 0;

        wifi_para.wifiEnable = 1;

//        connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));

        QVariant value;
        value.setValue(wifi_para);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_apply,value);
        emit doingConnect(this);
    }
}

void WiFiSettingCell::cmdResult(int cmd,int result ,QVariant data)
{
    if(UIConfig::LS_CMD_WIFI_apply == cmd)
    {
        cmdst_wifi_get wifi_para = data.value<cmdst_wifi_get>();
        char str[33];
        memset(str ,0 ,33);
        memcpy(str ,wifi_para.ssid ,32);
        if(apInfo.SSID.compare(str)){
            switch(apInfo.encryType)
            {
            case NO_Securty:
                apInfo.APStatus = tr("ResStr_No_Security"); break;
            case WEP:
                apInfo.APStatus = tr("ResStr_Protected_by_WEP"); break;
            case WPA2_PSK_AES:
                apInfo.APStatus = tr("ResStr_Protected_by_WPA2"); break;
            case Mixed_Mode_PSK:
                apInfo.APStatus = tr("ResStr_Protected_by_Mixed_Mode_PSK"); break;
            default:    break;
            }
            ui->label_APStatus->setText(apInfo.APStatus);
            ui->label_APStatus_2->setText(apInfo.APStatus);
            ui->btConnect->setEnabled(true);
            ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Connect_Disable.png);}");
            return;
        }
        QString deviceMsg;
        if(!result)
        {
            ui->label_APStatus->setText(tr("ResStr_Connected"));
            ui->label_APStatus_2->setText(tr("ResStr_Connected"));
            ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Connect.png);}");
            ui->label_line1->setStyleSheet("QLabel {border: 1px solid  rgb(105, 185, 18);}");
            ui->label_line2->setStyleSheet("QLabel {border: 1px solid  rgb(105, 185, 18);}");
            apInfo.APStatus.clear();
            apInfo.APStatus = tr("ResStr_Connected");
            deviceMsg = tr("ResStr_Setting_Successfully_");
            ui->btConnect->setDisabled(true);
            emit connectSuc(this, true);
        }
        else
        {
            deviceMsg = tr("ResStr_Setting_Fail");
            ui->btConnect->setEnabled(true);
            emit connectSuc(this, false);
        }
        gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
//        disconnect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));
    }
}
void WiFiSettingCell::setWifiEnabled(bool enabled)
{
    if(enabled)
    {
        ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Connect.png);}");
    }
    else
    {
       ui->label_StatusIcon->setStyleSheet("QLabel{border-image: url(:/Images/Signal_Connect_Disable.png);}");
    }
}
