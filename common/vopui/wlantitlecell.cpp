#include "wlantitlecell.h"
#include "ui_wlantitlecell.h"
#include <QPropertyAnimation>
#include <QPainter>
#include <QDebug>
#include <QWizard>
#include <QMatrix>
#include <QRegExp>
#include <QValidator>
#include "settingwarming.h"
#include "authenticationdlg.h"
#include <QMessageBox>
#include <QListView>
#include <QRegExpValidator>


#define DEFWIDTH 220
#define DEFTITELHIGHT 65
WlanTitleCell::WlanTitleCell(QWidget *parent, QScrollBar *_scrollBar,  bool wlanON, bool *_islogin) :
    QStackedWidget(parent),
    ui(new Ui::WlanTitleCell),
    m_isLogin(false)
{
    ui->setupUi(this);

    pScrollBar = _scrollBar;
    oldScrollValue = 0;

    QListView *listView = new QListView(ui->combox_encryption);
    listView->setStyleSheet("QListView{border-color:black;border-width:2px;border-radius:0px;}");
    listView->setStyleSheet("QListView::item:!selected{background-color:white;color:black;}");
    listView->setStyleSheet("QListView::item:selected:!active{background-color:gray;color:black;}");
    listView->setStyleSheet("QListView::item:selected:active{background-color:gray;color:white;}");
    ui->combox_encryption->setView(listView);

    timer1 = new QTimer(this);
    cycleCount = 0;
    currentAPID = 0;
    currentAp = new APInfo;
    isManual = false;
    isWlanOn = wlanON;
    ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
    ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
    ui->btManualWiFi->hide();
    ui->btFlesh->hide();
    ui->label_line->hide();
    ui->label_network->hide();

    this->setCurrentIndex(0);
    this->resize(QSize(220,60));
    widget = new QWidget(ui->page1);
    widget->setGeometry(QRect(0,73,DEFWIDTH,10));
    currentSize.setWidth(DEFWIDTH);
    currentSize.setHeight(10);
    pageLayout.setSpacing(1);
    pageLayout.setMargin(1);

    wifiWepCell = new WiFiSettingWEPCell();
    wifiCell = new WiFiSettingCell();


//    device->emit_cmd_plus(CMD_WIFI_refresh_plus);

//    cycle = new BusyRefreshLabel(parent);
//    cycle->setGeometry(QRect(90,130,50,50));

    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimeout()));

    QRegExp regexp1("^[\\x0020-\\x007e]{1,32}$");
    QValidator *validator1 = new QRegExpValidator(regexp1, this);
    ui->lineEdit_SSID->setValidator(validator1);

    ui->combox_encryption->setCurrentIndex(2);
    ui->label_keyid->hide();
    ui->btKey1->hide();
    ui->btKey2->hide();
    ui->btKey3->hide();
    ui->btKey4->hide();

    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));
//    qRegisterMetaType<APInfo>("APInfo");
#ifdef DEBUG
    m_isLogin = true;
    islogin = &m_isLogin;
#else
    if(NULL != _islogin)
    {
//        qDebug()<<"1";
        islogin = _islogin;
    }
    else
    {
//        qDebug()<<"2";
        islogin = &m_isLogin;
    }
#endif
    isWitch = false;
    is_wifi_now_on = false;
}

WlanTitleCell::~WlanTitleCell()
{
    delete timer1;
    delete currentAp;
    delete ui;
    delete wifiWepCell;
    delete wifiCell;
}

