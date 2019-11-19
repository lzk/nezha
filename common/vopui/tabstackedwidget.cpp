#include "tabstackedwidget.h"
#include "ui_tabstackedwidget.h"
#include "qdebug.h"
#include "mainwindow.h"
#include "animationdlg.h"
#include <QGraphicsDropShadowEffect>
#include <QValidator>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QPainter>
#include <QTime>
#include "settingwarming.h"
#include <qdesktopservices.h>
#include <qurl.h>
#include <qapplication.h>
#include "scannerapp.h"
#include "lshell.h"
#ifndef Q_OS_MAC
#include <sys/statfs.h>
#endif
#include <stdio.h>
#include <tiffio.h>
#include "commonapi.h"
#include "uiconfig.h"
#include "statuspaser.h"

TabStackedWidget::TabStackedWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::TabStackedWidget)
  ,scanning(false)
{
    ui->setupUi(this);

    //init copyNum,defaule value is 1;
//    QRegExp rx("^([0-9]|[1-9]\\d)$");
//    QRegExpValidator *pReg = new QRegExpValidator(rx,this);
//    ui->copyNum->setValidator(pReg);
    ui->copyNum->setText(tr("1"));

    //init Density,default is level2;
    value_Density = 3;
    ui->mark0_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
    ui->mark1_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
    ui->mark2_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
    ui->mark3_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
    ui->mark4_Density->setStyleSheet("background-color: rgb(154, 238, 117);");

    //init Param_Copy
    this->setDefault_Copy();

    selectState = "QPushButton{border-image: url(:/Images/Btn_Green.png);color:white;}";
    selectHover = "QPushButton::hover{color:black;}";
    selectPressed = "QPushButton::pressed{border-image: url(:/Images/Btn_Green_Pressed.png);color:white;}";

    unSelectState = "QPushButton{border-image: url(:/Images/Btn_Gray.png);}";
    unSelectHover = "QPushButton::hover{color: rgb(52, 212, 34);}";
    unSelectPressed = "QPushButton::pressed{border-image: url(:/Images/Btn_Gray_Pressed.png);color:white;}";


    QString strStyleScrollBar="QScrollBar:vertical { background: rgb(127, 127, 127); } "
            "QListWidget {background-image: url(); background-color:white;border-width:0px;}";
    ui->scrollArea_ScanImage->setStyleSheet(strStyleScrollBar);

    timerCopyNum = new QTimer(this);

    timerClick = new QTimer(this);
    connect(timerClick,SIGNAL(timeout()),this,SLOT(timerItemClick()));

    on_scrollArea_ScanImage_itemSelectionChanged();
    ui->disableScrollArea->hide();
    //bms:7670
    ui->scrollArea_ScanImage->setFocusPolicy(Qt::NoFocus);

    this->setDefault_Scan();

    QString labelTitle = tr("ResStr_Scanned_image_size");
    QString labelText = QString("%1%2").arg(labelTitle).arg("24.89MB");
    ui->label_ImageSize->setText(labelText);

    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)), this ,SLOT(cmdResult(int,int,QVariant)));

//    connect(ui->settingStackedWidget,SIGNAL(cycleStart()),this,SLOT(startCycleEmit()));
//    connect(ui->settingStackedWidget,SIGNAL(cycleStop()),this,SLOT(stopCycleEmit()));
//    connect(ui->settingStackedWidget->titelCell,SIGNAL(cycleStartFromWT()),this,SLOT(startCycleEmit()));
//    connect(ui->settingStackedWidget->titelCell,SIGNAL(cycleStopFromWT()),this,SLOT(stopCycleEmit()));
    connect(gUInterface,SIGNAL(signal_update_scan_progress(int)),this,SLOT(updateScanProcess(int)));

    m_oldJob = UIConfig::UnknowJob;

    QSettings settings(g_config_file ,QSettings::NativeFormat);
    paramCopy.promptInfo.isDuplex = settings.value("isDuplex" ,QVariant(1)).toInt();
    paramCopy.promptInfo.isIDCard = settings.value("isIDCard" ,QVariant(true)).toBool();
    paramCopy.promptInfo.isMultible = settings.value("isMultible" ,QVariant(true)).toBool();
}

