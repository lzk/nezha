#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QDialog>
#include <QtNetwork/QtNetwork>
#include <qtimer.h>

namespace Ui {
class UserLogin;
}

class UserLogin : public QDialog
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();

public slots:
//    QString getPhone();
    bool isLogin();
    void loginAction(QString strPhoneNumber,QString strVerifyCode);
    QString getUserInfo(QString strPhoneNumber);

private slots:
    void on_bt_login_clicked();

    void on_bt_getAuthCode_clicked();

    void replyFinish_send(QNetworkReply* reply);

//    void replyFinish_check(QNetworkReply* reply);

    void showTimelimit();

private:
    Ui::UserLogin *ui;

    QString m_strKey;
    bool m_loginSuccess;

    QTimer *acTimer;
    int acTime;
};

#endif // USERLOGIN_H
