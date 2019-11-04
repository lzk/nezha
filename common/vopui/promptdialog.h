#ifndef PROMPTDIALOG_H
#define PROMPTDIALOG_H

#include <QDialog>

namespace Ui {
class PromptDialog;
}

class PromptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PromptDialog(QWidget *parent = 0);
    void setDialogMsg(const QString &msg);
    void setDialogMsgAlignment(Qt::Alignment align);
    ~PromptDialog();

private:
    Ui::PromptDialog *ui;
};

#endif // PROMPTDIALOG_H
