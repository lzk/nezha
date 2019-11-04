#ifndef WIFISETTINGCELL_H
#define WIFISETTINGCELL_H

#include <QStackedWidget>
#include "typedefine.h"
#include "qvariant.h"

namespace Ui {
class WiFiSettingCell;
}

class WiFiSettingCell : public QStackedWidget
{
    Q_OBJECT

public:
    explicit WiFiSettingCell(QWidget *parent = 0, APInfo *info = 0, bool *_islogin = 0, bool isconnected = false);
    ~WiFiSettingCell();
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

        void changeStatus();
        void on_btConnect_clicked();
        void cmdResult(int,int,QVariant);

private:
    Ui::WiFiSettingCell *ui;
    APInfo apInfo;
    bool isDisplayPW;
    bool *islogin;
    bool m_isLogin;
    bool isConnected;
};

#endif // WIFISETTINGCELL_H
