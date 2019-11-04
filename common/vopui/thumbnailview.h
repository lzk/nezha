#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QListWidget>

//#define DEBUG_VIEW 1

class ImageHandler : public QObject
{
    Q_OBJECT
public:
    explicit ImageHandler(QObject *parent = 0);
    ~ImageHandler();

signals:
    void image_send(QObject* ,const QImage& ,int ,int);

public slots:
    void image_answer_then_send(QObject* obj ,QListWidgetItem* item ,QSize size ,int weight ,int angle);
    void image_save(const QString& path ,int angle);
    void image_rm(const QString& path);
};

class ThumbnailView : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailView(QWidget *parent = 0);
    ~ThumbnailView();

private:
    ImageHandler* image_handler;

signals:
//    void print_scan_images(QStringList filename_list);

public slots:
    void add_image_item(const QString& ,QSize size = QSize());
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};

#endif // THUMBNAILVIEW_H
