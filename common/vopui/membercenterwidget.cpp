#include "membercenterwidget.h"
#include "ui_membercenterwidget.h"
#include "membercenter/userlogin.h"
#include "membercenter/about.h"
#include "membercenter/experiencepro.h"
#include "jsonparser/parser.h"
#include "qmessagebox.h"
#include "qdesktopservices.h"
#include <qstringlist.h>
#include "uinterface.h"
#include <qsettings.h>
#include "log.h"
#include "settingwarming.h"

#include "requestcrm.h"
#define NO_CRM 1
extern const char* g_config_file;
MemberCenterWidget::MemberCenterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemberCenterWidget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(1);
    m_strKey = "86c02972fba047b0b0a9adb8123029fb";

    ui->btChInfo->setDisabled(true);
    ui->changeMsg->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);color:gray}");

    ui->btProduct->setDisabled(true);
    ui->inbox->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);color:gray}");

#ifndef NO_CRM
    crmTimer = new QTimer(this);

    connect(crmTimer,SIGNAL(timeout()),this,SLOT(uploadCRM()));
#else
    ui->btExpe->hide();
    ui->exper_bg->hide();
    ui->exper_img->hide();
    ui->expe_arrow->hide();
    ui->experience->hide();
#endif
    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)), this ,SLOT(cmdResult(int,int,QVariant)));
    isLogin = false;

    int country = QLocale::system().country();
//    int country = QLocale::China;
    if(country == QLocale::China)
    {
        QSettings settings(g_config_file,QSettings::NativeFormat);
#ifndef NO_CRM
        QString CRM = settings.value("CRMTips").toString();
        m_bCRM = settings.value("enableCRM").toBool();
//        qDebug()<<"CRM"<<CRM;
        if(CRM != "true" )
        {
            settings.setValue("CRMTips","true");
            bool bCRM = false;
            ExperiencePro *exp = new ExperiencePro(this,bCRM);
            exp->exec();
            bCRM = exp->isStartCRM();
            if(bCRM)
            {
                startCRM();
            }
        }
#endif
        loginPhone = settings.value("loginPhone").toString();
        if(loginPhone != NULL)
        {
            QString password = settings.value("password").toString();
            UserLogin userLogin;
            userLogin.loginAction(loginPhone,password);
            if(userLogin.isLogin())
            {
#if 0
                QString loginName;
                settings.beginGroup(loginPhone);
                loginName = settings.value("loginName").toString();
//                qDebug()<<"loginName:"<<loginName;
                settings.endGroup();
                if(loginName != NULL)
                {
                    QFont ft = ui->login_name->font();
                    QFontMetrics fm(ft);
                    QString loginStr = QString("%0(%1)").arg(loginName).arg(loginPhone);
//                    QString loginStr = "自动获取自动获取自动获取自动获取自动获取自动获取自动获取自动获取自动获取自动获取自动获取";
                    QString ssid1 = fm.elidedText(loginStr, Qt::ElideRight, ui->login_name->width());
                    ui->login_name->setText(ssid1);
                }
                else
                {
                    ui->login_name->setText(loginPhone);
                }
#else
                ui->login_name->setText(loginPhone);
#endif
                ui->btLogin->setDisabled(true);
                ui->login_arrow->hide();
                isLogin = true;
                ui->btloginImg->setStyleSheet("QPushButton{"
                                               "border-image: url(:/Images/Logon_Active.png);}"
                                               "QPushButton:pressed{"
                                               "border-image: url(:/Images/Logon_Normal.png);}");
                ui->btloginImg2->setStyleSheet("QPushButton{"
                                               "border-image: url(:/Images/Logon_Active.png);}"
                                               "QPushButton:pressed{"
                                               "border-image: url(:/Images/Logon_Normal.png)}");

                ui->btChInfo->setEnabled(true);
                ui->changeMsg->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);}");

                ui->btProduct->setEnabled(true);
                ui->inbox->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);}");

#ifndef NO_CRM
                m_bCRM = settings.value("enableCRM").toBool();
//                qDebug()<<"m_bCRM"<<m_bCRM;

                if(m_bCRM)
                {
                    crmTimer->start(30*60*1000);//30min
                }
#endif
            }
        }
    }
}

