#ifndef WLANTITLECELL_H
#define WLANTITLECELL_H

#include <QStackedWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QObjectList>
#include "typedefine.h"
#include <QTimer>
#include <QLabel>
#include <QList>
#include "busyrefreshlabel.h"
#include "typedefine.h"
#include "uinterface.h"
#include "qvariant.h"
#include "wifisettingwepcell.h"
#include "wifisettingcell.h"
#include <qscrollbar.h>

//#define DEBUG

#ifdef DEBUG
struct cmdst_aplistaa
{
    QString ssid;
    int encryption;
};

#define NUM_OF_APLIST 10
typedef struct cmdst_aplist_getaa
{
    struct cmdst_aplistaa  aplist[NUM_OF_APLIST];
}
    cmdst_aplist_getaa;

typedef struct cmdst_wifi_getaa
{
    UINT8 wifiEnable ; // bit0: Wi-Fi Enable, bit1: GO Enable, bit2: P2P Enable
    UINT8 sigLevel   ; //
    UINT8 reserved0  ; //
    UINT8 netType    ; // lenovo always 0
    UINT8 encryption ; // 0:No Security 1:WEP 64/128 bit 2.WPA-PSK-TKIP  3. WPA2-PSK-AES 4.Mixed Mode PSK
    UINT8 wepKeyId   ; //
    UINT8 reserved1  ; //
    UINT8 channel    ; //
    QString  ssid   ; // used by both Legacy WiFi SSID and Wi-Fi Direct GO SSID
    QString  pwd    ; // used by both Legacy WiFi Passphrase & WEPKey and Wi-Fi Direct GO Passphrase
    char  pinCode[8] ; //
    UINT8 reserved2[64]; //
    UINT8 ipAddr[4]  ; //
}cmdst_wifi_getaa;
#endif

namespace Ui {
class WlanTitleCell;
}

class WlanTitleCell : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WlanTitleCell(QWidget *parent = 0,QScrollBar *_scrollBar = 0, bool wlanON=true, bool *_islogin = 0);
    ~WlanTitleCell();

//    BusyRefreshLabel *cycle;
    APInfo *currentAp;
    void emptyWifiList();
    void setWifiEnabled(bool enabled);

public slots:
    void updateAP();
    void on_btFlesh_clicked();

private slots:
    void on_btManualWiFi_clicked();
    void on_btCancel_clicked();
    void on_btWLANON1_clicked();
    void on_btWLANON2_clicked();

    void getSizeChanged(QSize, QSize);
    void addCell(QString ssid, EncrypType type, APInfo info, bool isConnected);
#ifndef DEBUG
    void initCell(cmdst_wifi_get wifi_para, cmdst_aplist_get aplist);
#else
    void initCell(cmdst_wifi_getaa wifi_para, cmdst_aplist_getaa aplist);
#endif
    void onTimeout();

    void getConnectAction(QWidget*);
    void getConnectResult(QWidget*, bool);

    void on_combox_encryption_currentIndexChanged(int index);

    void on_btConnect_clicked();

    void on_btKey1_toggled(bool checked);

    void on_btKey2_toggled(bool checked);

    void on_btKey3_toggled(bool checked);

    void on_btKey4_toggled(bool checked);

    int checkSSID(QString SSID);

    void on_checkBox_visiable_toggled(bool checked);

    void cmdResult(int,int,QVariant);

signals:
    void statusChange();
//    void tryToConnect(APInfo);

private:
    Ui::WlanTitleCell *ui;

    WiFiSettingWEPCell *wifiWepCell;
    WiFiSettingCell *wifiCell;

    QWidget *widget;
    QSize currentSize;
    QVBoxLayout pageLayout;

    QList<QWidget *> apList;

    QTimer *timer1;
    QScrollBar *pScrollBar;
    int oldScrollValue;

    QList<APInfo> aList;

    bool isWlanOn;
    bool isManual;

    int cycleCount;
    int currentAPID;

    cmdst_wifi_get wifi_para;
    cmdst_wifi_get orin_wifi_para;
    bool *islogin;
    bool isWitch;
    bool is_wifi_now_on;
    bool m_isLogin;

//signals:
//    void cycleStartFromWT();
//    void cycleStopFromWT();
};

#endif // WLANTITLECELL_H
