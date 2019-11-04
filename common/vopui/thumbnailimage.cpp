#include "thumbnailimage.h"
#include "ui_thumbnailimage.h"

#include <QListWidget>
#include <QListWidgetItem>
#include "imagepreviewdialog.h"
#include "thumbnailview.h"
#include <QBitmap>
#include <qdebug.h>
#include <qfile.h>

extern QString get_preview_file_name(const QString& origin_file_name);

ThumbnailImage::~ThumbnailImage()
{
    QString path = item->data(Qt::UserRole).toString();
    image_rm(path);
    delete ui;
    delete item;
}

ThumbnailImage::ThumbnailImage(QListWidgetItem* _item ,ImageHandler* handler) :
    ui(new Ui::ThumbnailImage),
    item(_item),
    image_handler(handler),
    image(QImage())
{
    ui->setupUi(this);
    pixmap = QPixmap(":/Images/close.png");
    ui->label->resize(pixmap.size());
    ui->label->setPixmap(pixmap);
//    ui->label->setMask(pixmap.mask());
    ui->label->setMask(QRegion(ui->label->rect() ,QRegion::Ellipse));
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->installEventFilter(this);

    connect(item->listWidget() ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(listWidget_itemSelectionChanged()));

    connect(image_handler ,SIGNAL(image_send(QObject*,QImage,int,int)) ,this ,SLOT(image_update(QObject*,QImage)));
    connect(this ,SIGNAL(image_ask(QObject*,QListWidgetItem*,QSize,int,int))
            ,image_handler ,SLOT(image_answer_then_send(QObject*,QListWidgetItem*,QSize,int,int)));
    connect(this ,SIGNAL(image_rm(QString)),image_handler ,SLOT(image_rm(QString)));
    connect(this ,SIGNAL(image_save(QString,int)),image_handler ,SLOT(image_save(QString,int)));


    pTimerSingleClick = new QTimer(this);
    connect(pTimerSingleClick,SIGNAL(timeout()),this,SLOT(timerSingleClick()));
    listWidget_itemSelectionChanged();
    image_ask();
}

void ThumbnailImage::timerSingleClick()
{
    pTimerSingleClick->stop();
    if(item->isSelected()){
        int index = item->listWidget()->selectedItems().indexOf(item) + 1;
//        ui->label->setText(tr("<h2><i><font color=white>%1</font></i></h2>").arg(index));
        ui->label->setText(tr("<font fontsize=15 color=white>%1</font>").arg(index));
        ui->label->setStyleSheet("background-color: green;");
    }else{
        ui->label->setPixmap(pixmap);
        ui->label->setStyleSheet("background-color: white;");
    }
}

void ThumbnailImage::listWidget_itemSelectionChanged()
{
    pTimerSingleClick->start(430);
}

void ThumbnailImage::image_ask()
{
    //get preview image
//    QString imageFileName = item->data(Qt::UserRole).toString();
//    QSize image_size = item->data(Qt::UserRole + 1).toSize();
//    QImage image(imageFileName);
//    QString previewFileName = get_preview_file_name(imageFileName);
//    qDebug()<<previewFileName<<" "<<image.depth();
//    if(image.depth() != 1){
//        QSize size = image_size;
//        while(size.width() * size.height() > 30 * 1024 * 1024){
//            size /= 2;
//        }
//        image.scaled(size).save(get_preview_file_name(imageFileName));
////            while(size.width() * size.height() > 1 * 1024 * 1024){
////                size /= 2;
////            }
////            image.scaled(size).save(get_thumbnail_file_name(imageFileName));
//    }else{
//        QFile::copy(imageFileName ,get_preview_file_name(imageFileName));
////            QFile::copy(imageFileName ,get_thumbnail_file_name(imageFileName));
//    }

    emit image_ask(this ,item ,item->sizeHint() ,0 ,0);
}

void ThumbnailImage::image_update(QObject *obj, const QImage &_image)
{
//    qDebug()<<"ThumbnailImage::image_update";
    if(obj == this){
        image = _image;
        update();
    }
}

#include <QPainter>
void ThumbnailImage::paintEvent(QPaintEvent *e)
{
    if(!image.isNull()){
        QPainter painter(this);
//        painter.drawImage(QPoint((width() - image.width())/2 ,(height() - image.height()) / 2) ,image);
        int imgWidth;
        int imgHeight;
        if(image.width()>image.height())
        {
            imgWidth = width();
            imgHeight = image.height()*imgWidth/image.width();
        }
        else if(image.width() < image.height())
        {
            imgHeight = height();
            imgWidth = image.width()*imgHeight/image.height();
        }
        else
        {
            imgWidth = width();
            imgHeight = height();
        }

        painter.drawPixmap((width()-imgWidth)/2,(height()-imgHeight)/2,imgWidth,imgHeight,QPixmap::fromImage(image));
    }
    QWidget::paintEvent(e);
}

#include "promptdialog.h"
#include <QMouseEvent>
bool ThumbnailImage::eventFilter(QObject * obj, QEvent * event)
{
    if(obj == ui->label){
        if(event->type() == QEvent::MouseButtonPress){
            QMouseEvent* me = static_cast<QMouseEvent*>(event);
            if(Qt::LeftButton == me->button()){
                if(!item->isSelected()){
                    PromptDialog *pDialog = new PromptDialog(this);
                    pDialog->setDialogMsg(tr("ResStr_Are_you_sure_to_delete_the_selected_picture"));
                    pDialog->setDialogMsgAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                    if (pDialog->exec() == QDialog::Accepted)
                    {
                        item->listWidget()->removeItemWidget(item);
                        item->listWidget()->takeItem(item->listWidget()->row(item));
                    }
                }
                return true;
            }
        }
//    }else if(event->type() == QEvent::MouseButtonDblClick){

//        QMouseEvent* me = static_cast<QMouseEvent*>(event);
//        if(Qt::LeftButton == me->button()){
//            doubleClickEvent();
//            return true;
//        }
    }
    return QWidget::eventFilter(obj,event);
}

void ThumbnailImage::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(Qt::LeftButton == e->button()){
        doubleClickEvent();
    }
}

void ThumbnailImage::doubleClickEvent()
{
    pTimerSingleClick->stop();
    if(item){
        item->setSelected(false);
        item->setSelected(true);
//        item->setSelected(!item->isSelected());
        timerSingleClick();
        ImagePreviewDialog dialog(item ,image_handler);
        int ret = dialog.exec();
        int angle = ret % 4;
        //rotato
        QString path = item->data(Qt::UserRole).toString();

        if(angle){
            emit image_save(path ,angle);
            if(angle % 4){
                if(angle % 2){
                    QSize size = QSize(item->data(Qt::UserRole + 1).toSize().height(),item->data(Qt::UserRole + 1).toSize().width());
                    item->setData(Qt::UserRole + 1,size);
                }
            }
            image_ask();
        }
//        //print
//        if(ret >= 20){
//            QStringList sl;
//            sl << path;
//            emit print_scan_images(sl);
//        }
    }
}