TabStackedWidget::~TabStackedWidget()
{
    delete ui;
    QSettings settings(g_config_file ,QSettings::NativeFormat);
    settings.setValue("isDuplex",paramCopy.promptInfo.isDuplex);
    settings.setValue("isIDCard",paramCopy.promptInfo.isIDCard);
    settings.setValue("isMultible",paramCopy.promptInfo.isMultible);
}
void TabStackedWidget::cmdResult(int cmd,int result,QVariant data)
{
    switch(cmd)
    {
    case UIConfig::CMD_GetStatus:
    {
        PrinterInfo_struct printerInfo = data.value<PrinterInfo_struct>();
        if(result || !printerInfo.printer.isConnected){
            ui->btn_Copy->setEnabled(false);
            ui->btn_Scan->setEnabled(false);
            ui->settingStackedWidget->setEnabled(false);
        }else{
            PrinterStatus_struct& status = printerInfo.status;
            ui->btn_Copy->setEnabled(StatusPaser::is_enable_copy(status.PrinterStatus));
            if(scanning)
                ui->btn_Scan->setEnabled(false);
            else
                ui->btn_Scan->setEnabled(StatusPaser::is_enable_scan(status.PrinterStatus));
            ui->settingStackedWidget->setEnabled(StatusPaser::is_enable_setting(status.PrinterStatus));

            //bms8160
            UIConfig::EnumMachineJob job = (UIConfig::EnumMachineJob)status.job;
            if ( m_oldJob == UIConfig::IDCardCopyJob && m_oldJob != job )
            {
                ui->cBox_IsIDCard->setChecked(false);
                ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
                ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconDisable.png);");
                ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
            }
            if (m_oldJob == UIConfig::NormalCopyJob && m_oldJob != job)
            {
                ui->cBox_DuplexCopy->setChecked(false);
                ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
                ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconDisable.tif);");
                ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
            }

            m_oldJob = job;
        }
        break;
    }
    case UIConfig::LS_CMD_COPY:
    {
//        qDebug()<<"LS_CMD_COPY"<<result;
        gUInterface->emitEnableCycleAnimation(false);
        if(result != 0)
        {
            gUInterface->setDeviceMsgFrmUI(tr("ResStr_Copy_Fail"),result);
            if(result == LShell::ERR_Do_not_support)
            {
                SettingWarming *busyWarning = new SettingWarming(this, tr("ResStr_Unsupported"));
                busyWarning->setWindowTitle(tr("ResStr_Error"));

                busyWarning->setWindowFlags(busyWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                busyWarning->exec();
            }
            else if(result == LShell::ERR_Printer_busy)
            {
                SettingWarming *busyWarning = new SettingWarming(this, tr("ResStr_The_machine_is_busy__please_try_later_"),2);
                busyWarning->setWindowTitle(tr("ResStr_Error"));

                busyWarning->setWindowFlags(busyWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                busyWarning->exec();
            }
            else if (result == LShell::ERR_Printer_error)
            {
                SettingWarming *errorWarning = new SettingWarming(this, tr("ResStr_Operation_can_not_be_carried_out_due_to_machine_malfunction_"));
                errorWarning->setWindowTitle(tr("ResStr_Error"));

                errorWarning->setWindowFlags(errorWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                errorWarning->exec();
            }
            else
            {
                SettingWarming *errorWarning = new SettingWarming(this, tr("ResStr_Operation_can_not_be_carried_out_due_to_machine_malfunction_"));
                errorWarning->setWindowTitle(tr("ResStr_Error"));

                errorWarning->setWindowFlags(errorWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                errorWarning->exec();
            }
        }
    }

        break;
    case UIConfig::CMD_Scan:
    {
//        qDebug()<<"CMD_Scan"<<result;
        if(!result)
        {
            ScanSettings scanSettings = data.value<ScanSettings>();

            QString image_path = scanSettings.filename;
            QSize size = QSize(scanSettings.calc_data.target.pixels_per_line,scanSettings.calc_data.target.total_lines);
            ui->scrollArea_ScanImage->add_image_item(image_path ,size);
        }
        else if(result != 0 && result != ScannerApp::STATUS_Cancel)
        {
            gUInterface->setDeviceMsgFrmUI(tr("ResStr_Scan_Fail"),result);
            if(result == ScannerApp::STATUS_Error_busy)
//            if(result == ScannerApp::STATUS_USEWITHOUTLOCK)
            {
                SettingWarming *busyWarning = new SettingWarming(this, tr("ResStr_The_machine_is_busy__please_try_later_"),2);
                busyWarning->setWindowTitle(tr("ResStr_Error"));

                busyWarning->setWindowFlags(busyWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                busyWarning->exec();
            }
            else if(result == ScannerApp::STATUS_OUTOFMEMERY)
            {
                SettingWarming *errorWarning = new SettingWarming(this, tr("ResStr_Operation_cannot_be_carried_out_due_to_insufficient_memory_or_hard_disk_space_Please_try_again_after_freeing_memory_or_hard_disk_space_"));
                errorWarning->setWindowTitle(tr("ResStr_Error"));

                errorWarning->setWindowFlags(errorWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                errorWarning->exec();
            }
//            else if(result == ScannerApp::STATUS_Error_App)
//            {
//                SettingWarming *errorWarning = new SettingWarming(this, tr("ResStr_can_not_be_carried_out_due_to_software_has_error__please_try__again_after_reinstall_the_Driver_and_Virtual_Operation_Panel_"));
//                errorWarning->setWindowTitle(tr("ResStr_Error"));

//                errorWarning->setWindowFlags(errorWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
//                                    & ~Qt::WindowMinimizeButtonHint);
//                errorWarning->exec();
//            }
            else
//                if(result == ScannerApp::STATUS_Error_machine)
            {
                SettingWarming *errorWarning = new SettingWarming(this, tr("ResStr_Operation_can_not_be_carried_out_due_to_machine_malfunction_"));
                errorWarning->setWindowTitle(tr("ResStr_Error"));

                errorWarning->setWindowFlags(errorWarning->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
                errorWarning->exec();
            }
        }

        gUInterface->emitStopScanSignal();
        ui->scrollArea_ScanImage->setEnabled(true);
        ui->disableScrollArea->hide();
        if(ui->scrollArea_ScanImage->selectedItems().isEmpty()){
            ui->btn_ScanSave->setEnabled(false);
        }else{
            ui->btn_ScanSave->setEnabled(true);
        }
        ui->btn_MoreSetting_Scan->setEnabled(true);
//        ui->btn_Scan->setEnabled(true);
        ui->btn_ScanCancel->setEnabled(false);
        ui->progressBar_Scan->setValue(0);
        scanning = false;
    }
        break;
    default:
        break;
    }
}

void TabStackedWidget::updateScanProcess(int progress)
{
//    qDebug()<<"TabStackedWidget:"<<progress;
    ui->progressBar_Scan->setValue(progress);
}

bool TabStackedWidget::getScrollAreaImageStatus()
{

    if(ui->scrollArea_ScanImage->count() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


void TabStackedWidget::setDefault_Copy(bool isExceptTips)
{
    copycmdset para;
    copycmdset *p = &para;
    memcpy(p ,&default_copy_parameter ,sizeof(default_copy_parameter));

    paramCopy.scaling = (int)p->scale;
    paramCopy.docType = (DocType_Copy)p->scanMode;
    int country = QLocale::system().country();
    if(country == QLocale::UnitedStates)
    {
        paramCopy.docSize = DocSize_Copy_Letter;
        paramCopy.outputSize = OutPutSize_Copy_letter;
    }else
    {
        paramCopy.docSize = DocSize_Copy_A4;
        paramCopy.outputSize = OutPutSize_Copy_A4;
    }
    paramCopy.docDpi = (DocDpi_Copy)p->dpi;
    paramCopy.paperType = (MediaType_Copy)p->mediaType;
    paramCopy.isMultiPage = false;
    paramCopy.multiMode = (MultiMode_Copy)p->nUp;
    paramCopy.idCardCopyMode = A4Mode1;
    paramCopy.duplexMode = Flip_Long_Edge;
//    paramCopy.promptInfo.isDuplex = 1;
    if(isExceptTips == false)
    {
//        paramCopy.promptInfo.isIDCard = true;
//        paramCopy.promptInfo.isMultible = true;
    }
    else
    {
        ui->copyNum->setText(tr("1"));

        //init Density,default is level2;
        value_Density = 3;
        ui->mark0_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
        ui->mark1_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
        ui->mark2_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
        ui->mark3_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
        ui->mark4_Density->setStyleSheet("background-color: rgb(154, 238, 117);");

        ui->cBox_DuplexCopy->setChecked(false);
        ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconDisable.tif);");

        ui->cBox_IsIDCard->setChecked(false);
        ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconDisable.png);");
        ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
    }
}

void TabStackedWidget::setDefault_Scan()
{
    paramScan.scan_doctype = T_Photo;
    paramScan.scan_dpi = Scan_300DPI;
    paramScan.colorModel = Color;
    paramScan.contrast = 50;
    paramScan.brightness = 50;
    paramScan.scan_size = Scan_A4;
    paramScan.scan_type = Hight_Speed;
}

void TabStackedWidget::timerItemClick()
{
    timerClick->stop();
    if(ui->scrollArea_ScanImage->selectedItems().isEmpty()){
        ui->btn_ScanSave->setEnabled(false);
    }else{
        ui->btn_ScanSave->setEnabled(true);
    }
}

void TabStackedWidget::on_scrollArea_ScanImage_itemSelectionChanged()
{
    timerClick->start(440);
}

void TabStackedWidget::initWiFi_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless_Active.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    ui->settingStackedWidget->changeStackIndex(0);
    ui->settingStackedWidget->setCurrentIndex(0);
}
void TabStackedWidget::on_btn_WiFi_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless_Active.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
    if(ui->settingStackedWidget->currentIndex() > 0)
    {
        ui->settingStackedWidget->changeStackIndex(0);
        ui->settingStackedWidget->setCurrentIndex(0);
    }
}

void TabStackedWidget::on_btn_SoftAP_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP_Active.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
   ui->settingStackedWidget->changeStackIndex(1);
    ui->settingStackedWidget->setCurrentIndex(1);
}

void TabStackedWidget::on_btn_TCPIPV4_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP_Active.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
    ui->settingStackedWidget->changeStackIndex(2);
    ui->settingStackedWidget->setCurrentIndex(2);
}

void TabStackedWidget::on_btn_TCPIPV6_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP_Active.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
    ui->settingStackedWidget->changeStackIndex(3);
    ui->settingStackedWidget->setCurrentIndex(3);
}

