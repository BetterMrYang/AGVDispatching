#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView
{
public:
    MyGraphicsView();
    qreal scaleFactor;
    qreal realScaling;

private:
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void magnify();
    void shrink();
    void scaling(qreal scaleFactor);
};

#endif // MYGRAPHICSVIEW_H
