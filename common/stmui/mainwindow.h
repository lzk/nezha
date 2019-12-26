#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <QMouseEvent>
#include "uiconfig.h"
#include <qsystemtrayicon.h>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class QTableWidgetItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void cmdResult(int cmd,int result ,QVariant data=QVariant());

    void on_tableWidget_printers_itemSelectionChanged();

    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();

    void timeout();

private:
    void setcurrentPrinter(const QString& str);
//    void mouse_event(QMouseEvent* e);
    void updateStatus(const PrinterInfo_struct& printer);
    void updatePrinter(const QVariant& data);
    void updateCurrentPrinterStatus(const QString& printer);

    void createSysTray();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool event(QEvent *event);
    bool isPress;
    QPoint last;

private:
    Ui::MainWindow *ui;
    QString current_printer;
    QStringList printers;
    QList<PrinterInfo_struct> printerlist;
//    QList<Printer_struct> printerlist;

    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    int time_for_traymessage;
    int time_for_hide;
    QMap<QString ,int> error_map;
    QMap<QString ,int > shown_error_map;
//    QMap<QString ,QList<int> > shown_error_map;
    QString message_printer;
    QTimer timer;
public slots:
    void show_top();
};

#endif // MAINWINDOW_H