void TabStackedWidget::on_btn_PowerSave_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave_Active.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
    ui->settingStackedWidget->changeStackIndex(4);
    ui->settingStackedWidget->setCurrentIndex(4);
}

void TabStackedWidget::on_btn_UserConfig_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig_Active.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(selectState+selectHover+selectPressed);
    ui->btn_Password->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);

    //neil add for settings page 2016-01-29
    ui->settingStackedWidget->changeStackIndex(5);
    ui->settingStackedWidget->setCurrentIndex(5);
}

void TabStackedWidget::on_btn_Password_clicked()
{
    ui->img_WiFi->setStyleSheet("border-image: url(:/Images/Wireless.png)");
    ui->img_SoftAP->setStyleSheet("border-image: url(:/Images/SoftAP.png)");
    ui->img_TCPIPV4->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_TCPIPV6->setStyleSheet("border-image: url(:/Images/TCPIP.png)");
    ui->img_PowerSave->setStyleSheet("border-image: url(:/Images/PowerSave.png)");
    ui->img_UserConfig->setStyleSheet("border-image: url(:/Images/UserConfig.png)");
    ui->img_Password->setStyleSheet("border-image: url(:/Images/Password_Active.png)");

    ui->btn_WiFi->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_SoftAP->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV4->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_TCPIPV6->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_PowerSave->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_UserConfig->setStyleSheet(unSelectState+unSelectHover+unSelectPressed);
    ui->btn_Password->setStyleSheet(selectState+selectHover+selectPressed);

    //neil add for settings page 2016-01-29
    ui->settingStackedWidget->changeStackIndex(6);
    ui->settingStackedWidget->setCurrentIndex(6);
}