MemberCenterWidget::~MemberCenterWidget()
{
    QSettings settings(g_config_file,QSettings::NativeFormat);
    if(loginPhone != NULL)
    {
        settings.beginGroup(loginPhone);
        settings.setValue("loginName",ui->login_name->text());
        settings.endGroup();
    }
#ifndef NO_CRM
    settings.setValue("enableCRM",m_bCRM);
#endif
    delete ui;
}

void MemberCenterWidget::on_btLogin_clicked()
{
    UserLogin *login = new UserLogin(this);
    login->setWindowTitle(tr("ResStr_Login_"));
    login->exec();

    if(login->isLogin())
    {
//        loginPhone = login->getPhone();
        QSettings settings(g_config_file ,QSettings::NativeFormat);
        loginPhone = settings.value("loginPhone").toString();
#if NEW_CRM
        ui->login_name->setText(loginPhone );
#else
        settings.beginGroup(loginPhone);
        QString userName = settings.value("loginName").toString();
        settings.endGroup();
        if(userName != NULL)
        {
            QFont ft = ui->login_name->font();
            QFontMetrics fm(ft);
            QString loginStr = QString("%0(%1)").arg(userName).arg(loginPhone);
            QString ssid1 = fm.elidedText(loginStr, Qt::ElideRight, ui->login_name->width());
            ui->login_name->setText(ssid1);
        }
        else
        {
            ui->login_name->setText(loginPhone );
        }
#endif
        ui->btLogin->setDisabled(true);
        ui->login_arrow->hide();
        isLogin = true;
        ui->btloginImg->setStyleSheet("QPushButton{"
                                       "border-image: url(:/Images/Logon_Active.png);}"
                                       "QPushButton:pressed{"
                                       "border-image: url(:/Images/Logon_Normal.png);}");
        ui->btloginImg2->setStyleSheet("QPushButton{"
                                       "border-image: url(:/Images/Logon_Active.png);}"
                                       "QPushButton:pressed{"
                                       "border-image: url(:/Images/Logon_Normal.png)}");
        bt->setStyleSheet("QPushButton{"
                          "border-image: url(:/Images/Logon_Active.png);}"
                          "QPushButton:pressed{"
                          "border-image: url(:/Images/Logon_Normal.png)}");
        ui->btChInfo->setEnabled(true);
        ui->changeMsg->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);}");

        ui->btProduct->setEnabled(true);
        ui->inbox->setStyleSheet("QLabel{background-color: rgb(235, 235, 235);}");
    }
}

void MemberCenterWidget::on_btAbout_clicked()
{
    About *about = new About(this);
    about->exec();
}

void MemberCenterWidget::on_btCancel_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

//void MemberCenterWidget::getUserInfo()
//{
//    QString baseUrl = "http://crm.iprintworks.cn/api/app_getuserinfo";
//    QUrl url(baseUrl);

//    QDateTime dateTime;
//    QString time = dateTime.currentDateTime().toString("yyyyMMddHHmmss");

//    QString text = loginPhone;

//    QString str = QString("%0%1%2").arg(text).arg(time).arg(m_strKey);
////    qDebug()<<str;

//    QString md5;
//    QByteArray bb;
//#if QT_VERSION > 0x050000
//    bb = QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
//#else
//    bb = QCryptographicHash::hash(str.toAscii(),QCryptographicHash::Md5);
//#endif
//    md5.append(bb.toHex());

//    QByteArray post_data;
//    QString post_str = QString("mobile=%0&time=%1&Sign=%2").arg(text).arg(time).arg(md5);
////    qDebug()<<post_str;
//#if QT_VERSION > 0x050000
//    post_data = post_str.toLocal8Bit();
//#else
//    post_data = post_str.toAscii();
//#endif

//    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_get(QNetworkReply*)));

//    QNetworkRequest req;

//    req.setUrl(url);

//    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
//    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

//    manager->post(req,post_data);

//}

