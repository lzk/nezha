#include "promptdialog.h"
#include "ui_promptdialog.h"

PromptDialog::PromptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PromptDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("ResStr_Prompt")); //added by gavin 2016-04-07
    connect(ui->btn_YES,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->btn_NO,SIGNAL(clicked()),this,SLOT(reject()));
}

void PromptDialog::setDialogMsg(const QString &msg)//added by gavin 2016-04-07
{
    ui->label->setText(msg);
}

void PromptDialog::setDialogMsgAlignment(Qt::Alignment align)//added by gavin 2016-04-07
{
    ui->label->setAlignment(align);
}

PromptDialog::~PromptDialog()
{
    delete ui;
}
