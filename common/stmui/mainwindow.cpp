#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uinterface.h"
#include "uiconfig.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("ResStr_AppName"));
    setWindowFlags(Qt::FramelessWindowHint);
    ui->menuBar->hide();
    ui->statusBar->hide();
    ui->mainToolBar->hide();

    QHeaderView *headerView = ui->tableWidget_printers->horizontalHeader();
//    headerView->setStretchLastSection(true);
#if QT_VERSION > 0x050000
    headerView->setSectionResizeMode(QHeaderView::Stretch);
#else
    headerView->setResizeMode(QHeaderView::Stretch);
#endif
//    ui->tableWidget_printers->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignHCenter);
    ui->tableWidget_printers->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget_printers->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget_printers->horizontalHeaderItem(2)->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget_printers->horizontalHeaderItem(3)->setTextAlignment(Qt::AlignLeft);

    ui->label_status_image->hide();

    createSysTray();

//    headerView->setDefaultAlignment(Qt::AlignTop);
    connect(gUInterface ,SIGNAL(cmdResult(int,int,QVariant)) ,this ,SLOT(cmdResult(int,int,QVariant)));

    gUInterface->setCmd(UIConfig::CMD_GetPrinters ,current_printer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createSysTray()
{
    quitAction = new QAction(QString("%1(&Q)").arg(tr("ResStr_Exit")),this);
    connect(quitAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    trayIconMenu = new QMenu(this);
//    trayIconMenu->addAction(minimizeAction);
//    trayIconMenu->addAction(maximizeAction);
//    trayIconMenu->addAction(restoreAction);
//    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/Images/app_gray.png"));

    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
}
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:{
//        if(printers.isEmpty()){

//            QMessageBox message_box;
//            message_box.setButtonText(QMessageBox::Ok ,"确定");
//            message_box.setIcon(QMessageBox::Critical);
//            message_box.setText("系统中未安装打印机。");
//            message_box.setWindowTitle(" ");
//            message_box.exec();
//        }else{
            showNormal();
//        }
    }
        break;
    case QSystemTrayIcon::MiddleClick:
//        showMessage();
        break;
    default:
        ;
    }
}
void MainWindow::messageClicked()
{
    showNormal();
}



void MainWindow::mousePressEvent(QMouseEvent *e)
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

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(isPress)
    {
        int dx = e->globalX() - last.rx();
        int dy = e->globalY() - last.ry();
        last = e->globalPos();
        move(x()+dx, y()+dy);
    }
//    qDebug()<<e->pos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    isPress = false;
}
//void MainWindow::mouse_event(QMouseEvent *e)
//{
//    switch (e->type()) {
//    case QEvent::MouseButtonPress:
//        if(e->button() != Qt::LeftButton){
//            return;
//        }
//        break;
//    case QEvent::MouseButtonRelease:
//        if(e->button() != Qt::LeftButton){
//            return;
//        }
//        break;
//    case QEvent::MouseMove:
//        if(e->button() != Qt::LeftButton){
//            return;
//        }
//        break;
//    default:
//        break;
//    }
//}

void MainWindow::setcurrentPrinter(const QString& str)
{
    current_printer = str;
    gUInterface->setcurrentPrinter(str);
//    gUInterface->setCmd(UIConfig::CMD_GetStatus ,current_printer);

//    setWindowTitle(app_name + " - " + str);
    updateCurrentPrinterStatus(str);
    ui->tableWidget_printers->selectRow(printers.indexOf(str));
    LOGLOG("set current printer:%s" ,str.toLatin1().constData());
}

void MainWindow::cmdResult(int cmd,int result ,QVariant data)
{
    switch(cmd){
    case UIConfig::CMD_GetStatus:{
        PrinterInfo_struct printerInfo = data.value<PrinterInfo_struct>();
//        PrinterStatus_struct& status = printerInfo.status;
        if(!printerInfo.printer.name[0] || !current_printer.compare(printerInfo.printer.name)){
            updateStatus(printerInfo);
        }
    }
        break;

    case UIConfig::CMD_GetPrinters:{
        if(!result){
            updatePrinter(data);
            updateCurrentPrinterStatus(current_printer);
        }
    }
        break;

    default:
        break;
    }
}

