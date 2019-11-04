#include "busyrefreshlabel.h"
#include <QPainter>
#include <QTimerEvent>

BusyRefreshLabel::BusyRefreshLabel(QWidget *parent, bool bBusy) :
    QWidget(parent)
{
    m_rotation = 0.0;
    timerID = 0;
    if(bBusy)
    {
        m_pixmap = QPixmap(":/Images/busyRefresh.tif");
    }
    else
    {
        m_pixmap = QPixmap(":/Images/Status_RefreshEnable.tif");
    }


    this->setStyleSheet("border:transparent;");
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->hide();
}

void BusyRefreshLabel::setMinLoops(int loops)
{
    minLoops = loops;
}

void BusyRefreshLabel::startAnimation(int interval)
{
    if(timerID == 0){
        timerID = startTimer(interval);
    }
    this->show();
}

void BusyRefreshLabel::stopAnimation()
{
    killTimer(timerID);
    timerID = 0;
    this->hide();
}

void BusyRefreshLabel::timerEvent(QTimerEvent * event)
{
    if(event->timerId() == timerID){
        m_rotation += 5;
        update();

        if(m_rotation > minLoops*360){
            emit runMinLoopsEnough();
        }
    }
}

void BusyRefreshLabel::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    QPointF center(m_pixmap.width() / qreal(2), m_pixmap.height() / qreal(2));

    painter.translate(center);
    painter.rotate(m_rotation);
    painter.translate(-center);

    painter.drawPixmap(QPointF(0, 0), m_pixmap);
}
