#include "moresettingsforscan.h"
#include "ui_moresettingsforscan.h"
#include <QIntValidator>
#include <QDebug>
#include <QGroupBox>
#include <QListView>

MoreSettingsForScan::MoreSettingsForScan(QWidget *parent, UiSettings *param) :
    QDialog(parent),
    ui(new Ui::MoreSettingsForScan)
{
    isPress = false;

    ui->setupUi(this);
    ui->btClose->hide();
    ui->label->hide();
    //限制亮度和对比度的输入框的输入范围在【min， max】的数值之间
    ui->lineEdit_brightness->setValidator(new QIntValidator(MIN_BRIGHT, 1000, this));
    ui->lineEdit_brightness->installEventFilter(this);
    ui->slider_brightness->setMaximum(MAX_BRIGHT);
    ui->slider_brightness->setMinimum(MIN_BRIGHT);
    ui->lineEdit_constrast->setValidator(new QIntValidator(MIN_CONTRAST, 1000, this));
    ui->lineEdit_constrast->installEventFilter(this);
    ui->slider_contrast->setMaximum(MAX_CONTRAST);
    ui->slider_contrast->setMinimum(MIN_CONTRAST);
    ui->label_tip->hide();

    //timer 指针初始化
    timerC = new QTimer(this);
    timerB = new QTimer(this);

    grey_str = new QString("130,130,130");
    lightgrey_str = new QString("211,211,211");
    param_scan = NULL;
    if(!param) //此处往后应改为传入的参数的指针。
    {
        param_scan = &defaultParam;
    }
    else
    {
        param_scan = param;
    }

    showParam(param);

    QListView *listView = new QListView(ui->comboDpi);
    listView->setStyleSheet("QListView::item:selected{background-color:gray;color:white}");
    ui->comboDpi->setView(listView);

    QListView *listView2 = new QListView(ui->comboScanSize);
    listView2->setStyleSheet("QListView::item:selected{background-color:gray;color:white}");
    ui->comboScanSize->setView( listView2);
}

MoreSettingsForScan::~MoreSettingsForScan()
{
    delete grey_str;
    delete lightgrey_str;
    delete ui;
}

