#ifndef EXPERIENCEPRO_H
#define EXPERIENCEPRO_H

#include <QDialog>

namespace Ui {
class ExperiencePro;
}

class ExperiencePro : public QDialog
{
    Q_OBJECT

public:
    explicit ExperiencePro(QWidget *parent = 0, bool bCRM = false);
    ~ExperiencePro();
    bool isStartCRM();

private slots:
    void on_bt_OK_clicked();

    void on_btLink_clicked();

    void on_checkBox_clicked(bool checked);

private:
    Ui::ExperiencePro *ui;
    bool m_bCRM;

};

#endif // EXPERIENCEPRO_H