void TabStackedWidget::on_cBox_IsIDCard_clicked(bool checked)
{
    if (true==checked)
    {
        ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
        ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconEnable.png);");
        ui->btn_Copy->setText(tr("ResStr_ID_Card_Copy"));

        ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->cBox_DuplexCopy->setChecked(false);
        ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconDisable.tif);");
    }
    else
    {
        ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconDisable.png);");
        ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
    }
}

void TabStackedWidget::on_cBox_DuplexCopy_clicked(bool checked)
{
    if (true==checked)
    {
        ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->cBox_IsIDCard->setChecked(false);
        ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconDisable.png);");

        ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Open.png);");
        ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconEnable.tif);");
        ui->btn_Copy->setText(tr("ResStr_Duplex_Copy"));
    }
    else
    {
        ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
        ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconDisable.tif);");
        ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
    }
}

void TabStackedWidget::on_btn_Scan_clicked()
{
//    qDebug()<<"on_btn_Scan_clicked";
//    const char *image_path = "/tmp/vop_scan/2019-03-12_15-45-11-475.bmp";

//    QSize size = QSize(2496,3507);
//    ui->scrollArea_ScanImage->add_image_item(image_path ,size);

    if(is_disk_no_space("/tmp" ,500)){
        SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_Operation_cannot_be_carried_out_due_to_insufficient_memory_or_hard_disk_space_Please_try_again_after_freeing_memory_or_hard_disk_space_"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
        return;
    }
    QVariant data;
    ScanSettings paraScanSettings;
    paraScanSettings.settings = paramScan;
//    qDebug()<<"scan_doctype"<<paramScan.scan_doctype;
    data.setValue<ScanSettings>(paraScanSettings);
    gUInterface->setCurrentPrinterCmd(UIConfig::CMD_Scan,data);

    gUInterface->emitStartScanSignal();
    ui->btn_ScanSave->setEnabled(false);
    ui->scrollArea_ScanImage->setEnabled(false);
    ui->disableScrollArea->show();
    ui->btn_MoreSetting_Scan->setEnabled(false);
    ui->btn_Scan->setEnabled(false);
    ui->btn_ScanCancel->setEnabled(true);

    scanning = true;
}

void TabStackedWidget::on_btn_MoreSetting_Scan_clicked()
{
    MoreSettingsForScan *moreSettingScan = new MoreSettingsForScan(this,&paramScan);
    connect(moreSettingScan, SIGNAL(signals_scan_buffer_size(float,int)), SLOT(slots_scan_image_size(float,int)));
    moreSettingScan->exec();
}

void TabStackedWidget::slots_scan_image_size(float size, int unit)//Added by gavin for setting scan buffer size. 2016-04-08
{
    QString labelTitle = tr("ResStr_Scanned_image_size");
    char sizeText[16] = {0};
    if(unit == 1)
    {
        sprintf(sizeText, "%0.2f%s", size, "KB");
    }
    else if(unit == 2)
    {
        sprintf(sizeText, "%0.2f%s", size, "MB");
    }
    else
    {
        sprintf(sizeText, "%d%s", (int)size, "B");
    }
    QString labelText = QString("%1%2").arg(labelTitle).arg(sizeText);

    ui->label_ImageSize->setText(labelText);

}

void TabStackedWidget::on_btn_MoreSetting_Copy_clicked()
{
    int mode = 0;
    if(!ui->icon_DuplexCopy->isVisible()){
        mode = 1;
        this->setStyleSheet("#MoreSettingsForCopy {background-image: url(:/Images/moreSettings_Copy_2.png);}");
    }
    else
    {
        this->setStyleSheet("#MoreSettingsForCopy {background-image: url(:/Images/moreSettings_Copy_1.png);}");
    }
    MoreSettingsForCopy *moreSettingsForCopy = new MoreSettingsForCopy(this,ui->cBox_DuplexCopy->isChecked(),ui->cBox_IsIDCard->isChecked(),&paramCopy ,mode);
    moreSettingsForCopy->exec();
}

void TabStackedWidget::on_btn_DensityAdd_clicked()
{
    if (value_Density < 5)
    {
        value_Density++;
        switch (value_Density) {
        case 5:
            ui->mark4_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
            break;
        case 4:
            ui->mark3_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
            break;
        case 3:
            ui->mark2_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
            break;
        case 2:
            ui->mark1_Density->setStyleSheet("background-color: rgb(53, 177, 20);");
            break;
        default:
            break;
        }
    }
}

void TabStackedWidget::on_btn_DensityReduce_clicked()
{
    if (value_Density > 1)
    {
        value_Density--;
        switch (value_Density) {
        case 4:
            ui->mark4_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
            break;
        case 3:
            ui->mark3_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
            break;
        case 2:
            ui->mark2_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
            break;
        case 1:
            ui->mark1_Density->setStyleSheet("background-color: rgb(154, 238, 117);");
            break;
        default:
            break;
        }
    }
}

void TabStackedWidget::on_btn_CopyNumAdd_clicked()
{
    int value = ui->copyNum->text().toInt();
    if (value < 99)
    {
        value ++;
        QString str;
        str.setNum(value);
        ui->copyNum->setText(str);
    }
}

void TabStackedWidget::on_btn_CopyNumReduce_clicked()
{
    int value = ui->copyNum->text().toInt();
    if (value > 1)
    {
        value --;
        QString str;
        str.setNum(value);
        ui->copyNum->setText(str);
    }
}

void TabStackedWidget::setCopyStackedWidgetCurrentIndex(int index)
{
    ui->copyPage->setHidden(false);
    ui->copyStackedWidget->setCurrentIndex(index);
}

void TabStackedWidget::set_copy_enabled(bool enabled)
{
    ui->btn_Copy->setEnabled(enabled);
}

