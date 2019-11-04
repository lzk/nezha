#ifndef THUMBNAILIMAGE_H
#define THUMBNAILIMAGE_H

#include <QWidget>
#include <qtimer.h>

class QListWidgetItem;
class ImageHandler;

namespace Ui {
class ThumbnailImage;
}

class ThumbnailImage : public QWidget
{
    Q_OBJECT

public:
    ThumbnailImage(QListWidgetItem* ,ImageHandler*);
    ~ThumbnailImage();

    void image_ask();

private:
    Ui::ThumbnailImage *ui;

    QListWidgetItem* item;
    ImageHandler* image_handler;
    QString origin_image;
    QImage image;
    QPixmap pixmap;
    QTimer *pTimerSingleClick;

protected:
    virtual void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);

signals:
    void image_ask(QObject* obj ,QListWidgetItem* item ,QSize size ,int weight ,int angle);
    void image_save(const QString& path ,int angle);
    void image_rm(const QString& path);
//    void print_scan_images(QStringList filename_list);

private slots:
    void image_update(QObject* obj ,const QImage& image);
    void listWidget_itemSelectionChanged();
    void timerSingleClick();
    void doubleClickEvent();
};

#endif // THUMBNAILIMAGE_H
