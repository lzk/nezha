#ifndef SETTINGWARMING_H
#define SETTINGWARMING_H

#include <QDialog>
#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>

namespace Ui {
class SettingWarming;
}

class SettingWarming : public QDialog
{
    Q_OBJECT

public:
    explicit SettingWarming(QWidget *parent = 0, QString text = "", int wType = 0);//0:warning image, 1:no image, 2:busyImage
    ~SettingWarming();
    Ui::SettingWarming *ui;
protected:
    void paintEvent(QPaintEvent *);
private slots:
    void on_btClose_clicked();

};

#endif // SETTINGWARMING_H