void TabStackedWidget::set_scan_enabled(bool enabled)
{
     ui->btn_Scan->setEnabled(enabled);
}

void TabStackedWidget::set_setting_enabled(bool enabled)
{
    ui->settingStackedWidget->setEnabled(enabled);
}

//void TabStackedWidget::startCycleEmit()
//{
//    emit cycleStartFromTab();
//}

//void TabStackedWidget::stopCycleEmit()
//{
//    emit cycleStopFromTab();
//}

void TabStackedWidget::setEnabledDuplexCopy(bool enabled)
{
//    qDebug()<<"setEnabledDuplexCopy"<<enabled;
    ui->text_DuplexCopy->setHidden(!enabled);
    ui->icon_DuplexCopy->setHidden(!enabled);
    ui->cBox_DuplexCopy->setHidden(!enabled);
    QRect geometry = ui->horizontalWidget->geometry();
    if(enabled == false)
    {
        geometry.setX(120);
        ui->horizontalWidget->setGeometry(geometry);
    }
    else
    {
        geometry.setX(30);
        ui->horizontalWidget->setGeometry(geometry);
    }
}

void TabStackedWidget::setEnabledCopyScan(bool enabled)
{
    ui->copyPage->setHidden(!enabled);
    if(enabled)
    {
        ui->settingPage->setStyleSheet("#settingPage{background-image: url(:/Images/PageScan.tif);}");
    }
    else
    {
        ui->scanPage->setHidden(true);
        ui->settingPage->setStyleSheet("#settingPage{background-image: url(:/Images/PagePrint.tif);}");
    }
}
void TabStackedWidget::setTitelCellEnabled(bool enabled)
{
    if(enabled)
        ui->settingStackedWidget->titelCell->setWifiEnabled(true);
    else
        ui->settingStackedWidget->titelCell->setWifiEnabled(false);
}

void TabStackedWidget::setEnabledNet(bool enabled)
{
    ui->widget_ipv4->show();
    ui->widget_ipv6->show();
    ui->settingStackedWidget->setNetEnabled(!enabled);
    if(enabled){
        ui->widget_wifi->hide();
        ui->widget_softap->hide();
        ui->settingStackedWidget->setCurrentIndex(2);
    }else{
        ui->widget_wifi->show();
        ui->widget_softap->show();
        ui->settingStackedWidget->setCurrentIndex(0);
    }
}

void TabStackedWidget::setEnabledWifi(bool enabled)
{
#if 1
    ui->settingStackedWidget->setWifiEnabled(enabled);
    if(enabled){
        ui->widget_wifi->show();
        ui->widget_ipv4->show();
        ui->widget_ipv6->show();
        ui->widget_softap->show();
        ui->settingStackedWidget->setCurrentIndex(0);
    }else{
        ui->widget_wifi->hide();
        ui->widget_ipv4->hide();
        ui->widget_ipv6->hide();
        ui->widget_softap->hide();
        ui->settingStackedWidget->setCurrentIndex(4);
    }
#else
    ui->settingStackedWidget->setWifiEnabled(enabled);
    ui->btn_WiFi->setHidden(!enabled);
    ui->btn_SoftAP->setHidden(!enabled);
    ui->btn_TCPIPV4->setHidden(!enabled);
    ui->btn_TCPIPV6->setHidden(!enabled);
    ui->img_WiFi->setHidden(!enabled);
    ui->img_SoftAP->setHidden(!enabled);
    ui->img_TCPIPV4->setHidden(!enabled);
    ui->img_TCPIPV6->setHidden(!enabled);

    if(enabled)
    {
        ui->settingStackedWidget->setCurrentIndex(0);
        ui->btn_PowerSave->setGeometry(ui->btn_TCPIPV6->geometry().x(),ui->btn_TCPIPV6->geometry().y()+40,ui->btn_TCPIPV6->geometry().width(),ui->btn_TCPIPV6->geometry().height());
        ui->btn_UserConfig->setGeometry(ui->btn_TCPIPV6->geometry().x(),ui->btn_TCPIPV6->geometry().y()+80,ui->btn_TCPIPV6->geometry().width(),ui->btn_TCPIPV6->geometry().height());
        ui->btn_Password->setGeometry(ui->btn_TCPIPV6->geometry().x(),ui->btn_TCPIPV6->geometry().y()+120,ui->btn_TCPIPV6->geometry().width(),ui->btn_TCPIPV6->geometry().height());
        QRect rect = ui->img_PowerSave->geometry();
        ui->img_PowerSave->setGeometry(rect.x(),ui->btn_PowerSave->geometry().y()+(ui->btn_PowerSave->geometry().height()-rect.height())/2,rect.width(),rect.height());
        rect = ui->img_UserConfig->geometry();
        ui->img_UserConfig->setGeometry(rect.x(),ui->btn_UserConfig->geometry().y()+(ui->btn_UserConfig->geometry().height()-rect.height())/2,rect.width(),rect.height());
        rect = ui->img_Password->geometry();
        ui->img_Password->setGeometry(rect.x(),ui->btn_Password->geometry().y()+(ui->btn_Password->geometry().height()-rect.height())/2,rect.width(),rect.height());

    }
    else
    {
        ui->settingStackedWidget->setCurrentIndex(4);
        ui->btn_PowerSave->setGeometry(ui->btn_WiFi->geometry());
        ui->btn_UserConfig->setGeometry(ui->btn_SoftAP->geometry());
        ui->btn_Password->setGeometry(ui->btn_TCPIPV4->geometry());
        QRect rect = ui->img_PowerSave->geometry();
        ui->img_PowerSave->setGeometry(rect.x(),ui->btn_PowerSave->geometry().y()+(ui->btn_PowerSave->geometry().height()-rect.height())/2,rect.width(),rect.height());
        rect = ui->img_UserConfig->geometry();
        ui->img_UserConfig->setGeometry(rect.x(),ui->btn_UserConfig->geometry().y()+(ui->btn_UserConfig->geometry().height()-rect.height())/2,rect.width(),rect.height());
        rect = ui->img_Password->geometry();
        ui->img_Password->setGeometry(rect.x(),ui->btn_Password->geometry().y()+(ui->btn_Password->geometry().height()-rect.height())/2,rect.width(),rect.height());
    }
#endif
}

