#ifndef MEMBERCENTERWIDGET_H
#define MEMBERCENTERWIDGET_H

#include <QWidget>
#include "qpushbutton.h"
#include "qstackedwidget.h"
#include <QtNetwork/QtNetwork>

namespace Ui {
class MemberCenterWidget;
}

class MemberCenterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MemberCenterWidget(QWidget *parent = 0);
    ~MemberCenterWidget();
    QString loginPhone;

public slots:
    void setSW(QStackedWidget* _sw, QPushButton * _bt);
    void startCRM();

private slots:
    void on_btLogin_clicked();

    void on_btAbout_clicked();

    void on_btCancel_clicked();

    void on_btChInfo_clicked();

    void on_btExpe_clicked();

    void on_pushButton_clicked();

//    void getUserInfo();

    void initUserInfo(QString userInfo);

    QString getHostMacAddress();

    void setUserInfo(QString fullName, QString birthDay, int sex, QString email, QString address);

    void replyFinish_set(QNetworkReply* reply);
    void replyFinish_uploadCRM(QNetworkReply* reply);

    void on_btApply_clicked();

    void on_btProduct_clicked();

    void uploadCRM();

    void cmdResult(int cmd,int result,QVariant data);

    void on_le_name_textEdited(const QString &arg1);

    void on_le_mail_textEdited(const QString &arg1);

    void on_le_addr_textEdited(const QString &arg1);

private:
    Ui::MemberCenterWidget *ui;

    QStackedWidget* sw;
    QPushButton *bt;
    bool isLogin;
    QString m_strKey;

    QTimer *crmTimer;
    bool isNotShowWarning;
    bool m_bCRM;
};

#endif // MEMBERCENTERWIDGET_H
