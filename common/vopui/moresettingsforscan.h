#ifndef MORESETTINGSFORSCAN_H
#define MORESETTINGSFORSCAN_H

#include <QDialog>
#include <QMouseEvent>
#include <QEvent>
#include <QPushButton>
#include <QTimer>
#include "scannerapp.h"
namespace Ui {
class MoreSettingsForScan;
}

class MoreSettingsForScan : public QDialog
{
    Q_OBJECT

public:
    explicit MoreSettingsForScan(QWidget *parent = 0, UiSettings *param= NULL);
    ~MoreSettingsForScan();

private:
    Ui::MoreSettingsForScan *ui;

    bool isPress;                           //标识鼠标左键是否按下，用于实现鼠标拖动事件
    QPoint last;
    QString *grey_str;                         //定义的一个灰色的颜色值，用于选中背景颜色设置
    QString *lightgrey_str;                    //定义的一个浅灰色的颜色值，用于选中背景颜色设置
    QTimer *timerB ;
    QTimer *timerC;
    int timeCount;
    int colorMode;

    UiSettings *param_scan;
    UiSettings defaultParam;

    void setDefValue();     //设置默认值
    void showParam(UiSettings *param);       //显示参数到ui
    void selectMode(int model);  //选择色彩模式
    void setBgColor(QPushButton *button, QString *color);     //设置背景颜色


protected slots:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    bool eventFilter(QObject *, QEvent *);      //事件过滤器，主要用于过滤lineEdit_scaling的foucout事件，
                                                //当用户lineEdit_scaling输入为空时焦点移开后未触发editedfinished事件
private slots:
    void on_btClose_clicked();
    void on_btOK_clicked();
    void on_btDefault_clicked();
//    void on_comboDpi_currentIndexChanged(int index);
//    void on_comboScanSize_currentIndexChanged(int index);
//    void on_btPthoto_toggled(bool checked);
//    void on_btPhotoText_toggled(bool checked);
//    void on_btText_toggled(bool checked);
    void on_btContrastReduce_clicked();
    void on_btContrastReduce_pressed();
    void on_btContrastReduce_released();
    void on_btContrastAdd_clicked();
    void on_btContrastAdd_pressed();
    void on_btContrastAdd_released();
    void on_btBrightnessReduce_clicked();
    void on_btBrightnessReduce_pressed();
    void on_btBrightnessReduce_released();
    void on_btBrightnessAdd_clicked();
    void on_btBrightnessAdd_pressed();
    void on_btBrightnessAdd_released();
    void onTimerBTimeout();
    void onTimerBTimeout1();
    void onTimerCTimeout();
    void onTimerCTimeout1();
    void on_lineEdit_constrast_textChanged(const QString &arg1);
    void on_lineEdit_brightness_textChanged(const QString &arg1);
    void on_slider_contrast_valueChanged(int value);
    void on_slider_brightness_valueChanged(int value);

    void on_btContrastBar_clicked();
    void on_btBrightnessBar_clicked();

    void on_btModeColor_clicked();

    void on_btModeGreyscale_clicked();

    void on_btModeBW_clicked();

    void on_btBgColor_clicked();

    void on_btBgGrayscale_clicked();

    void on_btBgBW_clicked();

signals:
    void signals_scan_buffer_size(float, int);//Added by gavin for setting scan buffer size. 2016-04-08
};

#endif // MORESETTINGSFORSCAN_H