void TabStackedWidget::on_btn_Copy_clicked()
{
//    qDebug()<<"start copy";
    if (ui->cBox_DuplexCopy->isChecked() == true)
    {
        if(paramCopy.promptInfo.isDuplex == 1){//duplex checked to show automation
            LOGLOG("show duplex automation");
            bool enNextShow = true;
    //            QString videoTypeStr = "01_JAM";
    //            QString languageStr = "SimplifiedChinese";
            AnimationDlg *aDialog = new AnimationDlg(this, UIConfig::Show_duplex_automation, &enNextShow);
            aDialog->setMaximumSize(400,650);
            aDialog->setMinimumSize(400,650);
            aDialog->setAttribute(Qt::WA_DeleteOnClose);
            if (aDialog->exec() == QDialog::Rejected)
            {
                return;
            }
            if(enNextShow == false)
            {
                paramCopy.promptInfo.isDuplex = 0;
            }
        }
    }else
    if (ui->cBox_IsIDCard->isChecked() == true)
    {
        if (paramCopy.promptInfo.isIDCard == true)
        {
            bool enNextShow = true;
//            QString videoTypeStr = "01_JAM";
//            QString languageStr = "SimplifiedChinese";
            AnimationDlg *aDialog = new AnimationDlg(this, UIConfig::Show_ID_Card_automation, &enNextShow);
            aDialog->setMaximumSize(400,650);
            aDialog->setMinimumSize(400,650);
            aDialog->setAttribute(Qt::WA_DeleteOnClose);
            if (aDialog->exec() == QDialog::Rejected)
            {
//                qDebug()<<"Rejected";
                return;
            }
            if(enNextShow == false)
            {
                paramCopy.promptInfo.isIDCard = false;
            }
        }

        paramCopy.scaling = 100;
        paramCopy.docDpi = DocDpi_Copy_DPI300;
        paramCopy.isMultiPage = false;
        paramCopy.multiMode = IDCardCopyFlag; // This value present sending ID Card Copy command.

    }
    else
    {
        if (paramCopy.promptInfo.isMultible == true && paramCopy.isMultiPage == true)
        {
            bool enNextShow = true;
//            QString videoTypeStr = "06_Nin1Copy";
//            QString languageStr = "SimplifiedChinese";
            AnimationDlg *aDialog = new AnimationDlg(this, UIConfig::Show_Nin1_automation, &enNextShow);
            aDialog->setMaximumSize(400,650);
            aDialog->setMinimumSize(400,650);
            aDialog->setAttribute(Qt::WA_DeleteOnClose);
            if (aDialog->exec() == QDialog::Rejected)
            {
                return;
            }
//            qDebug()<<"enNextShow"<<enNextShow;
            if(enNextShow == false)
            {
                paramCopy.promptInfo.isMultible = false;
            }
        }
    }

    copycmdset copyPara;
    copyPara.copyNum = ui->copyNum->text().toUInt();
    copyPara.Density = value_Density;
    copyPara.scanMode = paramCopy.docType;
    copyPara.orgSize = paramCopy.docSize;
    copyPara.paperSize = paramCopy.outputSize;

    if (paramCopy.isMultiPage == false && ui->cBox_IsIDCard->isChecked() == false)
    {
        copyPara.nUp = OneInOne;
    }
    else
    {
        copyPara.nUp = paramCopy.multiMode;
    }

    copyPara.dpi = paramCopy.docDpi;
    copyPara.mediaType = paramCopy.docType;
    copyPara.scale = paramCopy.scaling;

    if(ui->cBox_IsIDCard->isChecked() == true)
    {
        copyPara.IDCardMode = paramCopy.idCardCopyMode;
        if(copyPara.IDCardMode != A5Mode)
        {
            int country = QLocale::system().country();
            if(country == QLocale::UnitedStates)
            {
                copyPara.paperSize = OutPutSize_Copy_letter;
            }
            else
            {
                copyPara.paperSize = OutPutSize_Copy_A4;
            }
        }
        else
        {
            copyPara.paperSize = OutPutSize_Copy_A5;
        }
    }

    if(ui->cBox_DuplexCopy->isChecked() == true)
    {
        copyPara.duplexCopy = paramCopy.duplexMode + 1;
        copyPara.nUp = 0; // Reset Nin1 value for duplex mode
    }
    else
    {
        copyPara.duplexCopy = 0;
    }

    QVariant data;
    data.setValue<copycmdset>(copyPara);
    gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_COPY,data);
    gUInterface->emitEnableCycleAnimation(true);
}

void TabStackedWidget::recoverCopyMode()
{
    ui->cBox_IsIDCard->setChecked(false);
    ui->cBox_DuplexCopy->setChecked(false);
    ui->cBox_IsIDCard->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
    ui->icon_IDCardCopy->setStyleSheet("border-image: url(:/Images/IDCardCopyIconDisable.png);");
    ui->cBox_DuplexCopy->setStyleSheet("border-image: url(:/Images/CheckBox_Close.png);");
    ui->icon_DuplexCopy->setStyleSheet("border-image: url(:/Images/DulplexCopyIconDisable.tif);");
    ui->btn_Copy->setText(tr("ResStr_ExtraAdd_Copy"));
}