void WlanTitleCell::cmdResult(int cmd,int result ,QVariant data)
{
    switch(cmd)
    {
    case UIConfig::CMD_WIFI_refresh_plus:
//        qDebug()<<"CMD_WIFI_refresh_plus"<<result;
        if(!result){
            struct_wifi_refresh_info wifi_refresh_info;
            wifi_refresh_info = data.value<struct_wifi_refresh_info>();

            orin_wifi_para = wifi_refresh_info.wifi_para;
            wifi_para = wifi_refresh_info.wifi_para;
            cmdst_aplist_get aplist = wifi_refresh_info.wifi_aplist;

#ifndef DEBUG
            initCell(wifi_para,aplist);
#endif
            if(wifi_refresh_info.wifi_status)
            {
                is_wifi_now_on = true;
            }
        }
        else
        {
            isWlanOn = false;
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        }
        gUInterface->emitEnableCycleAnimation(false);

        break;
    case UIConfig::LS_CMD_WIFI_apply:
    {
        QString deviceMsg;
        if(!result && isWitch)
        {
            isWlanOn = isWlanOn ? false : true;
//            qDebug()<<isWlanOn;
            if(isWlanOn)
            {
                ui->btManualWiFi->show();
                ui->btFlesh->show();
                ui->label_line->show();
                ui->label_network->show();
            }
            if((is_wifi_now_on == true && isWlanOn == false) || (is_wifi_now_on == false && isWlanOn == true) )
            {
                deviceMsg = tr("ResStr_Msg_1");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);

                SettingWarming *warming = new SettingWarming(this, tr("ResStr_Msg_1"), 1);
                warming->setWindowTitle(tr("ResStr_Prompt"));

                warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                warming->exec();
            }
            else
            {
                deviceMsg = tr("ResStr_Setting_Successfully_");
                gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
            }
        }
        else
        {
            if(isWlanOn)
            {
                ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
                ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");


            }
            else
            {
                ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
                ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            }
            deviceMsg = tr("ResStr_Setting_Fail");
            gUInterface->setDeviceMsgFrmUI(deviceMsg,result);
        }

        isWitch = false;
        gUInterface->emitEnableCycleAnimation(false);
    }
        break;    
    default: break;
    }

}

void WlanTitleCell::on_btWLANON1_clicked()
{
//    qDebug()<<"on_btWLANON1_clicked";
    isWitch = true;
    if(!*islogin)
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, islogin);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(*islogin)
    {
        if(!isWlanOn)
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
            ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
            wifi_para = orin_wifi_para;
            wifi_para.wifiEnable = 7;

        }
        else
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            this->setCurrentIndex(0);
            updateAP();

            ui->btManualWiFi->hide();
            ui->btFlesh->hide();
            ui->label_line->hide();
            ui->label_network->hide();

            wifi_para = orin_wifi_para;
            wifi_para.wifiEnable = 0;
        }

        QVariant value;
        value.setValue<cmdst_wifi_get>(wifi_para);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_apply,value);
        gUInterface->emitEnableCycleAnimation(true);
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}

void WlanTitleCell::on_btManualWiFi_clicked()
{
//    qDebug()<<"scroll"<<parentScroll->contentsRect();
//    qDebug()<<"currentSize.height():"<<currentSize.height()<<"this.height"<<this->size().height();
    ui->lineEdit_SSID->setText("");
    ui->lineEdit_Password->setText("");
    ui->combox_encryption->setCurrentIndex(2);
    ui->checkBox_visiable->setChecked(false);

//    qDebug()<<pScrollBar->value();

    this->resize(DEFWIDTH, 310);
    this->setMinimumHeight(310);
    this->setCurrentIndex(1);
}

void WlanTitleCell::on_btCancel_clicked()
{
//    emit SizeChanged( QSize(211, 310), QSize(211, 71));
    this->resize(DEFWIDTH, currentSize.height() + DEFTITELHIGHT);
    this->setMinimumHeight(currentSize.height() + DEFTITELHIGHT);
//    qDebug()<<"this.height"<<this->size().height();
//    parentScroll->scroll(0,0);
    this->setCurrentIndex(0);
    pScrollBar->setValue(0);
}

