#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QMouseEvent>

class MyGraphicsView : public QGraphicsView
{

//    Q_OBJECT
public:
    MyGraphicsView();

    qreal getRealScaling();

    bool getFlagScaleChange();

    void setFlagScaleChange(bool);



protected:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void magnify();
    void shrink();
    void scaling(qreal scaleFactor);

private:
    qreal realscaling;
    qreal scalingOffset;
    bool flag_scaleChange;

signals:

};

#endif // MYGRAPHICSVIEW_H
