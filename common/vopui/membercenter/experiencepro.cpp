#include "experiencepro.h"
#include "ui_experiencepro.h"
#include "qdesktopservices.h"
#include "qurl.h"

ExperiencePro::ExperiencePro(QWidget *parent, bool bCRM) :
    QDialog(parent),
    ui(new Ui::ExperiencePro)
{
    ui->setupUi(this);
    ui->lab_msg->setWordWrap(true);
    ui->lab_msg->setText(tr("ResStr_In_order_to_deliver_improved"));
    ui->lab_check_info->setWordWrap(true);
    ui->lab_check_info->setText(tr("ResStr_I_have_read_and_agree"));

    m_bCRM = bCRM;

    ui->checkBox->setChecked(m_bCRM);
    this->setWindowIcon(QIcon(":/Images/logo.png"));
}

ExperiencePro::~ExperiencePro()
{
    delete ui;
}

void ExperiencePro::on_bt_OK_clicked()
{
    close();
}

void ExperiencePro::on_btLink_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.lenovo.com.cn/public/privacy.html"));
}

bool ExperiencePro::isStartCRM()
{
    return m_bCRM;
}

void ExperiencePro::on_checkBox_clicked(bool checked)
{
    m_bCRM = checked;
}
