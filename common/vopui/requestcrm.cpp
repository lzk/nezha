#include "requestcrm.h"
#include <QString>
#include <QNetworkAccessManager>
#include <QCryptographicHash>
#include <QDateTime>
#include <QEventLoop>
#include <QTimer>
#include <QNetworkReply>
#include "jsonparser/parser.h"
#include "log.h"
static const QString& base_uri = "http://vopapi.lenovoimage.com/api";
static QString request_token;

RequestCRM::RequestCRM()
{

}

QString RequestCRM::get(const QString& url ,const QString& post_str)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
//    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_get(QNetworkReply*)));

    QNetworkRequest req;

    req.setUrl(url + "?" + post_str);

//    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
//    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    if(!request_token.isEmpty()){
        req.setRawHeader(QByteArray("Authorization"),request_token.toLatin1());
    }

    QNetworkReply *reply = manager->get(req);

    QEventLoop eventloop;
    QObject::connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    QTimer::singleShot(10*1000,&eventloop,SLOT(quit()));
    eventloop.exec();

    QString str = reply->readAll();
    reply->deleteLater();
    manager->deleteLater();
    LOGLOG("reply str:%s" ,str.toUtf8().constData());
    return str;
}

QString RequestCRM::post(const QString& url ,const QString& post_str)
{
    QByteArray post_data;
//#if QT_VERSION > 0x050000
//    post_data = post_str.toLocal8Bit();
//#else
//    post_data = post_str.toAscii();
//#endif
    post_data = post_str.toUtf8();

    QNetworkAccessManager mm;
    QNetworkAccessManager *manager = &mm;
//    QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(post(QNetworkReply*)));

    QNetworkRequest req;

    req.setUrl(url + "?" + post_str);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json; charset=UTF-8");
//    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
//    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());
    if(!request_token.isEmpty()){
        req.setRawHeader(QByteArray("Authorization"),request_token.toLatin1());
    }
    QNetworkReply *reply = manager->post(req ,post_data);

    QEventLoop eventloop;
    QObject::connect(reply,SIGNAL(finished()),&eventloop,SLOT(quit()));
    QTimer::singleShot(10*1000,&eventloop,SLOT(quit()));
    eventloop.exec();

    QString str = reply->readAll();
    reply->deleteLater();
//    manager->deleteLater();
    LOGLOG("reply str:%s" ,str.toUtf8().constData());
    return str;
}

QString RequestCRM::get_tocken()
{
    QString url = base_uri + "/Login/Get_Token";

    const QString apikey ="431986";
    const QString apisecret = "1234567890";
    QString timestamp = "20190301";
    QString strsign = "";

    timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    QString str = apisecret +
            "apikey" + apikey
            +"timestamp" + timestamp
            + apisecret;

    QByteArray bb;
#if QT_VERSION > 0x050000
    bb = QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
#else
    bb = QCryptographicHash::hash(str.toAscii(),QCryptographicHash::Md5);
#endif
    strsign = QString(bb.toHex()).toUpper();

    QString post_str = QString("apikey=%0&sign=%1&timeStamp=%2").arg(apikey).arg(strsign).arg(timestamp);

    QString ret_data = get(url ,post_str);
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(ret_data.toUtf8(),&ok).toMap();

    QString token;
    if(ok)
    {
        if(!result["msg"].toString().compare("success"))
        {
            QVariantMap data = result["data"].toMap();
            token = data["Token"].toString();
        }
    }
    return token;
}

int RequestCRM::verify_mobilecode(const QString& phonenumber ,const QString& verifycode)
{
    if(request_token.isEmpty()){
        request_token = get_tocken();
    }
    if(request_token.isEmpty()){
        return -1;
    }

    QString url = base_uri + "/Login/Verify_MobileCode";

    QString post_str = QString() +
            "biz_content=" +
            "{" +
            "Mobile:" + phonenumber + "," +
            "ValidCode:" + verifycode +
            "}";

    QString ret_data = post(url ,post_str);
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(ret_data.toUtf8(),&ok).toMap();

    int ret = -1;
    if(ok)
    {
        if(!result["msg"].toString().compare("success"))
        {
            QVariantMap data = result["data"].toMap();
            ret = data["status"].toInt();
        }
    }
    if(ret)
        request_token.clear();
    return ret;
}

//curl -d "biz_content={Mobile:15918656240}" http://vopapi.lenovoimage.com/api/Login/Send_VerifyCode?biz_content={Mobile:15918656240}
int RequestCRM::send_verifycode(const QString& phonenumber)
{
    if(request_token.isEmpty()){
        request_token = get_tocken();
    }
    if(request_token.isEmpty()){
        return -1;
    }

    QString url = base_uri + "/Login/Send_VerifyCode";
    QString post_str = QString() +
            "biz_content=" +
            "{" +
            "Mobile:" + phonenumber +
            "}";

    QString ret_data = post(url ,post_str);
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(ret_data.toUtf8(),&ok).toMap();

    int ret = -1;
    if(ok)
    {
        if(!result["msg"].toString().compare("success"))
        {
            ret = 0;
        }
    }
    if(ret)
        request_token.clear();
    return ret;
}

int RequestCRM::get_hyperlink(const QString& url ,const QString& post_str ,QString& hyperlink)
{
    if(request_token.isEmpty()){
        request_token = get_tocken();
    }
    if(request_token.isEmpty()){
        return -1;
    }

    QString ret_data = get(url ,post_str);
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(ret_data.toUtf8(),&ok).toMap();

    int ret = -1;
    if(ok)
    {
        if(!result["msg"].toString().compare("success"))
        {
            ret = 0;
            QVariantMap data = result["data"].toMap();
            hyperlink = data["Hyperlink"].toString();
        }
    }
    if(ret)
        request_token.clear();
    return ret;
}

//curl http://vopapi.lenovoimage.com/api/User/Get_UserInfo?biz_content={Mobile:15918656240}
int RequestCRM::get_userinfo(const QString& phonenumber ,QString& hyperlink)
{
    QString post_str = QString("biz_content=") +
            "{" +
            "Mobile:" + phonenumber +
            "}";
    QString url = base_uri + "/User/Get_UserInfo";
    return get_hyperlink(url ,post_str ,hyperlink);
}

int RequestCRM::get_printersupplies(const QString& serialNO ,QString& hyperlink)
{
    QString post_str = QString("biz_content=") +
            "{" +
            "SerilNumber:" + "\"" + serialNO + "\"" +
            "}";
    QString url = base_uri + "/Device/PrinterSupplies";
    return get_hyperlink(url ,post_str ,hyperlink);
}

int RequestCRM::append_PrinterData(const QString& post_str)
{
    if(request_token.isEmpty()){
        request_token = get_tocken();
    }
    if(request_token.isEmpty()){
        return -1;
    }
    QString url = base_uri + "/Device/Append_PrinterData";
    QString ret_data = post(url ,post_str);
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(ret_data.toUtf8(),&ok).toMap();

    int ret = -1;
    if(ok)
    {
        if(!result["msg"].toString().compare("success"))
        {
            ret = 0;
        }
    }
    if(ret)
        request_token.clear();
    return ret;
}
