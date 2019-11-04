#ifndef SETTINGSSTACKEDWIDGET_H
#define SETTINGSSTACKEDWIDGET_H

#include <QStackedWidget>
#include "wlantitlecell.h"
#include <qscrollarea.h>
#include <qtimer.h>
#include "settingforipv6.h"
#include "ipv6status.h"
#include <qbuttongroup.h>

namespace Ui {
class SettingsStackedWidget;
}

enum Press_Btn {TimeoutSetting=0,TopMargin,LeftMargin,ImageDensity};

class SettingsStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit SettingsStackedWidget(QWidget *parent = 0);
    ~SettingsStackedWidget();

    WlanTitleCell *titelCell;
    void changeStackIndex(int index);
    void setWifiEnabled(bool enabled);
    void setNetEnabled(bool enabled);

protected slots:
    bool eventFilter(QObject *, QEvent *);
    void hideEvent(QHideEvent *);


private:
    Ui::SettingsStackedWidget *ui;
    SettingForIPv6 *settingIPv6Widget;
    IPv6Status *ipv6Status;

    QScrollArea *scrollArea;

    QTimer *timer1;
    bool isAPOpen;
    bool isWLANON;
    bool isLogn;

    bool isLowHumidity;
    bool isConrtolM;
    bool isCoolingM;

    int pressBtn;
    int timercounter;

    net_info_st info_ipv4;
    net_info_st info_ipv4_orn;
    net_ipv6_st info_ipv6;
    cmdst_PSave_time psavetm;

    cmdst_softap softap;
    cmdst_userconfig userconfig;
    cmdst_userconfig userconfig_orn;
    int old_dev_status;

    QButtonGroup *buttonGroupIP;
    QButtonGroup *buttonGroupDNS;

private slots:
    void on_btAPOpen_clicked();

    void initAP();
    void initIP();
    void initPowerSave();
    void initPassword();
    void cmdResult(int,int,QVariant);
    void initAdvanceSetting(cmdst_userconfig config, bool isFiest);


    void on_btTimeReduce_clicked();
    void on_btTimeAdd_clicked();
    void onTimer1Timeout_Add();
    void onTimer1Timeout_Reduce();
    void on_btApply_AP_clicked();
    void onbtDHCPtoggled(bool checked);
    void onbtStatictoggled(bool checked);
    void onbtAutotoggled(bool checked);
    void onbtManualtoggled(bool checked);
    void onRadioClickIP(bool checked);
    void onRadioClickDNS(bool checked);

    void on_btApply_IPConfig_clicked();
    void on_btIPv6Setting_clicked();
    void on_btIPv6Status_clicked();

    void on_lineEdit_IPAddressv4_textEdited(const QString &arg1);
    void on_lineEdit_Gatewayv4_textEdited(const QString &arg1);
    void on_lineEdit_Submaskv4_textEdited(const QString &arg1);
    void on_lineEdit_server_textEdited(const QString &arg1);

    void on_btApply_Timeout_clicked();

    void on_lineEdit_timeout_textEdited(const QString &arg1);

    void on_lineEdit_TopMargin_textEdited(const QString &arg1);

    void on_lineEdit_LeftMargin_textEdited(const QString &arg1);

    void on_lineEdit_ImageDensity_textEdited(const QString &arg1);

    void on_btApply_userconfig_clicked();


    void on_btDensityAdd_clicked();

    void on_btDensityReduce_clicked();

    void on_btDrumReset_clicked();

    void on_btErrorClear_clicked();

    void on_btLeftAdd_clicked();

    void on_btLeftReduce_clicked();

    void on_btLowHumidity_clicked();

    void on_btTonerReset_clicked();

    void on_btTopAdd_clicked();

    void on_btTopReduce_clicked();

    void on_btApply_clicked();

    void on_btTimeReduce_pressed();
    void on_btTimeReduce_released();
    void on_btTimeAdd_pressed();
    void on_btTimeAdd_released();
    void on_btTopReduce_pressed();
    void on_btTopReduce_released();
    void on_btTopAdd_released();
    void on_btTopAdd_pressed();
    void on_btLeftReduce_pressed();
    void on_btLeftReduce_released();
    void on_btLeftAdd_pressed();
    void on_btLeftAdd_released();
    void on_btDensityReduce_pressed();
    void on_btDensityReduce_released();
    void on_btDensityAdd_pressed();
    void on_btDensityAdd_released();
};

#endif // SETTINGSSTACKEDWIDGET_H
