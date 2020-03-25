#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include "uinterface.h"
#include "appconfig.h"
#include <signal.h>
#include "appserver.h"
UInterface* gUInterface;
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
#endif

void quit(int)
{
    LOGLOG("SIGINT quit");
    if(qApp)
        qApp->quit();
}

int main(int argc, char *argv[])
{
    if(AppConfig::initConfig()){
        Trans_Client tc(DOMAIN_UIEXE);
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
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Images/app.png"));
    a.setQuitOnLastWindowClosed(false);

    QTranslator trans;
    trans.load(QLocale(QLocale::Chinese),"vop" ,"." ,":/translations");
//    if(!trans.load(QLocale(QLocale::system().name()) ,"vop" ,"." ,":/translations")){
//        qDebug()<<"load trans";
//        trans.load(QLocale(QLocale::English),"vop" ,"." ,":/translations");
//    }
    a.installTranslator(&trans);

    QStringList arguments = QCoreApplication::arguments();
    if(arguments.contains("-test")){
        use_status_thread = false;
        testmode = true;
    }

    gUInterface = new UInterface;

    MainWindow w;
    w.connect(app_server ,SIGNAL(signal_startexe()) ,&w ,SLOT(show_top()));
//    w.setWindowIcon(QIcon(":/Images/app.png"));
    if(!arguments.contains("-hide"))
        w.show();

    int ret = a.exec();
    delete gUInterface;
    AppConfig::exit_app();
    return ret;
}
