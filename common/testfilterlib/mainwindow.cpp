#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filterlib.h"
static int callback_getPrinters(void* para,Printer_struct* ps)
{
    MainWindow* watcher = (MainWindow*)para;
    watcher->setPrinters(ps);
    return 1;
}

void MainWindow::setPrinters(Printer_struct* ps)
{
    printers << ps->name;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    printers.clear();
    cups_get_printers(callback_getPrinters ,this);
    ui->comboBox->addItems(printers);
    if(printers.isEmpty()){

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
#include <QMessageBox>
static int jobid = 0;
void MainWindow::on_pushButton_clicked()
{
    filterlib_struct para;
    strcpy(para.printername ,ui->comboBox->currentText().toUtf8().constData());
    strcpy(para.username ,"test user name");
    strcpy(para.filename ,"test file name");
    para.jobid = ++jobid;
    filterlib(&para);
    QMessageBox box;
    box.setText("printing...");
    box.exec();
    filterlib_exit();
}