void MemberCenterWidget::initUserInfo(QString userInfo)
{
//    qDebug()<<"replyFinish_get"<<userInfo;

    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(userInfo.toUtf8(),&ok).toMap();

    if (result["success"].toBool())
    {
        QVariantMap user = result["user"].toMap();

        if(user["realName"].toString() != NULL)
        {
            ui->le_name->setText(user["realName"].toString());
            ui->le_name->setCursorPosition(0);
            QFont ft = ui->login_name->font();
            QFontMetrics fm(ft);
            QString loginStr = QString("%0(%1)").arg(ui->le_name->text()).arg(loginPhone);
            QString ssid1 = fm.elidedText(loginStr, Qt::ElideRight, ui->login_name->width());
            ui->login_name->setText(ssid1);
        }
        else
        {
            ui->le_name->setText("");
            ui->login_name->setText(loginPhone);
        }

        QDate birthDate = user["birthDate"].toDate();
        ui->spinBox->setValue(birthDate.year());
        ui->spinBox_M->setValue(birthDate.month());
        ui->spinBox_D->setValue(birthDate.day());

        if(user["sex"].toString() != NULL)
        {
            if(user["sex"].toString() == "男")
            {
                ui->btFemale->setChecked(false);
                ui->btMan->setChecked(true);
            }
            else
            {
                ui->btFemale->setChecked(true);
                ui->btMan->setChecked(false);
            }
        }
        else
        {
            ui->btFemale->setChecked(false);
            ui->btMan->setChecked(true);
        }

        if(user["email"].toString() != NULL)
        {
            ui->le_mail->setText(user["email"].toString());
            ui->le_mail->setCursorPosition(0);
        }
        else
        {
            ui->le_mail->setText("");
        }

        if(user["address"].toString() != NULL)
        {
            ui->le_addr->setText(user["address"].toString());
            ui->le_addr->setCursorPosition(0);
        }
        else
        {
            ui->le_addr->setText("");
        }
    }
    else
    {
        ui->le_name->setText("");
        ui->login_name->setText(loginPhone);
        ui->btFemale->setChecked(false);
        ui->btMan->setChecked(true);
        ui->le_mail->setText("");
        ui->le_addr->setText("");
    }

    if(ui->le_name->text() == "" || ui->le_mail->text() == "" || ui->le_addr->text() == "")
    {
        ui->btApply->setEnabled(false);
    }
    else
    {
        ui->btApply->setEnabled(true);
    }
    ui->btChInfo->setEnabled(true);
    ui->btProduct->setEnabled(true);
    ui->stackedWidget->setCurrentIndex(0);
}

QString MemberCenterWidget::getHostMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();

    int nCnt = nets.count();
    QString strMacAddr = "";

    for(int i = 0; i < nCnt; i++)
    {
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}

void MemberCenterWidget::setUserInfo(QString fullName, QString birthDay, int sex, QString email, QString address)
{
    QString baseUrl = "http://crm.iprintworks.cn/api/app_userinfo";
    QUrl url(baseUrl);

    QString realName = fullName;


    QString strBirthDay = birthDay;
//    qDebug()<<"strBirthDay:"<<strBirthDay;

    int nSex = sex;

    QString strEmail = email;
    QString strAddress = address;

    QDateTime dateTime;
    QString time = dateTime.currentDateTime().toString("yyyyMMddHHmmss");

    QString strMacAddr = MemberCenterWidget::getHostMacAddress();
//    qDebug()<<"strMacAddr:"<<strMacAddr;

    QString md5_str = QString("%0%1%2").arg(strMacAddr).arg(time).arg(m_strKey);
//    qDebug()<<"md5_str:"<<md5_str;

    QString strSign;
    QByteArray bb;
#if QT_VERSION > 0x050000
    bb = QCryptographicHash::hash(md5_str.toLocal8Bit(),QCryptographicHash::Md5);
#else
    bb = QCryptographicHash::hash(md5_str.toAscii(),QCryptographicHash::Md5);
#endif
    strSign.append(bb.toHex());

    QString post_str = QString("MobileCode=%0&Mobile=%1&DeviceBrand%2&DeviceModel=%3&Truename=%4&Birthdate=%5&Sex=%6&Email=%7&Address=%8&time=%9&sign=%10")
            .arg(strMacAddr)
            .arg(loginPhone)
            .arg("")
            .arg("")
            .arg(realName)
            .arg(strBirthDay)
            .arg(nSex)
            .arg(strEmail)
            .arg(strAddress)
            .arg(time)
            .arg(strSign);
//    qDebug()<<"post_str:"<<post_str;
#if QT_VERSION > 0x050000
    QByteArray post_data = post_str.toLocal8Bit();
#else
    QByteArray post_data = post_str.toAscii();
#endif

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_set(QNetworkReply*)));

    QNetworkRequest req;

    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

    manager->post(req,post_data);
}

