#include "userlogin.h"
#include "ui_userlogin.h"
#include "qdatetime.h"
#include "qurl.h"
#include "jsonparser/parser.h"
#include "qeventloop.h"
#include "requestcrm.h"
#include "log.h"
extern const char* g_config_file;
UserLogin::UserLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    m_strKey = "86c02972fba047b0b0a9adb8123029fb";
    m_loginSuccess = false;
    acTimer = new QTimer(this);
    connect(acTimer,SIGNAL(timeout()),this,SLOT(showTimelimit()));

    QRegExp regexp("^[0-9]+$");
    QRegExpValidator *pReg = new QRegExpValidator(regexp,this);
    ui->le_userName->setValidator(pReg);
}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::on_bt_login_clicked()
{
//    qDebug()<<"on_bt_login_clicked";
    if(ui->le_userName->text() == "")
    {
        return;
    }

    QString strPhoneNumber = ui->le_userName->text();
    QString strVerifyCode = ui->le_autoCode->text();

    ui->bt_login->setEnabled(false);
    ui->labMsg->setText("");
    loginAction(strPhoneNumber,strVerifyCode);
}

void UserLogin::loginAction(QString strPhoneNumber,QString strVerifyCode)
{
#if NEW_CRM
    if(!RequestCRM::verify_mobilecode(strPhoneNumber,strVerifyCode)){
        LOGLOG("crm verify ok");
        m_loginSuccess = true;
        QSettings settings(g_config_file,QSettings::NativeFormat);
        settings.setValue("loginPhone",strPhoneNumber);
        settings.setValue("password",strVerifyCode);
        close();
    }else{
        ui->labMsg->setText(tr("ResStr_Msg_8"));
        ui->bt_login->setEnabled(true);
    }
#else
    QString baseUrl = "http://function.iprintworks.cn:8001/smsauth/authCode.php";
    QUrl url(baseUrl);
    QString post_str = QString("phoneNum=%0&authCode=%1").arg(strPhoneNumber).arg(strVerifyCode);
#if QT_VERSION > 0x050000
    QByteArray post_data = post_str.toLocal8Bit();
#else
    QByteArray post_data = post_str.toAscii();
#endif

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_check(QNetworkReply*)));

    QNetworkRequest req;

    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

    QNetworkReply *reply = manager->post(req,post_data);

    QEventLoop eventloop;
    connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    QTimer::singleShot(3*1000,&eventloop,SLOT(quit()));
    eventloop.exec();

    QString strJsonText = reply->readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(strJsonText.toUtf8(),&ok).toMap();

    if(ok)
    {
        if(result["success"].toBool())
        {
            m_loginSuccess = true;
            QSettings settings(g_config_file,QSettings::NativeFormat);
            settings.setValue("loginPhone",strPhoneNumber);
            settings.setValue("password",strVerifyCode);

            QString userInfo = getUserInfo(strPhoneNumber);
//            qDebug()<<userInfo;
            ok = false;
            QVariantMap result = parser.parse(userInfo.toUtf8(),&ok).toMap();
            if (result["success"].toBool())
            {
                QVariantMap user = result["user"].toMap();

                if(user["realName"].toString() != NULL)
                {
                    settings.beginGroup(strPhoneNumber);
                    settings.setValue("loginName",user["realName"].toString());
                    settings.endGroup();
                }
            }
            close();
        }
        else
        {
            ui->labMsg->setText(tr("ResStr_Msg_8"));
            ui->bt_login->setEnabled(true);
        }
    }
    else
    {
        ui->labMsg->setText(tr("ResStr_Msg_8"));
        ui->bt_login->setEnabled(true);
    }
    reply->deleteLater();
#endif
}

