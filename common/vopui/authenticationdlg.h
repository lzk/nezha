#ifndef AUTHENTICATIONDLG_H
#define AUTHENTICATIONDLG_H

#include <QDialog>
#include "busyrefreshlabel.h"
#include "qvariant.h"

namespace Ui {
class AuthenticationDlg;
}

class AuthenticationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenticationDlg(QWidget *parent = 0, bool *confirm = 0);
    ~AuthenticationDlg();

private slots:

    void on_lineEdit_Password_textEdited(const QString &arg1);

    void on_btApply_Login_clicked();

    void cmdResult(int,int,QVariant);

private:
    Ui::AuthenticationDlg *ui;
    bool *isconfirm;

    BusyRefreshLabel *cycle;

    bool isDoingCMD;
    int  times;
};

#endif // AUTHENTICATIONDLG_H