void WlanTitleCell::on_btWLANON2_clicked()
{
//    qDebug("on_btWLANON2_clicked");
    isWitch = true;
    if(!*islogin)
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, islogin);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(*islogin)
    {
        if(!isWlanOn)
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
            ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
            wifi_para = orin_wifi_para;
            wifi_para.wifiEnable = 7;

        }
        else
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
            this->setCurrentIndex(0);
            updateAP();

            ui->btManualWiFi->hide();
            ui->btFlesh->hide();
            ui->label_line->hide();
            ui->label_network->hide();

            wifi_para = orin_wifi_para;
            wifi_para.wifiEnable = 0;
        }
        QVariant value;
        value.setValue<cmdst_wifi_get>(wifi_para);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_WIFI_apply,value);
        gUInterface->emitEnableCycleAnimation(true);
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}

void WlanTitleCell::getSizeChanged(QSize oldSize, QSize newSize)
{
    currentSize = currentSize - oldSize + newSize;
    currentSize.setWidth(DEFWIDTH);
    widget->setMinimumHeight(currentSize.height());
    widget->resize(currentSize);
    widget->setLayout(&pageLayout);

    this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
    this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
    //bms7663
    if(newSize.height()>oldSize.height())
    {
        oldScrollValue = pScrollBar->value();
    }
    else
    {
        pScrollBar->setValue(oldScrollValue);
    }
}

