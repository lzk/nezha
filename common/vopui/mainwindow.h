#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "statusmonitor.h"
#include <QVariant>
#include "uiconfig.h"
#include "busyrefreshlabel.h"
#include <qsystemtrayicon.h>
#include <qtimer.h>
#include <qelapsedtimer.h>

//#define DEBUG

namespace Ui {
class MainWindow;
}
class QListWidgetItem;
class QCloseEvent;
class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString selectState;
    QString unSelectState;
    QPoint last;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool isPress;
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *, QEvent *);

private slots:
    void on_Copy_clicked();

    void on_Scan_clicked();

    void on_Setting_clicked();

    void on_loginButton_clicked();


    void on_CopyImgBtn_clicked();

    void on_ScanImgBtn_clicked();

    void on_SettingImgBtn_clicked();

    void on_refreshBtn_clicked();

    void cmdResult(int cmd,int result ,QVariant data=QVariant());

    void setcurrentPrinter(const QString& str);
    void currentPrinterChanged(const QString& str);

    void enableMPrinter(bool enabled);

    void updateStatus(QVariant data);

    void updateOtherStatus(const QString& printer ,const PrinterStatus_struct& status);

    void updatePrinter(const QVariant& data);

    void on_deviceNameBox_currentIndexChanged(int index);

    void set_Message_Background_Color(UIConfig::EnumStatus s);

    void enableTroubleshootingPage(bool enabled);
    void onStatusCh(PrinterStatus_struct& status);
    void updateStatusPanel(int,int);

    void updateTonerCarStatus(int toner);
    void enableAllFunction(bool enabled);

    void setDeviceMsg(const QString& msg, int result);

    void startScan();
    void stopScan();
    void onTimeout();
    void blink();
    void scrollCaption();

    void enableCycleAnimation(bool enabled);
//    void startCycleAnimation();
//    void stopCycleAnimation();

    void on_errorBtn_clicked();

    void on_btCar_clicked();

private:
    Ui::MainWindow *ui;

    QString current_printer;

//    BusyRefreshLabel *statusCycle;
    BusyRefreshLabel *cycle;

    QStringList printers;
    QList<Printer_struct> printerlist;

    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void createSysTray();

    bool enabledScanCopy;
    bool enabledWiFi;

    bool isOfflineStart;
    bool isShowMaintain;

    bool isStartCopy;

    int deviceStatus;

    QTimer *timerDeviceMsg;
    QTimer *timerBlink;
    int _oldToner;
    QTimer *timerDeviceName;
    QElapsedTimer elapsedTimer;
    QString deviceStatusString;

    QString strScrollCation;

signals:
    void signalCloseAnimationDlg();
};

#endif // MAINWINDOW_H