void MainWindow::updatePrinter(const QVariant& data)
{
#if 0
    printerInfos = data.value<QList<PrinterInfo_struct> >();
    PrinterInfo_struct printerInfo;
    int base = 0;
    ui->tableWidget_printers->setRowCount(printerInfos.length());
    printers.clear();
    int index_of_defaultprinter = 0;
    for(int i = 0 ;i < printerInfos.length() ;i++){
        printerInfo = printerInfos.at(i);
        if(printerInfo.printer.isDefault){
            index_of_defaultprinter =  i;
        }
        printers << printerInfo.printer.name;
        QTableWidgetItem* item;
        item = new QTableWidgetItem(tr("%1").arg(QString::fromLocal8Bit(printerInfo.printer.name)));
        ui->tableWidget_printers->setItem(i ,base+0 ,item);

//        item = new QTableWidgetItem(tr("%1").arg(get_Status_string(printerInfo.status)));
//        ui->tableWidget_printers->setItem(i ,base+1,item);
        item = new QTableWidgetItem(tr("%1").arg(QString::fromLocal8Bit(printerInfo.printer.makeAndModel)));
        ui->tableWidget_printers->setItem(i ,base+2,item);
        item = new QTableWidgetItem(tr("%1").arg(printerInfo.printer.connectTo));
        ui->tableWidget_printers->setItem(i ,base+3,item);
//        gUInterface->setCmd(UIConfig::CMD_GetStatus ,printerInfo.printer.name);
    }
#else
//    printerlist = data.value<QList<Printer_struct> >();
    printerlist = data.value<QList<PrinterInfo_struct> >();
    Printer_struct printer;
    int base = 0;
    ui->tableWidget_printers->setRowCount(printerlist.length());
    printers.clear();
    int index_of_online_printer = -1;
    int index_of_defaultprinter = 0;
    for(int i = 0 ;i < printerlist.count() ;i++){
        if(UIConfig::isAutoShow(printerlist[i].status.PrinterStatus)){
            show();
        }

        printer = printerlist[i].printer;
        if(printer.isConnected){
            if(index_of_online_printer < 0)
                index_of_online_printer = i;
        }
        if(printer.isDefault){
            index_of_defaultprinter =  i;
        }
        printers << printer.name;

//        ui->tableWidget_printers->setColumnCount(4);
        QBrush brush;
        int type = UIConfig::GetStatusTypeForUI(printer.status);
        QIcon icon;
        switch (type) {
        case UIConfig::Status_Error:
            brush = QBrush(QColor(Qt::red));
            icon = QIcon(":/Images/error1.png");
            break;
        case UIConfig::Status_Warning:
            brush = QBrush(QColor(Qt::yellow));
            icon = QIcon(":/Images/warning1.png");
            break;
        case UIConfig::Status_Offline:
            brush = QBrush(QColor(Qt::gray));
            icon = QIcon(":/Images/offline1.png");
            break;
        case UIConfig::Status_Ready:
        case UIConfig::Status_Sleep:
        case UIConfig::Status_Busy:
        default:
            brush = QBrush(QColor(Qt::darkBlue));
            icon = QIcon(":/Images/ready1.png");
            break;
        }

        QString str = UIConfig::GetStatusTypeString(type);
        QTableWidgetItem* item;
        item = new QTableWidgetItem(tr("%1").arg(str));
        item->setForeground(brush);
        item->setIcon(icon);
//        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_printers->setItem(i ,base+0 ,item);

//        QWidget *widget;
//        QHBoxLayout *hLayout;
//        QLabel* label;
//        label = new QLabel;
//        label->setText(str);
//        hLayout = new QHBoxLayout();
//        widget = new QWidget();
//        hLayout->addWidget(label);
//        hLayout->setMargin(0);
//        hLayout->setAlignment(label, Qt::AlignCenter);
//        widget->setLayout(hLayout);
//        ui->tableWidget_printers->setCellWidget(i ,base+0 ,widget);

        item = new QTableWidgetItem(tr("%1").arg(printer.name));
        item->setForeground(QBrush(Qt::blue));
//        item->setTextAlignment(Qt::AlignCenter);
        item->setToolTip(printer.name);
        ui->tableWidget_printers->setItem(i ,base+1,item);
        if(printer.toner < 0){
            item = new QTableWidgetItem("-");
        }else{
            item = new QTableWidgetItem(tr("%1%").arg(printer.toner));
        }
//        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_printers->setItem(i ,base+2,item);
        if(printer.drum < 0){
            item = new QTableWidgetItem("-");
        }else{
            item = new QTableWidgetItem(tr("%1%").arg(printer.drum));
        }
//        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_printers->setItem(i ,base+3,item);
//        gUInterface->setCmd(UIConfig::CMD_GetStatus ,printer.name);
    }
#endif

    if(printers.isEmpty())
    {

        LOGLOG("no printers");
        setcurrentPrinter(QString());
//        gUInterface->setTimer(0);
//        if(bCheckPrinter){
//            hide();
//            QMessageBox message_box;
//            message_box.setButtonText(QMessageBox::Ok ,"确定");
//            message_box.setIcon(QMessageBox::Critical);
//            message_box.setText("系统中未安装打印机。");
//            message_box.setWindowTitle(" ");
//            message_box.exec();
//            exit(0);
//        }
        return;
    }
    else if(printers.contains(current_printer))
    {
//        setcurrentPrinter(current_printer);
//         bCheckPrinter = false;
    }else if(index_of_online_printer >= 0){
//        bCheckPrinter = false;
        setcurrentPrinter(printers.at(index_of_online_printer));
    }else{
//         bCheckPrinter = false;
          setcurrentPrinter(printers.at(index_of_defaultprinter));
    }
//    gUInterface->setCurrentPrinterCmd(UIConfig::CMD_GetStatus);
}

