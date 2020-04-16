#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "uinterface.h"
UInterface* gUInterface;
#include <sys/wait.h>
#include "appconfig.h"
#include "commonapi.h"
//#include "statusthread.h"
#include <qtranslator.h>
#include <qsplashscreen.h>
#include <qdesktopwidget.h>
#include <qelapsedtimer.h>
#include <qsettings.h>
#include <qproxystyle.h>

#include <QFontDatabase>
#include <QIODevice>
#include <QFile>
#include "appserver.h"
extern AppServer* app_server;

#ifndef Q_OS_DARWIN
extern "C"{
#include <string.h>
/* some systems do not have newest memcpy@@GLIBC_2.14 - stay with old good one */
#ifdef __x86_64__
asm (".symver memcpy, memcpy@GLIBC_2.2.5");
#elif __i386__
asm (".symver memcpy, memcpy@GLIBC_2.0");
#endif

//void *memcpy(void* ,const void* ,size_t);
void *__wrap_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

}

#if QT_VERSION < 0x050000
#ifdef STATIC_BUILD
#include <QtPlugin>
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qtiff)
Q_IMPORT_PLUGIN(qmng)
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qico)
#endif
#endif
#endif
//BMS:7489
//class MyProxyStyle : public QProxyStyle
//{
//public:
//    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
//    {
////        if(PE_FrameFocusRect == element && widget && widget->inherits("QListWidget"))
////        {

////        }
////        else
//        {
//            QProxyStyle::drawPrimitive(element,option,painter,widget);
//        }
//    }
//};


void quit(int)
{
    LOGLOG("SIGINT quit");
    if(qApp)
        qApp->quit();
}

QString loadFontFamilyFromFiles(const QString &filename)
{
    QString font = "";

    QFile fontFile(filename);
    if(!fontFile.open(QIODevice::ReadOnly))
    {
//        qDebug()<<"Open font file error";
        return font;
    }

    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
//    qDebug()<<"loadedFontID="<<loadedFontID;
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
    if(!loadedFontFamilies.empty())
    {
        font = loadedFontFamilies.at(0);
    }
    fontFile.close();

    return font;
}

int main(int argc, char *argv[])
{
    if(AppConfig::initConfig()){
        Trans_Client tc(DOMAIN_VOPEXE);
        char buffer[1024];
        sprintf(buffer ,"startexe");
        tc.writeThenRead(buffer ,1024);

        LOGLOG("There has been a same app running!");
        return 0;
    }

    signal(SIGINT ,quit);
#ifdef Q_WS_X11
    qputenv("LIBOVERLAY_SCROLLBAR", 0);
#endif
    qputenv("QT_X11_NO_MITSHM", "1");
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Images/logo.png"));
//    a.setStyle(new MyProxyStyle);

    if(QLocale::system().name() == "en_US")
    {
        QString strFont=loadFontFamilyFromFiles(":/fonts/FreeSans.ttf");
    }

    a.setStyleSheet("*{font-size: 12px}");

#if QT_VERSION < 0x050000
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));
#endif

    QTranslator trans;
    if(!trans.load(QLocale(QLocale::system().name()) ,"vop" ,"." ,":/translations")){
//        qDebug()<<"load trans";
        trans.load(QLocale(QLocale::English),"vop" ,"." ,":/translations");
    }
    a.installTranslator(&trans);

//    QCoreApplication::setOrganizationName("Lenovo");
//    QCoreApplication::setApplicationName("VOP");

    QSplashScreen *splash = new QSplashScreen;
    QString lan = QLocale::system().name();
    if(lan == "en_US")
    {
       splash->setPixmap(QPixmap(":/Images/splash_en.PNG"));
    }else if (lan == "zh_CN")
    {
       splash->setPixmap(QPixmap(":/Images/splash_zh.PNG"));
    }else
    {
       splash->setPixmap(QPixmap(":/Images/splash_en.PNG"));
    }
    splash->setEnabled(false);
    splash->show();

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.contains("-test")){
        testmode = true;
        use_status_thread = false;
    }

    gUInterface = new UInterface;

    MainWindow w;
    w.setGeometry(splash->geometry());
    w.setWindowIcon(QIcon(":/Images/logo.png"));
    w.connect(app_server ,SIGNAL(signal_startexe()) ,&w ,SLOT(show_top()));

//    QTranslator trans1;
//    trans1.load("qt_" + QLocale::system().name() ,":/translations");
//    a.installTranslator(&trans1);

    int delayTime=2;//splash display 2s
    QElapsedTimer timer;
    timer.start();

    while(timer.elapsed()<(delayTime*1000))
    {
        a.processEvents();
    }

//    if(!arguments.contains("-hide"))
        w.show();
    splash->finish(&w);
    delete splash;

    int ret = a.exec();
//    delete thread_server;
//    delete statusThread;
    delete gUInterface;
    AppConfig::exit_app();
    return ret;
}
