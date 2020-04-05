#ifndef FLOYD_H
#define FLOYD_H

#include <QString>
#include <QList>

struct FloydInfo
{
   int startNo;
   int endNo;
   double weight;
};

class Floyd
{
//    Q_OBJECT
public:
    Floyd(int vexnums, QList<FloydInfo>);

    ~Floyd();
    //判断每次输入的边是否合法
    //顶点从1开始编号
    bool check_edge_value(int start,int end,double weight);
    //创建图
    void createGraph();
    //打印邻接矩阵
    void print();
    //求最短路径
    void floyds();
    //打印最短路径
    void printpath();
//    //获取输入输出的最短路径
//    QList<QString> getResult(int,int);

//    bool checkStartAndEnd(int,int);

    void printForm1();
    void printForm2();


    double **dis;//记录各顶点最短路径信息
    int **path;//记录各个最短路径信息

private:
    //Floyd算法
    int vexnum;//图顶点个数
    int edge;//图的边数
    double **arc;//邻接矩阵

    QList<FloydInfo> floydInfo;

};

#endif // FLOYD_H
