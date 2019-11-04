#include "settingwarming.h"
#include "ui_settingwarming.h"

SettingWarming::SettingWarming(QWidget *parent, QString text, int wType) :
    QDialog(parent),
    ui(new Ui::SettingWarming)
{
    ui->setupUi(this);
    if(text != NULL)
    {
        ui->label->setText(text);
    }
    else
    {
        ui->label->setText(tr("ResStr_Msg_2"));
    }

    switch(wType)
    {
    case 0:
        ui->busyLabel->hide();
        break;
    case 1:
        ui->warningLabel->hide();
        ui->busyLabel->hide();
        ui->label->setGeometry(QRect(60,10,340,90));
        break;
    case 2:
        ui->warningLabel->hide();
        ui->busyLabel->startAnimation(20);
        break;
    default:
        break;
    }
}

SettingWarming::~SettingWarming()
{
    delete ui;
}

void SettingWarming::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
void SettingWarming::on_btClose_clicked()
{
    this->close();
}
