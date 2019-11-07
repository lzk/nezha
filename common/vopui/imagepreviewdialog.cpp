#include "imagepreviewdialog.h"
#include "ui_imagepreviewdialog.h"
#include "thumbnailview.h"
#include "promptdialog.h"
#include <QLabel>
#include <QScrollBar>
#include <QDebug>

ImagePreviewDialog::~ImagePreviewDialog()
{
    delete ui;
}

ImagePreviewDialog::ImagePreviewDialog(QListWidgetItem* _item ,ImageHandler* handler):
    ui(new Ui::ImagePreviewDialog),
    item(_item),
    image_handler(handler),
    weight(0),
    angle(0)
{
    ui->setupUi(this);

    connect(image_handler ,SIGNAL(image_send(QObject*,QImage,int,int)) ,this ,SLOT(image_update(QObject*,QImage,int,int)));
    connect(this ,SIGNAL(image_ask(QObject*,QListWidgetItem*,QSize,int,int))
            ,image_handler ,SLOT(image_answer_then_send(QObject*,QListWidgetItem*,QSize,int,int)));

    ui->scrollArea->installEventFilter(this);
    this->setWindowIcon(QIcon(":/Images/logo.png"));
}

void ImagePreviewDialog::image_update(QObject *obj, const QImage &_image ,int flag ,int _weight)
{
    if(obj == this){
        image = _image;
        QLabel* label = new QLabel;
        label->setPixmap(QPixmap::fromImage(image));
        label->resize(image.size());
//        qDebug()<<"preview image size :"<<image.size();
        if((ui->scrollArea->size() - image.size()).isValid()){
            ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }else{
            ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        ui->scrollArea->setWidget(label);

        QScrollBar* bar = ui->scrollArea->horizontalScrollBar();
        if(qobject_cast<QScrollBar*>(bar)){
            bar->setValue((bar->maximum() - bar->minimum()) / 2 );
        }
        bar = ui->scrollArea->verticalScrollBar();
        if(qobject_cast<QScrollBar*>(bar)){
            bar->setValue((bar->maximum() - bar->minimum()) / 2 );
        }

        weight = _weight;

        if(flag & 1){
            ui->fit->setEnabled(true);
        }else{
            ui->fit->setEnabled(false);
        }
        if(flag & 2){
            ui->zoom_in->setEnabled(false);
        }else{
            ui->zoom_in->setEnabled(true);
        }
        if(flag & 4){
            ui->zoom_out->setEnabled(false);
        }else{
            ui->zoom_out->setEnabled(true);
        }

//        switch(flag){
//        case 0:
//            ui->fit->setEnabled(false);
//            ui->zoom_in->setEnabled(true);
//            ui->zoom_out->setEnabled(true);
//            break;
//        case 1:
//            ui->fit->setEnabled(true);
//            ui->zoom_in->setEnabled(true);
//            ui->zoom_out->setEnabled(true);
//            break;
//        case 2:
//            ui->fit->setEnabled(true);
//            ui->zoom_in->setEnabled(false);
//            ui->zoom_out->setEnabled(true);
//            break;
//        case -2:
//            ui->fit->setEnabled(true);
//            ui->zoom_in->setEnabled(true);
//            ui->zoom_out->setEnabled(false);
//            break;
//        case -1:
//        default:
//            ui->fit->setEnabled(false);
//            ui->zoom_in->setEnabled(false);
//            ui->zoom_out->setEnabled(false);
//            ui->turn->setEnabled(false);
//            break;
//        }
    }
}

void ImagePreviewDialog::image_ask(int _weight ,int _angle)
{
    weight = _weight;
    angle = _angle >= 4 ?0 :_angle;
    emit image_ask(this ,item ,ui->scrollArea->size() ,weight ,angle);
}

void ImagePreviewDialog::closeEvent(QCloseEvent *)
{
    on_back_clicked();
}

#include <QKeyEvent>
void ImagePreviewDialog::keyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_Escape == e->key()){
        on_back_clicked();
        e->ignore();
    }else{
        QDialog::keyPressEvent(e);
    }
}

bool ImagePreviewDialog::eventFilter(QObject *obj, QEvent *event)
{
//    qDebug()<<"eventFilter"<<angle<<" "<<weight;
    if(obj == ui->scrollArea){
        if(event->type() == QEvent::Resize){
//            qDebug()<<"1";
            image_ask(weight ,angle);
        }
    }
    return QWidget::eventFilter(obj,event);
}

void ImagePreviewDialog::on_fit_clicked()
{
    image_ask(0 ,angle);
}

void ImagePreviewDialog::on_zoom_in_clicked()
{
    image_ask(++weight ,angle);
}

void ImagePreviewDialog::on_zoom_out_clicked()
{
    image_ask(--weight ,angle);
}

void ImagePreviewDialog::on_turn_clicked()
{
    //image_ask(weight ,++angle);
    image_ask(0 ,++angle);
}

#include <QMessageBox>
void ImagePreviewDialog::on_back_clicked()
{
    int ret = 0;
    if(angle){
        PromptDialog *pDialog = new PromptDialog(this);
        pDialog->setDialogMsg(tr("ResStr_Scanning_image_has_been_changed__please_confirm_whether_save_it_or_not_"));
        pDialog->setDialogMsgAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (pDialog->exec() == QDialog::Accepted)
        {
            ret = angle;
        }
        else
        {
           ret = 0;
        }
    }
    done(ret);
}

//void ImagePreviewDialog::on_print_clicked()
//{
//    int ret = 20;
//    if(angle){
//        ret = QMessageBox::question(NULL ,"Lenovo" ,tr("ResStr_Scanning_image_has_been_changed__please_confirm_whether_save_it_or_not_"));
//        if(QMessageBox::Yes == ret){
//            ret =20 + angle;
//        }else{
//            ret = 20;
//        }
//    }
//    done(ret);
//}