void MemberCenterWidget::replyFinish_set(QNetworkReply* reply)
{
    gUInterface->emitEnableCycleAnimation(false);

    QString strJsonText = reply->readAll();
//    qDebug()<<"replyFinish_set"<<strJsonText.toUtf8();

    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(strJsonText.toUtf8(),&ok).toMap();
    if(result["success"].toBool() == true)
    {
        QFont ft = ui->login_name->font();
        QFontMetrics fm(ft);
        QString loginStr = QString("%0(%1)").arg(ui->le_name->text()).arg(loginPhone);
        QString ssid1 = fm.elidedText(loginStr, Qt::ElideRight, ui->login_name->width());
        ui->login_name->setText(ssid1);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        QMessageBox *message = new QMessageBox(this);
        message->setWindowTitle(QString::fromLocal8Bit("错误"));
        message->setIconPixmap(QPixmap(":/Images/Warning.tif"));
        message->setText(QString::fromLocal8Bit("修改账户信息失败。"));
        message->exec();
    }
    reply->deleteLater();
}

void MemberCenterWidget::on_btChInfo_clicked()
{
#if NEW_CRM
    QString url;
    if(isLogin
            && !RequestCRM::get_userinfo(loginPhone ,url)
            ){
//        url = QString("http://t3.lenovoimage.com/abc/userinfo.html?id=%1&sn=").arg(loginPhone);
        QDesktopServices::openUrl(QUrl(url));
    }
#else
    ui->btChInfo->setEnabled(false);
    UserLogin userLogin;
    QString userInfo = userLogin.getUserInfo(loginPhone);
    initUserInfo(userInfo);
#endif
}

void MemberCenterWidget::startCRM()
{
#ifndef NO_CRM
    m_bCRM = true;
    MemberCenterWidget::uploadCRM();
    crmTimer->start(30*60*1000);//30min
#endif
}

void MemberCenterWidget::on_btExpe_clicked()
{
#ifndef NO_CRM
    ExperiencePro *exp = new ExperiencePro(this,m_bCRM);
    exp->exec();

    if(exp->isStartCRM() != m_bCRM)
    {
        m_bCRM = exp->isStartCRM();
        if(m_bCRM)
        {
//            qDebug()<<"start crm";
            startCRM();
        }
        else
        {
//            qDebug()<<"stop crm";
            crmTimer->stop();
        }
    }
#endif
}

void MemberCenterWidget::uploadCRM()
{
//    qDebug()<<"uploadcrm";
//    gUInterface->setCurrentPrinterCmd(UIConfig::LS_CMD_PRN_getCRMInfo);
}

