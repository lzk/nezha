#ifndef IMAGEPREVIEWDIALOG_H
#define IMAGEPREVIEWDIALOG_H

#include <QDialog>
class QListWidgetItem;
class ImageHandler;

namespace Ui {
class ImagePreviewDialog;
}

class ImagePreviewDialog : public QDialog
{
    Q_OBJECT

public:
    ImagePreviewDialog(QListWidgetItem*,ImageHandler*);
    ~ImagePreviewDialog();

private:
    Ui::ImagePreviewDialog *ui;
    QListWidgetItem* item;
    ImageHandler* image_handler;
    QImage image;

    int weight;
    int angle;

    void image_ask(int _weight = 0 ,int _angle = 0);

protected:
    void closeEvent(QCloseEvent *);
    void keyPressEvent(QKeyEvent *);
    bool eventFilter(QObject *, QEvent *);

signals:
    void image_ask(QObject* obj ,QListWidgetItem* item ,QSize size ,int weight ,int angle);

private slots:
    void image_update(QObject* obj ,const QImage& image ,int flag ,int weight);

private slots:
    void on_fit_clicked();
    void on_zoom_in_clicked();
    void on_zoom_out_clicked();
    void on_turn_clicked();
    void on_back_clicked();
//    void on_print_clicked();
};

#endif // IMAGEPREVIEWDIALOG_H
