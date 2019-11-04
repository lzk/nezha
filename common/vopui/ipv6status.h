#ifndef IPV6STATUS_H
#define IPV6STATUS_H

#include <QDialog>
#include "busyrefreshlabel.h"
#include "uinterface.h"

namespace Ui {
class IPv6Status;
}

class IPv6Status : public QDialog
{
    Q_OBJECT

public:
    explicit IPv6Status(QWidget *parent = 0);
    ~IPv6Status();

private slots:

    void on_btReturn_clicked();
    void cmdResult(int cmd,int result,QVariant data);
    void showInfo(net_ipv6_st info);

private:
    Ui::IPv6Status *ui;
    net_ipv6_st info_ipv6;
    BusyRefreshLabel *cycle;
    bool isDoingCMD;
    int times;
};

#endif // IPV6STATUS_H
