#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>

#if QT_VERSION > 0x050000
#include <QUrlQuery>
#endif
#include <QMessageBox>
#include "filterlib.h"
#include "filtercommon.h"

void handler_result(int result)
{
    QString str;
    switch (result) {
    case Filterlib_Status_OK:
        str = QString::fromUtf8("认证成功！");
        break;
    case Filterlib_Status_Fail:
        str = QString::fromUtf8("打印作业失败！");
        break;
    case Filterlib_Status_Cancel:
        str = QString::fromUtf8("取消打印作业！");
        break;
    case Filterlib_Status_invalidJobid:
        str = QString::fromUtf8("jobid无效！");
        break;
    case Filterlib_Status_timeout:
        str = QString::fromUtf8("超时！");
        break;
    case Filterlib_Status_checking:
        str = QString::fromUtf8("认证中！");
        break;
    case Filterlib_Status_Abort_Print:
        str = QString::fromUtf8("打印作业被取消！");
        break;
    case Filterlib_Status_authorize_fail:
        str = QString::fromUtf8("认证失败！");
        break;
    default:
        break;
    }
    QMessageBox msg(QMessageBox::Warning,QString::fromUtf8("警告"),str);
    msg.setStandardButtons (QMessageBox::Ok);
    msg.setButtonText (QMessageBox::Ok,QString::fromUtf8("确定"));
    msg.exec ();
    qApp->quit();
}


MainWindow::MainWindow(const QString& _job_info ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
  job_info(_job_info)
    ,result_over(false)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    ui->statusBar->hide();

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QUrl url(job_info);
//    QString cmd = url.scheme();
    printer_name = url.host();

    QString str = job_info;
    int index;
    index = str.indexOf("://");
//        cmd = str.left(index);
//        LOGLOG("cmd is:%s" ,cmd.toLatin1().constData());
    QString printer = job_info.mid(index + strlen("://"));
    index = printer.indexOf('?');
    if(index > 0)
        printer = printer.left(index);
    printer_name = printer;

#if QT_VERSION > 0x050000
    jobid = QUrlQuery(QUrl(url)).queryItemValue("jobid").toInt();
    time_val = QUrlQuery(QUrl(url)).queryItemValue("time_val").toInt();
#else
    jobid = QUrl(url).queryItemValue("jobid").toInt();
    time_val = QUrl(url).queryItemValue("time_val").toInt();
#endif
    if(time_val < 10)
        time_val = 30;

//    setWindowFlags(Qt::FramelessWindowHint);
    QString title = QString::fromUtf8("认证-") + printer_name + QString("-%1").arg(jobid);
    this->setWindowTitle(title);

    connect(&timer ,SIGNAL(timeout()) ,this ,SLOT(timeout()));

    timer.setInterval(1000);
    timer.start();

    filter_update_status(jobid ,Filterlib_Status_checking);

    ui->lineEdit->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timeout()
{
    time_val --;
    if(time_val > 0){
//        ui->label_timeval->setText(QString("%1").arg(time_val));

        int status = filter_get_status(jobid);
        if(status != Filterlib_Status_checking){
            if(!result_over){
                result_over = true;
                filter_update_status(jobid ,status);
                close();
                handler_result(status);
            }
        }
    }else if(!time_val){
        result_over = true;
        int result = Filterlib_Status_timeout;
        filter_update_status(jobid ,result);
        close();
        handler_result(result);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!result_over){
        hide();
        result_over = true;
        int result = Filterlib_Status_Cancel;
        filter_update_status(jobid ,result);
        handler_result(result);
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::on_pushButton_clicked()
{
    result_over = true;
    int result = Filterlib_Status_authorize_fail;
    if(!ui->lineEdit->text().compare("admin")
            && !ui->lineEdit_2->text().compare("123456"))
        result = Filterlib_Status_OK;
    filter_update_status(jobid ,result);
    close();
    handler_result(result);
}
