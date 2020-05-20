#ifndef MYDIJKSTRA_H
#define MYDIJKSTRA_H
#include <QObject>
#include "include.h"

struct Dis{
    QString path;
    double value;
    bool visit;
    Dis(){
        visit=false;
        value=0;
        path="";
    }
};

class myDijkstra : public QObject
{
    Q_OBJECT
public:
    explicit myDijkstra(QObject *parent = nullptr,const int vexnums = 0,const int edges = 0);
    virtual ~myDijkstra();
    QString getNewRoute(int begin,int end,const QList<DijkstraInfo>& infos);

private:

    // 判断我们每次输入的的边的信息是否合法
    bool check_edge_value(int start,int end,double weight);
    //创建图
    void createGraph(const QList<DijkstraInfo>& infos) ;
    //求最短路径
    void Dijkstra(int begin);
    //求指定两点的最短路径



private:
    int vexnum;//图的顶点个数
    int edge;//图的边数
    double **arc;//邻接矩阵
    Dis *dis; //记录各个顶点最短路径的信息

signals:

public slots:
};

#endif // MYDIJKSTRA_H