void WlanTitleCell::addCell(QString ssid, EncrypType type, APInfo info, bool isConnected)
{
    WiFiSettingWEPCell *tmpWepCell;
    WiFiSettingCell *tmpCell;
    APInfo tmpinfo;
    tmpinfo = info;
    if(type == WEP)
    {
        tmpinfo.SSID = ssid;
        tmpinfo.encryType = type;

        currentSize.setHeight(51 + currentSize.height() + 1);
        this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
        this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
        widget->setMinimumHeight(currentSize.height());
        widget->resize(currentSize);

        tmpWepCell = new WiFiSettingWEPCell(widget, &tmpinfo, islogin, isConnected);
        aList.append(tmpWepCell->getAPInfo());
        apList.append(tmpWepCell);

        pageLayout.addWidget(apList.last());
        widget->setLayout(&pageLayout);

        connect(tmpWepCell, SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
        connect(tmpWepCell, SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
        connect(tmpWepCell, SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
//        connect(this, SIGNAL(tryToConnect(APInfo)), tmpWepCell, SLOT(tryConnect(APInfo)));
    }
    else
    {
        tmpinfo.SSID = ssid;
        tmpinfo.encryType = type;

        currentSize.setHeight(51 + currentSize.height() + 1);
        this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
        this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
        widget->setMinimumHeight(currentSize.height());
        widget->resize(currentSize);

        tmpCell = new WiFiSettingCell(widget, &tmpinfo, islogin, isConnected);
        aList.append(tmpCell->getAPInfo());
        apList.append(tmpCell);
        pageLayout.addWidget(apList.last());
        widget->setLayout(&pageLayout);

        connect((tmpCell), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
        connect((tmpCell), SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
        connect((tmpCell), SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
//        connect(this, SIGNAL(tryToConnect(APInfo)), tmpCell, SLOT(tryConnect(APInfo)));
    }
}

void WlanTitleCell::getConnectAction(QWidget* w)
{
//    connect(this, SIGNAL(statusChange()), qobject_cast<QWidget *>(apList.at(currentAPID)), SLOT(changeStatus()));
    gUInterface->emitEnableCycleAnimation(true);
}

void WlanTitleCell::getConnectResult(QWidget* w, bool s)
{
    if(ui->combox_encryption->currentIndex() == WEP)
    {
        disconnect((wifiWepCell), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
        disconnect((wifiWepCell), SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
        disconnect((wifiWepCell), SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
    }
    else
    {
        disconnect((wifiCell), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
        disconnect((wifiCell), SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
        disconnect((wifiCell), SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
    }
    gUInterface->emitEnableCycleAnimation(false);

//    if(s)
//    {
//        if(currentAPID != apList.indexOf(w))
//        {
//            emit statusChange();
//            disconnect(this, SIGNAL(statusChange()), qobject_cast<QWidget *>(apList.at(currentAPID)), SLOT(changeStatus()));
//            currentAPID =  apList.indexOf(w);
//        }
//    }
}

void WlanTitleCell::updateAP()
{
    while(!(apList.isEmpty()))
   {
       currentSize.setHeight( currentSize.height() - qobject_cast<QWidget *>(apList.last())->size().height() - 1);
       widget->resize(currentSize);
       widget->setMinimumHeight(currentSize.height());

       this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
       this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
       disconnect(qobject_cast<QWidget *>(apList.last()), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
       pageLayout.removeWidget(qobject_cast<QWidget *>(apList.last()));
       apList.last()->deleteLater();

       apList.removeLast();
       widget->setLayout(&pageLayout);
   }
   aList.clear();
   apList.clear();
}
#ifdef DEBUG
void WlanTitleCell::initCell(cmdst_wifi_getaa wifi_para, cmdst_aplist_getaa aplist)
#else
void WlanTitleCell::initCell(cmdst_wifi_get wifi_para, cmdst_aplist_get aplist)
#endif
{
//    qDebug()<<"initCell";

    while(!(apList.isEmpty()))
    {
       currentSize.setHeight( currentSize.height() - qobject_cast<QWidget *>(apList.last())->size().height() - 1);
       widget->resize(currentSize);
       widget->setMinimumHeight(currentSize.height());

       this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
       this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
       disconnect(qobject_cast<QWidget *>(apList.last()), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
       pageLayout.removeWidget(qobject_cast<QWidget *>(apList.last()));
       apList.last()->deleteLater();

       apList.removeLast();
       widget->setLayout(&pageLayout);
    }
    aList.clear();
    QString connected_ssid = wifi_para.ssid;
//    ui->lineEdit_SSID->setText(connected_ssid);
//    ui->lineEdit_Password->setText(QString(wifi_para.pwd));
//    ui->combox_encryption->setCurrentIndex(wifi_para.encryption);
    int tmp = wifi_para.wifiEnable & 1 ;
    isWlanOn = (0x01 == tmp ? true : false) ;
    if(isWlanOn)
    {
        ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
        ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");

        ui->btManualWiFi->show();
        ui->btFlesh->show();
        ui->label_line->show();
        ui->label_network->show();
        //search and add new ap
        int i = 0;
        QString ssid;
        for(i; i<10; i++)
        {
            ssid = aplist.aplist[i].ssid;
            currentAp->SSID = ssid;
            if(ssid.isEmpty()) break;
            EncrypType type = EncrypType(aplist.aplist[i].encryption);
            currentAp->encryType = type;
            if(ssid == connected_ssid.left(32) && i==0)
            {
                type = EncrypType(wifi_para.encryption);
                addCell(ssid.left(32), type, *currentAp, true);
                currentAPID = i;
            }
            else
            {
                currentAPID = 0;
                addCell(ssid.left(32), type, *currentAp, false);
            }
        }
    }
    else
    {
        ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->btWLANON2->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");

        ui->btManualWiFi->hide();
        ui->btFlesh->hide();
        ui->label_line->hide();
        ui->label_network->hide();
    }
}

void WlanTitleCell::onTimeout()
{
    cycleCount += 1;
    if(cycleCount>1)
    {
        timer1->stop();
        cycleCount = 0;
//        cycle->hide();
    }

}
void WlanTitleCell::setWifiEnabled(bool enabled)
{
    if(enabled)
    {
        if(isWlanOn)
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
        }
        else
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        }
        ui->label_wifiIcon->setStyleSheet("border-image: url(:/Images/Wireless_Active.png);");
        ui->btFlesh->setStyleSheet("border-image: url(:/Images/Status_RefreshEnable.tif);");
        wifiCell->setWifiEnabled(true);
        wifiWepCell->setWifiEnabled(true);

    }
    else
    {
        if(isWlanOn)
        {
             ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Disable.png);");
        }
        else
        {
            ui->btWLANON1->setStyleSheet("border-image: url(:/Images/CheckBox_Disable1.png);");
        }
        ui->label_wifiIcon->setStyleSheet("border-image: url(:/Images/Wireless.png);");
        ui->btFlesh->setStyleSheet("border-image: url(:/Images/Status_RefreshDisable.tif);");
        wifiCell->setWifiEnabled(false);
        wifiWepCell->setWifiEnabled(false);
    }
}
void WlanTitleCell::emptyWifiList()
{
    while(!(apList.isEmpty()))
   {
       currentSize.setHeight( currentSize.height() - qobject_cast<QWidget *>(apList.last())->size().height() - 1);
       widget->resize(currentSize);
       widget->setMinimumHeight(currentSize.height());

       this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
       this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
       disconnect(qobject_cast<QWidget *>(apList.last()), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
       pageLayout.removeWidget(qobject_cast<QWidget *>(apList.last()));
       apList.last()->deleteLater();

       apList.removeLast();
       widget->setLayout(&pageLayout);
   }
   aList.clear();
   setWifiEnabled(false);
}

void WlanTitleCell::on_btFlesh_clicked()
{
//    timer1->start(1000);
//    qDebug()<<"on_btFlesh_clicked";
    while(!(apList.isEmpty()))
   {
       currentSize.setHeight( currentSize.height() - qobject_cast<QWidget *>(apList.last())->size().height() - 1);
       widget->resize(currentSize);
       widget->setMinimumHeight(currentSize.height());

       this->setMinimumHeight(DEFTITELHIGHT + currentSize.height());
       this->resize(QSize(DEFWIDTH, DEFTITELHIGHT + currentSize.height()));
       disconnect(qobject_cast<QWidget *>(apList.last()), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
       pageLayout.removeWidget(qobject_cast<QWidget *>(apList.last()));
       apList.last()->deleteLater();

       apList.removeLast();
       widget->setLayout(&pageLayout);
   }
   aList.clear();
   setWifiEnabled(true);

   LOGLOG("cycleStartFromWT");
#ifndef DEBUG
    gUInterface->emitEnableCycleAnimation(true);
    gUInterface->setCurrentPrinterCmd(UIConfig::CMD_WIFI_refresh_plus);
#else
    cmdst_wifi_getaa wifiaa;
    wifiaa.channel = 11;
    wifiaa.encryption = 3;
    wifiaa.netType = 0;
    wifiaa.pwd = "12345678";
    wifiaa.sigLevel = 100;
    wifiaa.ssid = "AAA";
    wifiaa.wepKeyId = 0;
    wifiaa.wifiEnable = 7;

    cmdst_aplist_getaa aplist;
    aplist.aplist[0].ssid = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEF";
    aplist.aplist[0].encryption  = 3;

   initCell(wifiaa,aplist);
   is_wifi_now_on = true;
#endif
}

void WlanTitleCell::on_combox_encryption_currentIndexChanged(int index)
{
    if(1 == index)
    {
        ui->label_keyid->show();
        ui->btKey1->show();
        ui->btKey1->setChecked(true);
        ui->btKey2->show();
        ui->btKey3->show();
        ui->btKey4->show();
        ui->lineEdit_Password->setText("");
        ui->lineEdit_Password->setMaxLength(26);
        QRegExp regexp("^[\\x0020-\\x007e]{1,26}$");
        QValidator *validator = new QRegExpValidator(regexp, this);
        ui->lineEdit_Password->setValidator(validator);
        ui->lineEdit_Password->setStyleSheet("border:2px groove gray;border-radius:5px;");
        ui->label_Password->setEnabled(true);
        ui->lineEdit_Password->setEnabled(true);
        ui->checkBox_visiable->setEnabled(true);
    }
    else if(2 ==index || 3 == index)
    {
        ui->label_keyid->hide();
        ui->btKey1->hide();
        ui->btKey2->hide();
        ui->btKey3->hide();
        ui->btKey4->hide();
        ui->lineEdit_Password->setText("");
        ui->lineEdit_Password->setMaxLength(64);
        QRegExp regexp("^[\\x0020-\\x007e]{1,64}$");
        QValidator *validator = new QRegExpValidator(regexp, this);
        ui->lineEdit_Password->setValidator(validator);
        ui->lineEdit_Password->setStyleSheet("border:2px groove gray;border-radius:5px;");
        ui->label_Password->setEnabled(true);
        ui->lineEdit_Password->setEnabled(true);
        ui->checkBox_visiable->setEnabled(true);
    }
    else if(0 == index)
    {
        ui->label_keyid->hide();
        ui->btKey1->hide();
        ui->btKey2->hide();
        ui->btKey3->hide();
        ui->btKey4->hide();
        ui->label_Password->setEnabled(false);
        ui->lineEdit_Password->setEnabled(false);
        ui->lineEdit_Password->setText("");
        ui->lineEdit_Password->setStyleSheet("border:2px solid rgb(198, 198, 198);border-radius:5px;");
        ui->checkBox_visiable->setEnabled(false);
    }
}

int WlanTitleCell::checkSSID(QString SSID)
{
    for(int i=0; i<aList.size(); i++)
    {
        if(aList.at(i).SSID == SSID)
        {
            return i;
        }
    }
    return -1;
}

void WlanTitleCell::on_btConnect_clicked()
{
//ssid
    int sLen = ui->lineEdit_SSID->text().length();

    if(sLen < 1 || sLen > 32)
    {
        SettingWarming *warming = new SettingWarming(this, tr("ResStr_Msg_9"));
        warming->setWindowTitle(tr("ResStr_Warning"));
        warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        warming->exec();
        return;
    }

//判断输入密码格式是否正确
    APInfo tmpInfo;
    tmpInfo.SSID = ui->lineEdit_SSID->text();
//    tmpInfo.encryType = EncrypType(ui->combox_encryption->currentIndex());
    if(ui->combox_encryption->currentIndex()<2)//2:WPA_PSK_TKIP no use
        tmpInfo.encryType = EncrypType(ui->combox_encryption->currentIndex());
    else
        tmpInfo.encryType = EncrypType(ui->combox_encryption->currentIndex()+1);

    tmpInfo.Password = ui->lineEdit_Password->text();
    tmpInfo.wepKeyID = currentAp->wepKeyID;

    QRegExp reg_Exp("[a-fA-F0-9]{4}");
    int len = ui->lineEdit_Password->text().length();
    char *cPassword = ui->lineEdit_Password->text().toLatin1().data();
    bool bValidetePassWord = true;
    if (ui->combox_encryption->currentIndex() == 1)
    {
        switch(len)
        {
        case 5:
        case 13:
             break;
        case 10:
        case 26:
//            foreach (char ch, password )
            for(int i = 0;i <len;i++)
            {
                if(isdigit(cPassword[i]) ||(cPassword[i] >='a' && cPassword[i] <='f') || (cPassword[i] >='A' && cPassword[i] <='F'))
                {
                    continue;
                }
                else
                {
                    bValidetePassWord = false;
                    break;
                }
            }
            break;
        default:
            bValidetePassWord = false; break;
        }
        if(!bValidetePassWord)
        {
            SettingWarming *warming = new SettingWarming(this, tr("ResStr_Msg_2"));
            warming->setWindowTitle(tr("ResStr_Warning"));
            warming->setWindowFlags(warming->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
            warming->exec();
            return;
        }
    }
    else
    {
        if(len == 64)
        {
            for(int i = 0;i <len;i++)
            {
                if(isdigit(cPassword[i]) ||(cPassword[i] >='a' && cPassword[i] <='f') || (cPassword[i] >='A' && cPassword[i] <='F'))
                {
                    continue;
                }
                else
                {
                    bValidetePassWord = false;
                    break;
                }
            }
            if(!bValidetePassWord)
            {
                SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_Msg_3"));
                msgWarm->setWindowTitle(tr("ResStr_Warning"));
                msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                        & ~Qt::WindowMinimizeButtonHint );
                msgWarm->exec();
                return;
            }
        }
        else if(len < 8 || len > 64)
        {
            SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_Msg_3"));
            msgWarm->setWindowTitle(tr("ResStr_Warning"));
            msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint );
            msgWarm->exec();
            return;
        }
    }

    if(!(*islogin ))
    {
        AuthenticationDlg *dlg = new AuthenticationDlg(this, islogin);
        dlg->setWindowFlags(dlg->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                            & ~Qt::WindowMinimizeButtonHint );
        dlg->setWindowTitle(tr("ResStr_Identity_Authentication"));
        dlg->exec();
    }
    if(*islogin)
    {
        if(tmpInfo.encryType == WEP)
        {
            connect((wifiWepCell), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
            connect((wifiWepCell), SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
            connect((wifiWepCell), SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
            wifiWepCell->tryConnect(tmpInfo);
        }
        else
        {
            connect((wifiCell), SIGNAL(SizeChanged(QSize,QSize)), this, SLOT(getSizeChanged(QSize,QSize)));
            connect((wifiCell), SIGNAL(doingConnect(QWidget*)), this, SLOT(getConnectAction(QWidget*)));
            connect((wifiCell), SIGNAL(connectSuc(QWidget*, bool)), this, SLOT(getConnectResult(QWidget*, bool)));
            wifiCell->tryConnect(tmpInfo);
        }
    }
    else
    {
        QString deviceMsg = tr("ResStr_Setting_Fail");
        gUInterface->setDeviceMsgFrmUI(deviceMsg,1);
    }
}


void WlanTitleCell::on_btKey1_toggled(bool checked)
{
    if(checked)
    {
        currentAp->wepKeyID = 1;
    }
}

void WlanTitleCell::on_btKey2_toggled(bool checked)
{
    if(checked)
    {
        currentAp->wepKeyID = 2;
    }
}

void WlanTitleCell::on_btKey3_toggled(bool checked)
{
    if(checked)
    {
        currentAp->wepKeyID = 3;
    }
}

void WlanTitleCell::on_btKey4_toggled(bool checked)
{
    if(checked)
    {
        currentAp->wepKeyID = 4;
    }
}

void WlanTitleCell::on_checkBox_visiable_toggled(bool checked)
{
    if(checked)
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Normal);
        QValidator *validator2;
        if(ui->combox_encryption->currentIndex() == 1)
        {
            ui->lineEdit_Password->setMaxLength(26);
            QRegExp regexp("^[\\x0020-\\x007e]{1,26}$");
            validator2 = new QRegExpValidator(regexp, this);
        }
        else
        {
            ui->lineEdit_Password->setMaxLength(64);
            QRegExp regexp("^[\\x0020-\\x007e]{1,64}$");
            validator2 = new QRegExpValidator(regexp, this);
        }
        ui->lineEdit_Password->setValidator(validator2);
    }
    else
    {
        ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
        QValidator *validator2;
        if(ui->combox_encryption->currentIndex() == 1)
        {
            ui->lineEdit_Password->setMaxLength(26);
            QRegExp regexp("^[\\x0020-\\x007e]{1,26}$");
            validator2 = new QRegExpValidator(regexp, this);
        }
        else
        {
            ui->lineEdit_Password->setMaxLength(64);
            QRegExp regexp("^[\\x0020-\\x007e]{1,64}$");
            validator2 = new QRegExpValidator(regexp, this);
        }
        ui->lineEdit_Password->setValidator(validator2);
    }
}