QString UserLogin::getUserInfo(QString strPhoneNumber)
{
    QString baseUrl = "http://crm.iprintworks.cn/api/app_getuserinfo";
    QUrl url(baseUrl);

    QDateTime dateTime;
    QString time = dateTime.currentDateTime().toString("yyyyMMddHHmmss");

    QString text = strPhoneNumber;

    QString str = QString("%0%1%2").arg(text).arg(time).arg(m_strKey);
//    qDebug()<<str;

    QString md5;
    QByteArray bb;
#if QT_VERSION > 0x050000
    bb = QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
#else
    bb = QCryptographicHash::hash(str.toAscii(),QCryptographicHash::Md5);
#endif
    md5.append(bb.toHex());

    QByteArray post_data;
    QString post_str = QString("mobile=%0&time=%1&Sign=%2").arg(text).arg(time).arg(md5);
//    qDebug()<<post_str;
#if QT_VERSION > 0x050000
    post_data = post_str.toLocal8Bit();
#else
    post_data = post_str.toAscii();
#endif

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_get(QNetworkReply*)));

    QNetworkRequest req;

    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

    QNetworkReply *reply = manager->post(req,post_data);

    QEventLoop eventloop;
    connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    QTimer::singleShot(3*1000,&eventloop,SLOT(quit()));
    eventloop.exec();

    QString strJsonText = reply->readAll();
    reply->deleteLater();

    return strJsonText;
}

void UserLogin::on_bt_getAuthCode_clicked()
{
#if NEW_CRM
    QString strPhoneNumber = ui->le_userName->text();
    if(strPhoneNumber.length() == 11)
    {
        if(!RequestCRM::send_verifycode(ui->le_userName->text())){
            ui->labMsg->setText(tr("ResStr_Msg_5"));
            acTime = 60;
            QString num = QString::number(acTime);
            ui->bt_getAuthCode->setText(num);
            ui->bt_getAuthCode->setEnabled(false);
            acTimer->start(1000);
            return;
        }
        ui->labMsg->setText(tr("ResStr_Msg_6"));
    }else{
        ui->labMsg->setText(tr("ResStr_Msg_7"));
    }
#else
//    qDebug()<<"on_bt_getAuthCode_clicked";
    QString baseUrl = "http://function.iprintworks.cn:8001/smsauth/mt_u.php";
    QUrl url(baseUrl);

    QString strPhoneNumber = ui->le_userName->text();

    QString str = QString("phoneNum=%0").arg(strPhoneNumber);
//    qDebug()<<str;
#if QT_VERSION > 0x050000
    QByteArray post_data = str.toLocal8Bit();
#else
    QByteArray post_data = str.toAscii();
#endif
    post_data = str.toUtf8();
//    qDebug()<<post_data;
    if(strPhoneNumber.length() == 11)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_send(QNetworkReply*)));

        QNetworkRequest req;

        req.setUrl(url);

        req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
        req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

        manager->post(req,post_data);
    }
    else
    {
        ui->labMsg->setText(tr("ResStr_Msg_7"));
    }
#endif
}

void UserLogin::showTimelimit()
{
    if(acTime != 0)
    {
        acTime--;
        QString num = QString::number(acTime);
        ui->bt_getAuthCode->setText(num);
    }
    else
    {
        ui->bt_getAuthCode->setEnabled(true);
        ui->bt_getAuthCode->setText(tr("ResStr_Get_Verification_Code"));
        acTimer->stop();
    }
}

void UserLogin::replyFinish_send(QNetworkReply* reply)
{
    QString strJsonText = reply->readAll();
//    qDebug()<<"replyFinish_send"<<strJsonText;

    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(strJsonText.toUtf8(),&ok).toMap();

    if(ok)
    {
        if(result["success"].toBool())
        {
            ui->labMsg->setText(tr("ResStr_Msg_5"));
            acTime = 60;
            QString num = QString::number(acTime);
            ui->bt_getAuthCode->setText(num);
            ui->bt_getAuthCode->setEnabled(false);
            acTimer->start(1000);
            return;
        }
    }

    ui->labMsg->setText(tr("ResStr_Msg_6"));
    reply->deleteLater();
}

//QString UserLogin::getPhone()
//{
//    QString phone = ui->le_userName->text();
//    return phone;
//}

bool UserLogin::isLogin()
{
    return m_loginSuccess;
}
