#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QFrame>
class MyGraphicsView : public QGraphicsView
{
public:
    MyGraphicsView();
    MyGraphicsView(QFrame*);



private:
    qreal scaleFactor;
    qreal realScaling = 1;

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void magnify();
    void shrink();
    void scaling(qreal scaleFactor);
};

#endif // MYGRAPHICSVIEW_H
