#include "ipv6status.h"
#include "ui_ipv6status.h"

#define RETRYTIMES 3;

IPv6Status::IPv6Status(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IPv6Status)
{
    ui->setupUi(this);

    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)), this ,SLOT(cmdResult(int,int,QVariant)));

    cycle = new BusyRefreshLabel(this,true);
    cycle->setGeometry(QRect(235,100,50,50));
    cycle->startAnimation(20);
    gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_NET_GetV6);
    isDoingCMD = false;
    times = 0;
}

IPv6Status::~IPv6Status()
{
    delete ui;
}

void IPv6Status::on_btReturn_clicked()
{
    close();
}

void IPv6Status::showInfo(net_ipv6_st info)
{
    ui->label_8->setText(QString(info.StatelessAddress1));
    ui->label_9->setText(QString(info.StatelessAddress2));
    ui->label_10->setText(QString(info.StatelessAddress3));
    ui->label_11->setText(QString(info.AutoStatefulAddress));
    ui->label_12->setText(QString(info.LinkLocalAddress));
    ui->label_13->setText(QString(info.AutoGatewayAddress));
}

void IPv6Status::cmdResult(int cmd,int result,QVariant data)
{
    switch(cmd)
    {
    case UIConfig::LS_CMD_NET_GetV6:
        if(!result)
        {
            info_ipv6 = data.value<net_ipv6_st>();
            showInfo(info_ipv6);
            LOGLOG("get ipv6 information");
            isDoingCMD = false;
            times = 0;
        }
        else {
            if(!isDoingCMD){
                isDoingCMD = true;
                times = RETRYTIMES;
            }
            if(times > 0) {
                times--;
                gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_NET_GetV6);
            }
            else{
                isDoingCMD = false;
                times = 0;
            }
        }
        if(!isDoingCMD)
            cycle->stopAnimation();
        break;
    default: break;
    }
}
