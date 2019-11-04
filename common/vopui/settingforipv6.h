#ifndef SETTINGFORIPV6_H
#define SETTINGFORIPV6_H

#include <QDialog>
#include "busyrefreshlabel.h"

#include "uinterface.h"

namespace Ui {
class SettingForIPv6;
}

class SettingForIPv6 : public QDialog
{
    Q_OBJECT

public:
    explicit SettingForIPv6(QWidget *parent = 0, bool *_isLogin = 0);
    ~SettingForIPv6();

protected slots:
    bool eventFilter(QObject *, QEvent *);

private slots:

    void on_btCancel_clicked();

    void on_checkBox_isManualAddress_toggled(bool checked);

    void on_btApply_clicked();

//    void on_lineEdit_address_text/Edited(const QString &arg1);

//    void on_lineEdit_GateAddress_textEdited(const QString &arg1);

//    void on_lineEdit_SubMask_textEdited(const QString &arg1);

    void cmdResult(int cmd,int result,QVariant data);

    void showInfo(net_ipv6_st info);

    int is_valid_ipv6(const char *ipv6);

private:
    Ui::SettingForIPv6 *ui;
    bool *islogin;
    BusyRefreshLabel *cycle;
    net_ipv6_st info_ipv6;
    bool isDoingCMD;
    int times;
};

#endif // SETTINGFORIPV6_H
