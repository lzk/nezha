#include "authenticationdlg.h"
#include "ui_authenticationdlg.h"
#include "uinterface.h"
#include "qdebug.h"
#define RETRYTIMES 3

AuthenticationDlg::AuthenticationDlg(QWidget *parent, bool*confirm) :
    QDialog(parent),
    ui(new Ui::AuthenticationDlg)
{
    ui->setupUi(this);

    isconfirm = confirm;
    QRegExp rx("^[0-9a-zA-Z]{0,32}$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_Password->setValidator(validator);
    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));

    cycle = new BusyRefreshLabel(this,true);
    cycle->setGeometry(QRect(180,70,50,50));
    ui->label->hide();
}

AuthenticationDlg::~AuthenticationDlg()
{
    delete ui;
}

void AuthenticationDlg::on_lineEdit_Password_textEdited(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui->btApply_Login->setEnabled(false);
    }
    else
    {
        ui->btApply_Login->setEnabled(true);
    }
    ui->label_msg->setText(tr("ResStr_Password_Tip"));
    ui->label_msg->setStyleSheet("QLabel{color:black;}");
}

void AuthenticationDlg::on_btApply_Login_clicked()
{
    if(ui->lineEdit_Password->text() != NULL)
    {
        cmdst_passwd passwd;
        memset(passwd.passwd ,0 ,sizeof(passwd.passwd));
        strcpy(passwd.passwd,ui->lineEdit_Password->text().toLatin1().constData());
        QVariant data;
        data.setValue<cmdst_passwd>(passwd);
        gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PASSWD_confirm,data);

        cycle->startAnimation(20);
        ui->label->show();
    }

}

void AuthenticationDlg::cmdResult(int cmd,int result ,QVariant data)
{
    if(UIConfig::LS_CMD_PASSWD_confirm == cmd)
    {
        if(!result)
        {
            *isconfirm = true;
            close();
        }
        else
        {
            *isconfirm = false;
            ui->label_msg->setText(tr("ResStr_Authentication_error__please_enter_the_password_again_"));
            ui->label_msg->setStyleSheet("QLabel{color:red;}");
        }
        cycle->stopAnimation();
        ui->label->hide();
    }
}