#include "commonapi.h"
void MemberCenterWidget::cmdResult(int cmd,int result,QVariant data)
{
    switch(cmd)
    {
    case UIConfig::LS_CMD_PRN_getCRMInfo:
        if(!result)
        {
            struct_user_center userCenter = data.value<struct_user_center>();
//            qDebug()<<"user center printertotal:"<<userCenter.counter_printer_scan.PrinterTotal;
#if NEW_CRM
            QString SN = QString(userCenter.user_center._2ndSerialNO).left(20);
            QString Mobile = loginPhone;
            QString DeviceCode = MemberCenterWidget::getHostMacAddress();
            QString DeviceBrand = "Lenovo";
            QString DeviceModel = userCenter.strPrinterModel;
            QString Platform = "linux";//get_string_from_shell_cmd("cat /etc/issue");
//            Platform = Platform.left(Platform.indexOf('('));
//            Platform = Platform.left(Platform.indexOf('\\'));
            QString Version = app_version;
            QString PrinterModel = userCenter.strPrinterModel;
//            QString TonerSN = QString(userCenter.user_center._2ndSerialNO);
            QString InkLevel = QString("%1").arg(userCenter.printer_info.status.TonelStatusLevelK);
            QString TotalPrint = QString("%1").arg(userCenter.counter_printer_scan.Printer);
            QString TotalCopy = QString("%1").arg(userCenter.counter_printer_scan.Copier);
            QString TotalScan = QString("%1").arg(userCenter.counter_printer_scan.ScanTotal);
            QString TotalDuplexPrint = QString("%1").arg(userCenter.counter_printer_scan.DPrinter);
            QString TotalDuplexCopy = QString("%1").arg(userCenter.counter_printer_scan.DCopier);
            QString IsJob = "0";
//            QString JobTime = "1";
//            QString IsColor = "1";
//            QString IsDuplex = "0";
//            QString DocType = "1";
//            QString DocSize = "1";
            QString post_str =  QString("biz_content=") +
                    "{" +
                    "\"SN\":\"" + SN + "\"," +
                    "\"Mobile\":\"" + Mobile + "\"," +
                    "\"DeviceCode\":\"" + DeviceCode + "\"," +
                    "\"DeviceBrand\":\"" + DeviceBrand + "\"," +
                    "\"DeviceModel\":\"" + DeviceModel + "\"," +
                    "\"Platform\":\"" + (Platform.isEmpty() ?"mac":Platform) + "\"," +
                    "\"Version\":\"" + Version + "\"," +
                    "\"PrinterModel\":\"" + PrinterModel + "\"," +
//                    "\"TonerSN\":\"" + TonerSN + "\"," +
                    "\"InkLevel\":" + InkLevel + "," +
                    "\"TotalPrint\":" + TotalPrint + "," +
                    "\"TotalCopy\":" + TotalCopy + "," +
                    "\"TotalScan\":" + TotalScan + "," +
                    "\"TotalDuplexPrint\":" + TotalDuplexPrint + "," +
                    "\"TotalDuplexCopy\":" + TotalDuplexCopy + "," +
                    "\"IsJob\":" + IsJob +// "," +
//                    "\"JobTime\":\"" + JobTime + "\"," +
//                    "\"IsColor\":\"" + IsColor + "\"," +
//                    "\"IsDuplex\":\"" + IsDuplex + "\"," +
//                    "\"DocType\":\"" + DocType + "\"," +
//                    "\"DocSize\":" + DocSize +"" +
                    "}";
//            QJson::Parser parser;
//            bool ok;
//            QString aa = post_str.mid(12);
//            QVariantMap result = parser.parse(aa.toUtf8(),&ok).toMap();
            LOGLOG("to upload str:%s" ,post_str.toUtf8().constData());
            RequestCRM::append_PrinterData(post_str);
#else
            QString InkLevel = "0";
            QString printerDataStr = QString("[{{\"printermodel\":\"%0\",\"printerid\":\"%1\",\"tonerid\":\"%2\",\"totalprint\":\"%3\",\"inklevel\":\"%4\"}}]")
                    .arg(userCenter.strPrinterModel)
                    .arg(userCenter.user_center._2ndSerialNO)
                    .arg(userCenter.user_center._SerialNO4AIO)
                    .arg(userCenter.user_center._totalCounter)
                    .arg(inkLevel);

            QString strMacAddr = MemberCenterWidget::getHostMacAddress();
            QDateTime dateTime;
            QString time = dateTime.currentDateTime().toString("yyyyMMddHHmmss");
            QString md5_str = QString("%0%1%2").arg(strMacAddr).arg(time).arg(m_strKey);

            QString strPlatform = get_string_from_shell_cmd("cat /etc/issue");
            QString strVersion = "1.0.0.1";


            QString post_str = QString("{{\"devicecode\":\"%0\",\"mobile\":\"%1\",\"platform\":\"%2\",\"version\":\"%3\",\"time\":\"%4\",\"sign\":\"%5\",\"location\":\"\",\"printerdata\":%6}}")
                    .arg(strMacAddr)
                    .arg(loginPhone)
                    .arg(strPlatform)
                    .arg(strVersion)
                    .arg(time)
                    .arg(md5_str)
                    .arg(printerDataStr);
//            qDebug()<<post_str;
#if QT_VERSION > 0x050000
    QByteArray post_data = post_str.toLocal8Bit();
#else
    QByteArray post_data = post_str.toAscii();
#endif

             QString baseUrl = "http://crm.iprintworks.cn/api/printerinfo";//debug
             QUrl url(baseUrl);

             QNetworkAccessManager *manager = new QNetworkAccessManager(this);
             connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinish_uploadCRM(QNetworkReply*)));

             QNetworkRequest req;

             req.setUrl(url);

             req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
             req.setHeader(QNetworkRequest::ContentLengthHeader,post_data.length());

             manager->post(req,post_data);
#endif
        }
        break;

    default:
        break;

    }
}

