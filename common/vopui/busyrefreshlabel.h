#ifndef BUSYREFRESHLABEL_H
#define BUSYREFRESHLABEL_H

#include <QWidget>

class BusyRefreshLabel : public QWidget
{
    Q_OBJECT
public:
    explicit BusyRefreshLabel(QWidget *parent = 0, bool bBusy = false) ;
    void setMinLoops(int loops = 3);
    /*
     * @brief startAnimation start the Animation
     * @param interval   unit is  milliseconds
    */
    void startAnimation(int interval = 25);
    void stopAnimation();

signals:
    void runMinLoopsEnough();
protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *event);

private:
    int timerID;
    int minLoops;
    QPixmap m_pixmap;
    qreal m_rotation;
};

#endif // BUSYREFRESHLABEL_H
