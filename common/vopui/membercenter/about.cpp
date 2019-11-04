#include "about.h"
#include "ui_about.h"
#include "uiconfig.h"
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this); 
    ui->lab_version->setText(QString("%1 %2").arg(tr("ResStr_Lenovo_Printer")).arg(app_version));
}

About::~About()
{
    delete ui;
}
