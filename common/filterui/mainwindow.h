#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QCloseEvent>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString& job_info ,QWidget *parent = 0);
    ~MainWindow();

private slots:
    void timeout();
    void on_pushButton_clicked();


protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    int jobid;
    int time_val;
    QString job_info;
    QString printer_name;
    QTimer timer;
    bool result_over;
};

#endif // MAINWINDOW_H