#define _QT_PDF 1
#if _QT_PDF
#include <QPainter>
#include <QPrinter>

//extern void saveMultiPageTiffImage(NSString* tmpPath ,NSString* imageFilePath);
//void saveMultiPageTiffImage(const QString& tmpPath ,const QString& imageFilePath)
//{
//    return saveMultiPageTiffImage(tmpPath.toNSString() ,imageFilePath.toNSString());
//}

static QPrinter* printer = NULL;
static QPainter* painter = NULL;
void saveMultiPagePdfImageInit(const QString& fileName)
{
    printer = new QPrinter;
    printer->setPageSize(QPrinter::A4); //设置纸张大小为A4
    printer->setOutputFormat(QPrinter::PdfFormat); //设置输出格式为pdf
    printer->setOutputFileName(fileName);  //设置输出路径
    painter = new QPainter(printer);
}

bool saveMultiPagePdfImage(const QString& tmpPath ,bool firstPage)
{
    QRect rect = painter->viewport();
    if(rect.isEmpty())
        return false;
    QPixmap pixmap = QPixmap(tmpPath).scaled(rect.size() ,Qt::KeepAspectRatio);
    QSize size = pixmap.size();
    if(!size.isValid())
        return false;
    if(!firstPage)
        printer->newPage();
    painter->drawPixmap(rect.x() + (rect.width() - size.width()) / 2,
                       rect.y() + (rect.height() - size.height()) / 2,pixmap);
    return true;
}

void saveMultiPagePdfImageRelease()
{
    if(painter)
        delete painter;
    painter = NULL;
    if(printer)
        delete printer;
    printer = NULL;
}
#endif
void TabStackedWidget::on_btn_ScanSave_clicked()
{
    if(is_disk_no_space("/tmp" ,500)){
        SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_Operation_cannot_be_carried_out_due_to_insufficient_memory_or_hard_disk_space_Please_try_again_after_freeing_memory_or_hard_disk_space_"));
        msgWarm->setWindowTitle(tr("ResStr_Warning"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
        return;
    }
    QList<QListWidgetItem*> item_list = ui->scrollArea_ScanImage->selectedItems();
    QString filter = "TIF(*.tiff);;PDF(*.pdf);;JPG(*.jpg)";
    QString selectedFilter;
    if(item_list.count() > 1){
        filter = "TIF(*.tiff);;PDF(*.pdf)";
    }
#if QT_VERSION > 0x050000
    QStringList pathlist = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    static QString picPath = pathlist.isEmpty() ?QString() :pathlist.at(0);
#else
    static QString picPath = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
#endif
    QString savefilename = QFileDialog::getSaveFileName(0 ,tr("Save File"), picPath,filter,&selectedFilter);
    QFileInfo fi = QFileInfo(savefilename);
    picPath = fi.absoluteFilePath();
    QString filename = "/tmp/" + fi.fileName();
    QString temp_filename;
    if(!savefilename.isEmpty()){
//        if(filename.endsWith(".pdf")){
        if(selectedFilter == "PDF(*.pdf)"){
            if(filename.endsWith(".pdf") == false)
            {
                filename = filename.append(".pdf");
                savefilename = savefilename.append(".pdf");
            }

//            qDebug()<<filename;
            saveMultiPagePdfImageInit(filename);
            int j = 0;
            foreach (QListWidgetItem* item, item_list) {
                temp_filename = item->data(Qt::UserRole).toString();
                saveMultiPagePdfImage(temp_filename ,j == 0);
                j++;
            }
            saveMultiPagePdfImageRelease();
//        }else if(filename.endsWith(".tiff")){
        }else if(selectedFilter == "TIF(*.tiff)"){
            if(filename.endsWith(".tiff") == false)
            {
                filename = filename.append(".tiff");
                savefilename = savefilename.append(".tiff");
            }
//            qDebug()<<filename;
            int image_width,image_height;
            TIFF *tif = TIFFOpen(filename.toLocal8Bit().data(),"w");

            if(tif == NULL)
            {
                LOGLOG("save tif error");
                return;
            }
            TIFFSetField(tif,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
            TIFFSetField(tif,TIFFTAG_PAGENUMBER,item_list.count());
            for (int idx = 0; idx < item_list.count(); idx ++) {
                QListWidgetItem* item = item_list.at(idx);
                temp_filename = item->data(Qt::UserRole).toString();

                QImage image(temp_filename);
                image_width = image.width();
                image_height = image.height();

                TIFFSetDirectory(tif,idx + 1);
                TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,image_width);
                TIFFSetField(tif,TIFFTAG_IMAGELENGTH,image_height);
                TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);

//                qDebug()<<image.bitPlaneCount()<<"/ "<<image.isGrayscale()<<"/ "<<image.bytesPerLine()<<"/ ";
                if(image.bitPlaneCount() == 1)
                {
//                    qDebug()<<"whiteblack";
                    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE); // 0=black
                    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1);
                    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, image_height);

                    uchar *data = image.bits();
                    for (int idx =0;idx<image_height;idx++)
                    {
                        TIFFWriteScanline(tif,&data[idx*image_width/8],idx,0);
                    }
                }
                else if(image.bitPlaneCount() == 8)
                {
//                    qDebug()<<"Grayscale";
                    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK); // 0=black
                    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);//
                    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
                    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
                    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, image_height);

                    uchar *data = image.bits();
                    for (int idx =0;idx<image_height;idx++)
                    {
                        TIFFWriteScanline(tif,&data[idx*image_width],idx,0);
                    }
                }
                else if(image.bitPlaneCount() == 24)
                {
//                    qDebug()<<"Color";
                    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); // 0=black
                    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
                    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
                    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, image_height);
                    uchar *data = image.bits();
                    uchar *pdst = new uchar[image_width*4];

                    for (int idx =0;idx<image_height;idx++)
                    {
                        int curi_bit = idx*image_width*4;
                        for(int i = 0; i < image_width; i++)
                        {
                            int curi_dst = i*4;
                            int curi_bit2 = curi_dst + curi_bit;

                            pdst[curi_dst] = data[curi_bit2+2];
                            pdst[curi_dst+1] = data[curi_bit2+1];
                            pdst[curi_dst+2] = data[curi_bit2];
                            pdst[curi_dst+3] = data[curi_bit2+3];
                        }
                        TIFFWriteScanline(tif,pdst,idx,0);
                    }
                }

                TIFFWriteDirectory(tif);
            }
            TIFFClose(tif);
        }else{
            if(filename.endsWith(".jpg") == false)
            {
                filename = filename.append(".jpg");
                savefilename = savefilename.append(".jpg");
            }
//            qDebug()<<filename;
            foreach (QListWidgetItem* item, item_list) {
                temp_filename = item->data(Qt::UserRole).toString();
                QImage image(temp_filename);
                image.save(filename);
                break;
            }
        }
        QString dir = fi.absolutePath();
        qint64 prefer_size = QFile(filename).size();
        if(is_disk_no_space(dir.toLatin1().constData() ,prefer_size/(1024*1024) + 100)){
            SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_Operation_cannot_be_carried_out_due_to_insufficient_memory_or_hard_disk_space_Please_try_again_after_freeing_memory_or_hard_disk_space_"));
            msgWarm->setWindowTitle(tr("ResStr_Warning"));
            msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                    & ~Qt::WindowMinimizeButtonHint);
            msgWarm->exec();
            return;
        }
        if(QFile::exists(savefilename)){
            QFile::remove(savefilename);
        }
        QFile::copy(filename ,savefilename);
        QFile::remove(filename);
    }
}