void MemberCenterWidget::replyFinish_uploadCRM(QNetworkReply *reply)
{
    QString strJsonText = reply->readAll();
    LOGLOG("replyFinish_uploadCRM:%s" ,strJsonText.toUtf8().constData());
    reply->deleteLater();
}

void MemberCenterWidget::setSW(QStackedWidget* _sw, QPushButton * _bt)
{
    sw = _sw;
    bt = _bt;
    if(isLogin)
    {
        bt->setStyleSheet("QPushButton{"
                          "border-image: url(:/Images/Logon_Active.png);}"
                          "QPushButton:pressed{"
                          "border-image: url(:/Images/Logon_Normal.png)}");
    }
}

void MemberCenterWidget::on_pushButton_clicked()
{
    if(sw){
        sw->setCurrentIndex(0);
    }
}

void MemberCenterWidget::on_btApply_clicked()
{
    QRegExp rx2("^([a-zA-Z0-9_\-.]+)@(([[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.)"
                "|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})$");
   if( !rx2.exactMatch(ui->le_mail->text()) )
   {
       SettingWarming *msgWarm = new SettingWarming(this, tr("ResStr_Email_Format_Error"));
       msgWarm->setWindowTitle(tr("ResStr_Error"));
       msgWarm->setWindowFlags(msgWarm->windowFlags() & ~Qt::WindowMaximizeButtonHint \
                               & ~Qt::WindowMinimizeButtonHint);
       msgWarm->exec();
         return;
   }

   QString fullName = ui->le_name->text();

   QDate birthDate;
   birthDate.setDate(ui->spinBox->value(),ui->spinBox_M->value(),ui->spinBox_D->value());
   QString birthDay = birthDate.toString("yyyy-MM-dd");
   int sex = 0x01;

   if(ui->btFemale->isChecked())
   {
       sex = 0x00;
   }
   QString email = ui->le_mail->text();
   QString address = ui->le_addr->text();
   isNotShowWarning = false;
   setUserInfo(fullName,birthDay,sex,email,address);
   gUInterface->emitEnableCycleAnimation(true);
}

void MemberCenterWidget::on_btProduct_clicked()
{
    if(isLogin){
        QString url = QString("http://t3.lenovoimage.com/abc/myprinters.html?id=%1&sn=").arg(loginPhone);
        QDesktopServices::openUrl(QUrl(url));
    }
//    QDesktopServices::openUrl(QUrl("http://ibase.lenovoimage.com/home_abc.aspx"));
//    QDesktopServices::openUrl(QUrl("http://vopapi.lenovoimage.com/index.html"));
}

/*disable btApply button if not all the le_name,le_email,le_addr have got value*/
void MemberCenterWidget::on_le_name_textEdited(const QString &arg1)
{
    if(ui->le_addr->text().isEmpty() || ui->le_mail->text().isEmpty() || ui->le_name->text().isEmpty())
    {
        ui->btApply->setEnabled(false);
    }
    else
    {
        ui->btApply->setEnabled(true);
    }
}

void MemberCenterWidget::on_le_mail_textEdited(const QString &arg1)
{

    if(ui->le_addr->text().isEmpty() || ui->le_mail->text().isEmpty() || ui->le_name->text().isEmpty())
    {
        ui->btApply->setEnabled(false);
    }
    else
    {
        ui->btApply->setEnabled(true);
    }
}

void MemberCenterWidget::on_le_addr_textEdited(const QString &arg1)
{
    if(ui->le_addr->text().isEmpty() || ui->le_mail->text().isEmpty() || ui->le_name->text().isEmpty())
    {
        ui->btApply->setDisabled(true);
    }
    else
    {
        ui->btApply->setEnabled(true);
    }
}
