#ifndef TABSTACKEDWIDGET_H
#define TABSTACKEDWIDGET_H

#include <QStackedWidget>
#include "settingsstackedwidget.h"
#include "moresettingsforscan.h"
#include "moresettingsforcopy.h"
#include "uinterface.h"
#include <qtimer.h>

namespace Ui {
class TabStackedWidget;
}

class TabStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit TabStackedWidget(QWidget *parent = 0);
    ~TabStackedWidget();

public slots:
    void on_btn_WiFi_clicked();
    void initWiFi_clicked();
    void on_btn_PowerSave_clicked();
    void on_btn_TCPIPV4_clicked();
    bool getScrollAreaImageStatus();
    void set_copy_enabled(bool enabled);
    void set_scan_enabled(bool enabled);
    void set_setting_enabled(bool enabled);
    void setCopyStackedWidgetCurrentIndex(int index);

    void setEnabledDuplexCopy(bool enabled);
    void setEnabledCopyScan(bool enabled);
    void setEnabledWifi(bool enabled);
    void setEnabledNet(bool enabled);

    void recoverCopyMode();
    void setDefault_Copy(bool isExceptTips = false);

    void setDefault_Scan();

    void setTitelCellEnabled(bool enabled);


private slots:
    void on_cBox_IsIDCard_clicked(bool checked);

    void on_cBox_DuplexCopy_clicked(bool checked);

    void on_btn_SoftAP_clicked();

    void on_btn_TCPIPV6_clicked();

    void on_btn_UserConfig_clicked();

    void on_btn_Password_clicked();

    void on_btn_Scan_clicked();

    void on_scrollArea_ScanImage_itemSelectionChanged();

    void on_btn_MoreSetting_Scan_clicked();

    void on_btn_MoreSetting_Copy_clicked();

    void on_btn_DensityAdd_clicked();

    void on_btn_DensityReduce_clicked();

    void on_btn_CopyNumAdd_clicked();

    void on_btn_CopyNumReduce_clicked();

//    void startCycleEmit();
//    void stopCycleEmit();

    void on_btn_Copy_clicked();

    void cmdResult(int,int,QVariant);
    void updateScanProcess(int);

    void on_btn_ScanSave_clicked();

    void on_copyNum_textChanged(const QString &arg1);


    void on_btn_ScanCancel_clicked();
    void slots_scan_image_size(float, int);

    void on_TUSBBtn_3_clicked();

    void on_TWiFiBtn_3_clicked();

    void on_btn_CopyNumReduce_pressed();

    void on_btn_CopyNumReduce_released();

    void on_btn_CopyNumAdd_pressed();

    void on_btn_CopyNumAdd_released();

    void onTimerCopyNumOut();

    void onTimerCopyNumOut1();

    void timerItemClick();


private:
    Ui::TabStackedWidget *ui;

    QString selectState;
    QString selectHover;
    QString selectPressed;

    QString unSelectState;
    QString unSelectHover;
    QString unSelectPressed;

    int value_Density;

    QStringList filenamelist;

    Param_Copy paramCopy;

    UiSettings paramScan;

    QTimer *timerCopyNum;

    int timerCount;

    QTimer *timerClick;

    bool scanning;
    UIConfig::EnumMachineJob  m_oldJob;
//signals:
//    void cycleStartFromTab();
//    void cycleStopFromTab();
};

#endif // TABSTACKEDWIDGET_H