void MainWindow::updateCurrentPrinterStatus(const QString& printername)
{
    Printer_struct* printer;
    for(int i = 0 ;i < printerlist.count() ;i++){
        printer = &printerlist[i].printer;
        if(!printername.compare(printer->name)){
            updateStatus(printerlist[i]);
        }
    }
}

void MainWindow::updateStatus(const PrinterInfo_struct& printer)
{
    int type = UIConfig::GetStatusTypeForUI(printer.status.PrinterStatus);
    switch (type) {
    case UIConfig::Status_Ready:
    case UIConfig::Status_Sleep:
    case UIConfig::Status_Busy:
        ui->label_status_image->setPixmap(QPixmap(QString::fromUtf8(":/Images/ready2.png")));
//        ui->label_status_image->hide();
        break;
    case UIConfig::Status_Warning:
        ui->label_status_image->setPixmap(QPixmap(QString::fromUtf8(":/Images/warning2.png")));
//        ui->label_status_image->show();
        break;
    case UIConfig::Status_Offline:
        ui->label_status_image->setPixmap(QPixmap(QString::fromUtf8(":/Images/offline2.png")));
        break;
    case UIConfig::Status_Error:
    default:
        ui->label_status_image->setPixmap(QPixmap(QString::fromUtf8(":/Images/error2.png")));
//        ui->label_status_image->show();
        break;
    }

    QString str = UIConfig::getErrorMsg(printer.status.PrinterStatus, 0, false);
    if(!printer.printer.isConnected){
        if(QString(printer.printer.deviceUri).startsWith("usb://")){
            str = tr("ResStr_Offline_usb");
        }else{
            str = tr("ResStr_Offline_net");
        }
    }
    ui->label_status_string->setText(str);
    if(str.isEmpty()){
        ui->label_status_image->hide();
    }else{
        ui->label_status_image->show();
    }

    if(printer.status.TonelStatusLevelK<11)
    {
        trayIcon->setIcon(QIcon(":/Images/app_gray.png"));
    }
    else
    {
        trayIcon->setIcon(QIcon(":/Images/app.png"));
    }
//    if(UIConfig::isAutoShow(printer.status.PrinterStatus)){
//        show();
//    }
}

void MainWindow::on_tableWidget_printers_itemSelectionChanged()
{
    QTableWidgetItem *item = ui->tableWidget_printers->currentItem();
    if(item){
        int row = item->row();
        setcurrentPrinter(ui->tableWidget_printers->item(row ,1)->text());
    }else{
        setcurrentPrinter(QString());
    }
}
