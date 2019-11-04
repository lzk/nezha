#ifndef UINTERFACE_H
#define UINTERFACE_H

#include <QObject>
#include <QThread>
#include <QVariant>
class Worker;
class UInterface : public QObject
{
    Q_OBJECT
public:
    explicit UInterface(QObject *parent = 0);
    ~UInterface();
    void setCmd(int cmd ,const QString& printer_name ,QVariant data = QVariant());
    void setTimer(int timeVal = 0);
    void setcurrentPrinter(const QString& str);//{current_printer = str;}
    void setCurrentPrinterCmd(int cmd ,QVariant data = QVariant());
    void cancel_work();
    
signals:
    void cmdToWorker(int cmd ,const QString& printer_name = QString() ,QVariant data = QVariant());
    void cmdResult(int cmd,int result ,QVariant data=QVariant());

private:
    Worker* worker;
    QThread thread;
    QString current_printer;

};

extern UInterface* gUInterface;
#endif // UINTERFACE_H
