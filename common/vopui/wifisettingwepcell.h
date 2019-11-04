#ifndef WIFISETTINGWEPCELL_H
#define WIFISETTINGWEPCELL_H

#include <QStackedWidget>
#include "typedefine.h"
#include "qvariant.h"


namespace Ui {
class WiFiSettingWEPCell;
}

class WiFiSettingWEPCell : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WiFiSettingWEPCell(QWidget *parent = 0, APInfo *info = 0, bool *_islogin = 0, bool isconnected = false);
    ~WiFiSettingWEPCell();

    APInfo getAPInfo();
    void tryConnect(APInfo);
    void setWifiEnabled(bool enabled);

signals:
    void SizeChanged(QSize , QSize );
    void doingConnect(QWidget*);
    void connectSuc(QWidget*, bool);

private slots:
        void on_btCancel_clicked();
        void on_pushButton_clicked();
        void on_pushButton_2_clicked();
        void on_checkBox_visiable_clicked();
        void initStatus();

        void on_btKey1_toggled(bool checked);

        void on_btKey2_toggled(bool checked);

        void on_btKey3_toggled(bool checked);

        void on_btKey4_toggled(bool checked);

        void on_btConnect_clicked();

        void changeStatus();
        void cmdResult(int,int,QVariant);

private:
    Ui::WiFiSettingWEPCell *ui;
    APInfo apInfo;
    APInfo pInfo;
    bool isDisplayPW;
    bool *islogin;
    bool isConnected;
    bool isDoingCMD;
    bool m_isLogin;
    int times;
};

#endif // WIFISETTINGWEPCELL_H