//rewrite the mousePressEvent
void MoreSettingsForScan::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        isPress = true;
        last = e->globalPos();
    }
    else
    {
        isPress = false;
    }
}
void MoreSettingsForScan::mouseMoveEvent(QMouseEvent *e)
{
    if(isPress)
    {
        int dx = e->globalX() - last.rx();
        int dy = e->globalY() - last.ry();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
}
void MoreSettingsForScan::mouseReleaseEvent(QMouseEvent *e)
{
    isPress = false;
}

void MoreSettingsForScan::on_btClose_clicked()
{
    close();
}

void MoreSettingsForScan::on_btOK_clicked()
{
    //save para data
    param_scan->scan_dpi = Scan_Dpi(ui->comboDpi->currentIndex());
    param_scan->scan_size = Scan_Size(ui->comboScanSize->currentIndex());
    if(ui->btPthoto->isChecked())
    {
        param_scan->scan_doctype = T_Photo;
    }
    else if(ui->btPhotoText->isChecked())
    {
        param_scan->scan_doctype = T_Text_Graph;
    }else
    {
        param_scan->scan_doctype = T_Text;
    }

    param_scan->contrast = ui->lineEdit_constrast->text().toInt();
    param_scan->brightness = ui->lineEdit_brightness->text().toInt();
    param_scan->colorModel = colorMode;

    if (ui->btSpeed->isChecked())
    {
        param_scan->scan_type = Hight_Speed;
    }else
    {
        param_scan->scan_type = Hight_Quality;
    }

    //Added for show scanned image size by gavin 2016-04-08
    double paper_size[5] = {(210 / 25.4) * (297 /25.4), (148 / 25.4) * (210 /25.4), (182 / 25.4) * (257 /25.4), 8.5 * 11, 4 * 6};
    int res[5] = {100 * 100, 200 *200, 300 * 300, 600 * 600, 1200 * 1200};

//    int paper_size[5] = {
//        (int)(21000 / 25.4) * (int)(29700 /25.4),
//        (int)(14800 / 25.4) * (int)(21000 /25.4),
//        (int)(18200 / 25.4) * (int)(25700 /25.4),
//        850 * 1100,
//        400 * 600};
//    int res[5] = {1 * 1, 2 *2, 3 * 3, 6 * 6, 12 * 12};
    int scan_mode[3] = {24, 1 , 8};
    double image_size = paper_size[param_scan->scan_size] * res[param_scan->scan_dpi] * scan_mode[param_scan->colorModel] / 8;
    int uint = 0;
    double temp = 0;
    while((temp = image_size / 1024) > 1)
    {
        uint++;
        image_size = temp;
    }
    emit signals_scan_buffer_size(image_size, uint);
    //end 2016-04-08

    close();
}

void MoreSettingsForScan::on_btDefault_clicked()
{
    setDefValue();
}


//设置默认值
void MoreSettingsForScan::setDefValue()
{
    UiSettings tmpParam;
    UiSettings *defParamScan = &tmpParam;
    defParamScan->scan_doctype = T_Photo;
    defParamScan->scan_dpi = Scan_300DPI;
    defParamScan->colorModel = Color;
    defParamScan->contrast = 50;
    defParamScan->brightness = 50;
    defParamScan->scan_size = Scan_A4;
    defParamScan->scan_type = Hight_Speed;
    showParam(defParamScan);
}

//显示参数到ui
void MoreSettingsForScan::showParam(UiSettings *param)
{
    QString text;

    switch(param->scan_doctype)
    {
    case T_Photo:   ui->btPthoto->setChecked(true); break;
    case T_Text_Graph: ui->btPhotoText->setChecked(true); break;
    case T_Text:    ui->btText->setChecked(true);   break;
    default: break;
    }

    switch(param->scan_type)
    {
    case Hight_Speed: ui->btSpeed->setChecked(true); break;
    case Hight_Quality: ui->btQuality->setChecked(true);break;
    default: break;
    }

    ui->comboDpi->setCurrentIndex((int)param->scan_dpi);
    ui->lineEdit_constrast->setText(text.number(param->contrast));
    ui->slider_contrast->setValue(param->contrast);

    ui->lineEdit_brightness->setText(text.number(param->brightness));
    ui->slider_brightness->setValue(param->brightness);

    ui->comboScanSize->setCurrentIndex((int)param->scan_size);

    selectMode(param->colorModel);
}

//设置背景颜色
void MoreSettingsForScan::setBgColor(QPushButton *button, QString *color)
{
    button->setStyleSheet(QString("border-radius:10px; background-color: rgb(").append(color).append(");"));
}

//选择色彩模式
 void MoreSettingsForScan::selectMode(int model)
 {
    colorMode = model;
    setBgColor(ui->btBgColor, lightgrey_str);
    setBgColor(ui->btBgBW,lightgrey_str);
    setBgColor(ui->btBgGrayscale,lightgrey_str);

    switch(model)
     {
     case Color:setBgColor(ui->btBgColor, grey_str);    break;
     case Grayscale:setBgColor(ui->btBgGrayscale, grey_str);    break;
     case Black_White:setBgColor(ui->btBgBW, grey_str); break;
     default:  break;
     }
}

//void MoreSettingsForScan::on_comboDpi_currentIndexChanged(int index)
//{
//    param_scan->scan_dpi = Scan_Dpi(index);
//}

//void MoreSettingsForScan::on_comboScanSize_currentIndexChanged(int index)
//{
//    param_scan->scan_size = Scan_Size(index);
//}

//void MoreSettingsForScan::on_btPthoto_toggled(bool checked)
//{
//    if(checked)
//    {
//        param_scan->scan_doctype = T_Photo;
//    }
//}

//void MoreSettingsForScan::on_btPhotoText_toggled(bool checked)
//{
//    if(checked)
//    {
//        param_scan->scan_doctype = T_Text_Graph;
//    }
//}

//void MoreSettingsForScan::on_btText_toggled(bool checked)
//{
//    if(checked)
//    {
//        param_scan->scan_doctype = T_Text;
//    }
//}


/*****************************************************************************
*对比度两边加减按钮的点击及长按事件的响应，实现Contrast的加减
******************************************************************************/
void MoreSettingsForScan::on_btContrastReduce_clicked()
{
    QString text;
    int value = ui->lineEdit_constrast->text().toInt();
    if(value > MIN_CONTRAST && value <= MAX_CONTRAST)
    {
//        param_scan->contrast--;
        value --;
        ui->lineEdit_constrast->setText(text.number(value));
    }

}

void  MoreSettingsForScan::onTimerCTimeout()
{
    if(timeCount>9) on_btContrastReduce_clicked();
    timeCount++;
}
void MoreSettingsForScan::on_btContrastReduce_pressed()
{
    timeCount = 0;
    connect(timerC, SIGNAL(timeout()),this, SLOT(onTimerCTimeout()));
    timerC->start(60);
}

void MoreSettingsForScan::on_btContrastReduce_released()
{
    timerC->stop();
    disconnect(timerC, SIGNAL(timeout()), this, SLOT(onTimerCTimeout()));
}

void MoreSettingsForScan::on_btContrastAdd_clicked()
{
    QString text;
    int value = ui->lineEdit_constrast->text().toInt();
    if(value >= MIN_CONTRAST && value < MAX_CONTRAST)
    {
//        param_scan->contrast++;
        value ++;
        ui->lineEdit_constrast->setText(text.number(value));
    }
}

void MoreSettingsForScan::onTimerCTimeout1()
{
    if(timeCount>9) on_btContrastAdd_clicked();
    timeCount++;
}

void MoreSettingsForScan::on_btContrastAdd_pressed()
{
    timeCount = 0;
    connect(timerC, SIGNAL(timeout()),this, SLOT(onTimerCTimeout1()));
    timerC->start(60);
}

void MoreSettingsForScan::on_btContrastAdd_released()
{
    timerC->stop();
    disconnect(timerC, SIGNAL(timeout()), this, SLOT(onTimerCTimeout1()));
}

/*****************************************************************************
*对比度lineEdit的输入属性设置
*
******************************************************************************/
void MoreSettingsForScan::on_lineEdit_constrast_textChanged(const QString &arg1)
{
    if(arg1.toInt()>=MIN_CONTRAST && arg1.toInt()<=MAX_CONTRAST && arg1  != "")
    {
//        param_scan->contrast = arg1.toInt();
        ui->slider_contrast->setValue(arg1.toInt());
        ui->label_tip->setVisible(false);
        ui->lineEdit_constrast->setStyleSheet("#lineEdit_constrast{border:transparent;}");
        ui->btContrastAdd->setEnabled(true);
        ui->btContrastReduce->setEnabled(true);
        ui->btOK->setEnabled(true);
    }
    else
    {
        ui->label_tip->setGeometry(195,202,331,20);/**********************************************/
        ui->label_tip->setVisible(true);
        ui->lineEdit_constrast->setStyleSheet("#lineEdit_constrast{border:2px solid red;}");
        ui->btContrastAdd->setEnabled(false);
        ui->btContrastReduce->setEnabled(false);
        ui->btOK->setEnabled(false);
    }
}

void MoreSettingsForScan::on_slider_contrast_valueChanged(int value)
{
    QString text;
    ui->lineEdit_constrast->setText(text.number(value));
}


void MoreSettingsForScan::on_btContrastBar_clicked()
{
    QString text;
    QPoint bar = this->mapToGlobal(ui->btContrastBar->pos());
    QPoint curPoint = QCursor::pos();

    int dy = curPoint.y() - bar.y();
    int dx = curPoint.x() - bar.x();
    int value =100 * dx / ui->btContrastBar->geometry().width();
    if(value >= 0 || value <= 100)
    on_lineEdit_constrast_textChanged(text.number(value));
}



/*****************************************************************************
*亮度两边加减按钮的点击及长按事件的响应，实现brightness的加减
******************************************************************************/
void MoreSettingsForScan::on_btBrightnessReduce_clicked()
{
    QString text;
    int value = ui->lineEdit_brightness->text().toInt();
    if(value > MIN_BRIGHT && value <= MAX_BRIGHT)
    {
//        param_scan->brightness--;
        value --;
        ui->lineEdit_brightness->setText(text.number(value));
    }
}

void MoreSettingsForScan::onTimerBTimeout()
{
    if(timeCount>9) on_btBrightnessReduce_clicked();
    timeCount++;
}

void MoreSettingsForScan::on_btBrightnessReduce_pressed()
{
    timeCount = 0;
    connect(timerB, SIGNAL(timeout()), this, SLOT(onTimerBTimeout()));
    timerB->start(60);
}

void MoreSettingsForScan::on_btBrightnessReduce_released()
{
    timerB->stop();
    disconnect(timerB, SIGNAL(timeout()), this, SLOT(onTimerBTimeout()));
}

void MoreSettingsForScan::on_btBrightnessAdd_clicked()
{
    QString text;
    int value = ui->lineEdit_brightness->text().toInt();
    if(value >= MIN_BRIGHT && value < MAX_BRIGHT)
    {
//        param_scan->brightness++;
        value ++;
        ui->lineEdit_brightness->setText(text.number(value));
    }
}

void MoreSettingsForScan::onTimerBTimeout1()
{
    if(timeCount>9) on_btBrightnessAdd_clicked();
    timeCount++;
}

void MoreSettingsForScan::on_btBrightnessAdd_pressed()
{
    timeCount = 0;
    connect(timerB, SIGNAL(timeout()), this, SLOT(onTimerBTimeout1()));
    timerB->start(60);
}

void MoreSettingsForScan::on_btBrightnessAdd_released()
{
    timerB->stop();
    disconnect(timerB, SIGNAL(timeout()), this, SLOT(onTimerBTimeout1()));
}

/*****************************************************************************
*亮度lineEdit的输入属性设置
*
******************************************************************************/


void MoreSettingsForScan::on_lineEdit_brightness_textChanged(const QString &arg1)
{
    if(arg1.toInt()>=MIN_BRIGHT && arg1.toInt()<=MAX_BRIGHT && arg1  != "")
    {
//        param_scan->brightness = arg1.toInt();
        ui->slider_brightness->setValue(arg1.toInt());
        ui->label_tip->setVisible(false);
        ui->lineEdit_brightness->setStyleSheet("#lineEdit_brightness{border:transparent;}");
        ui->btBrightnessAdd->setEnabled(true);
        ui->btBrightnessReduce->setEnabled(true);
        ui->btOK->setEnabled(true);
    }
    else
    {
        ui->label_tip->setGeometry(195,320,331,20);/***********************************************/
        ui->label_tip->setVisible(true);
        ui->lineEdit_brightness->setStyleSheet("#lineEdit_brightness{border:2px solid red;}");
        ui->btBrightnessAdd->setEnabled(false);
        ui->btBrightnessReduce->setEnabled(false);
        ui->btOK->setEnabled(false);
    }
}


void MoreSettingsForScan::on_slider_brightness_valueChanged(int value)
{
    QString text;
    ui->lineEdit_brightness->setText(text.number(value));
}

void MoreSettingsForScan::on_btBrightnessBar_clicked()
{
    QString text;
    QPoint bar = this->mapToGlobal(ui->btBrightnessBar->pos());
    QPoint curPoint = QCursor::pos();

    int dy = curPoint.y() - bar.y();
    int dx = curPoint.x() - bar.x();
    int value =100 * dx / ui->btBrightnessBar->geometry().width();
    if(value>=0 && value <=100)
    on_lineEdit_brightness_textChanged(text.number(value));

}

bool MoreSettingsForScan::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit_constrast)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(ui->lineEdit_constrast->text().toInt()>MAX_CONTRAST )
            {
//                param_scan->contrast = 50;
                ui->lineEdit_constrast->setText(QString("100"));
            }
            else if(ui->lineEdit_constrast->text() == "")
            {
                ui->lineEdit_constrast->setText(QString("50"));
            }
        }
    }
    if(watched == ui->lineEdit_brightness)
    {
        if(event->type() == QEvent::FocusOut)
        {
            if(ui->lineEdit_brightness->text().toInt()>MAX_BRIGHT )
            {
//                param_scan->brightness = 50;
                ui->lineEdit_brightness->setText(QString("100"));
            }
            else if (ui->lineEdit_brightness->text() == "")
            {
                ui->lineEdit_brightness->setText(QString("50"));
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void MoreSettingsForScan::on_btModeColor_clicked()
{
//    param_scan->colorModel = Color;
    selectMode(Color);
}

void MoreSettingsForScan::on_btModeGreyscale_clicked()
{
//    param_scan->colorModel = Grayscale;
    selectMode(Grayscale);
}

void MoreSettingsForScan::on_btModeBW_clicked()
{
//    param_scan->colorModel = Black_White;
    selectMode(Black_White);
}

void MoreSettingsForScan::on_btBgColor_clicked()
{
    on_btModeColor_clicked();
}

void MoreSettingsForScan::on_btBgGrayscale_clicked()
{
    on_btModeGreyscale_clicked();
}

void MoreSettingsForScan::on_btBgBW_clicked()
{
    on_btModeBW_clicked();
}
