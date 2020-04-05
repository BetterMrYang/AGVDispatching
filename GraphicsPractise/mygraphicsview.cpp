#include "mygraphicsview.h"
#include <QWheelEvent>
#include <QDebug>
MyGraphicsView::MyGraphicsView()
{
    realscaling = 1.0;
    this->installEventFilter(this);
    flag_scaleChange = false;
//    this->setMouseTracking(true);
}

//进行放缩后必须考虑鼠标点击位置跟场景坐标偏移

void MyGraphicsView::magnify()
{
    if(realscaling >= 2.4) //放大5次
    {
        return;
    }
    scalingOffset = 1;
    scalingOffset *= 1.2;
    realscaling*=1.2;
    scaling(scalingOffset);
//    flag_scaleChange = false;
    qDebug() << "放大"<< realscaling;
}

void MyGraphicsView::shrink() //缩小4次
{
    if(realscaling <= 0.52)
    {
        return;
    }
    scalingOffset = 1;
    scalingOffset/=1.2;
    realscaling/=1.2;
    scaling(scalingOffset);
    qDebug() << "缩小"<< realscaling;
}

void MyGraphicsView::scaling(qreal scaleFactor)
{
    scale(scaleFactor,scaleFactor);
    flag_scaleChange = true;
}

void MyGraphicsView::wheelEvent(QWheelEvent *event)
{
    QPoint sroll=event->angleDelta();
    sroll.y()>0?magnify():shrink();

}

qreal MyGraphicsView::getRealScaling()
{
    return realscaling;
}

bool MyGraphicsView::getFlagScaleChange()
{
    return flag_scaleChange;
}

void MyGraphicsView::setFlagScaleChange(bool flag)
{
    flag_scaleChange = flag;
}
