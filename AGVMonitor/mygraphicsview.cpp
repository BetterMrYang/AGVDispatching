#include "mygraphicsview.h"
#include <QWheelEvent>
#include <QDebug>
MyGraphicsView::MyGraphicsView()
{
//    this->installEventFilter(this);
//    realScaling = 1;
}

MyGraphicsView::MyGraphicsView(QFrame* frame)
{
//    this->setParent(frame);
}

void MyGraphicsView::magnify()
{

    if(realScaling >= 2.4) //放大5次
    {
        return;
    }
    scaleFactor = 1;
    scaleFactor *= 1.2;
    realScaling*=1.2;
    scaling(scaleFactor);
     qDebug() << "放大" << realScaling;
}

void MyGraphicsView::shrink() //缩小5次
{

    if(realScaling <= 0.4)
    {
        return;
    }
    scaleFactor = 1;
    scaleFactor/=1.2;
    realScaling/=1.2;
    scaling(scaleFactor);
    qDebug() << "缩小" << realScaling;
}

void MyGraphicsView::scaling(qreal scaleFactor)
{
    scale(scaleFactor,scaleFactor);
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    QPoint sroll=event->angleDelta();
    sroll.y()>0?magnify():shrink();
}