void TabStackedWidget::on_copyNum_textChanged(const QString &arg1)
{
//    qDebug()<<arg1;
    if(arg1.toInt() < 1)
    {
        ui->copyNum->setText("1");
        SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_The_valid_range_is_1_99__please_confirm_and_enter_again_"));
        msgWarm->setWindowTitle(tr("ResStr_Error"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
    }
    else if(arg1.toInt() > 99)
    {
        ui->copyNum->setText("99");
        SettingWarming *msgWarm  = new SettingWarming(this, tr("ResStr_The_valid_range_is_1_99__please_confirm_and_enter_again_"));
        msgWarm->setWindowTitle(tr("ResStr_Error"));
        msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                                & ~Qt::WindowMinimizeButtonHint);
        msgWarm->exec();
    }
}

void TabStackedWidget::on_btn_ScanCancel_clicked()
{
    gUInterface->cancel_work();
}

void TabStackedWidget::on_TUSBBtn_3_clicked()
{
    QString lan = QLocale::system().name();
    QString helpPath;
    if(lan == "en_US")
    {
        helpPath = APP_PATH + QString("html/HelpFile/Help/English/USB.htm");
    }else if (lan == "zh_CN")
    {
        helpPath = APP_PATH + QString("html/HelpFile/Help/SimplifiedChinese/USB.htm");
    }
//    qDebug()<<helpPath;
    QDesktopServices::openUrl(QUrl(helpPath));
}

void TabStackedWidget::on_TWiFiBtn_3_clicked()
{
    QString lan = QLocale::system().name();
    QString helpPath;
    if(lan == "en_US")
    {
        helpPath = APP_PATH + QString("html/HelpFile/Help/English/Wi-Fi.htm");
    }else if (lan == "zh_CN")
    {
        helpPath = APP_PATH + QString("html/HelpFile/Help/SimplifiedChinese/Wi-Fi.htm");
    }
    QDesktopServices::openUrl(QUrl(helpPath));
}

void TabStackedWidget::onTimerCopyNumOut()
{
    if(timerCount > 9)
        on_btn_CopyNumReduce_clicked();
    timerCount++;
}

void TabStackedWidget::onTimerCopyNumOut1()
{
    if(timerCount > 9)
        on_btn_CopyNumAdd_clicked();
    timerCount++;
}

void TabStackedWidget::on_btn_CopyNumReduce_pressed()
{
    timerCount = 0;
    connect(timerCopyNum,SIGNAL(timeout()),this,SLOT(onTimerCopyNumOut()));
    timerCopyNum->start(60);
}

void TabStackedWidget::on_btn_CopyNumReduce_released()
{
    timerCopyNum->stop();
    disconnect(timerCopyNum,SIGNAL(timeout()),this,SLOT(onTimerCopyNumOut()));
}

void TabStackedWidget::on_btn_CopyNumAdd_pressed()
{
    timerCount = 0;
    connect(timerCopyNum,SIGNAL(timeout()),this,SLOT(onTimerCopyNumOut1()));
    timerCopyNum->start(60);
}

void TabStackedWidget::on_btn_CopyNumAdd_released()
{
    timerCopyNum->stop();
    disconnect(timerCopyNum,SIGNAL(timeout()),this,SLOT(onTimerCopyNumOut1()));
}
