#include "mydijkstra.h"
#include <QInputDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>

myDijkstra::myDijkstra(QObject *parent,const int vexnums,const int edges)
    : QObject(parent),vexnum(vexnums),edge(edges)
{
    arc=new double*[vexnum];
    dis=new Dis[vexnum];
    for(int i=0;i<vexnum;i++)
    {
        arc[i]=new double[vexnum];
        for(int k=0;k<vexnum;k++)
        {
            //邻接矩阵初始化为无穷大
            arc[i][k]=INT_MAX;
        }
    }
}

myDijkstra::~myDijkstra()
{
    delete [] dis;
    for(int i=0;i<vexnum;i++)
    {
        delete arc[i];
    }
    delete arc;
}


// 判断我们每次输入的的边的信息是否合法
//顶点从1开始编号
bool myDijkstra::check_edge_value(int start,int end,double weight)
{
    if(start<0 || end<0|| start > vexnum-1 || end>vexnum-1 || weight<0)
    {
        return false;
    }
    return true;
}


void myDijkstra::createGraph(const QList<DijkstraInfo>& infos)
{
    for(auto info : infos)
    {
        if(check_edge_value(info.start_num,info.end_num,info.length))
        {
            arc[info.start_num][info.end_num] = info.length;
        }
        else
        {
            qDebug() << "创建新地图检验不通过";
            return;
        }

    }
    int count_row = 0; //打印行的标签
    int count_col = 0; //打印列的标签
    //开始打印
    while (count_row != this->vexnum) {
        count_col = 0;
        QString str;
        while (count_col != this->vexnum) {
            if (arc[count_row][count_col] == INT_MAX)
            {
              str = str + "∞ ";
            }
            else
            {
               str = str + QString::number(arc[count_row][count_col]) + " ";
            }

            ++count_col;
        }
        qDebug() << str;
        ++count_row;
    }

}

void myDijkstra::Dijkstra(int begin)
{
    //初始化dis数组
    int i;
    for(i=0;i<vexnum;i++)
    {
        //设置当前路径
        dis[i].path=QString::number(begin)+ "," +QString::number(i);
        dis[i].value=arc[begin][i];
    }
    //设置起点到起点的路径为0
    dis[begin].value=0;
    dis[begin].visit=true;

    int count=1;
    //计算剩余的顶点的最短路径（剩余this->vexnum-1个顶点
    while(count!=vexnum)
    {
     //temp用于保存当前dis数组中最小的那个下标
     //min记录的当前的最小值
        int temp=0;
        double min=INT_MAX;
        for(i = 0;i < vexnum;i++)
        {
            if(!dis[i].visit && dis[i].value < min)
            {
                min = dis[i].value;
                temp=i;
            }
        }
        //把temp对应的顶点加入到已经找到的最短路径的集合中
        dis[temp].visit=true;
        ++count;
        for(i=0;i<vexnum;i++)
        {
            if(!dis[i].visit && arc[temp][i] != INT_MAX && (dis[temp].value+arc[temp][i]<dis[i].value))
            {
                 //如果新得到的边可以影响其他为访问的顶点，那就就更新它的最短路径和长度
                dis[i].value=dis[temp].value+arc[temp][i];
                dis[i].path=dis[temp].path + "," +QString::number(i);
                qDebug() << "更新" << i << "dis[temp].path" <<  dis[temp].path << "dis[i].path" << dis[i].path;
            }
        }

    }
}

//成功返回
QString myDijkstra::getNewRoute(int begin,int end,const QList<DijkstraInfo>& infos)
{
    createGraph(infos);
    Dijkstra(begin);
    qDebug()<< QString::number(begin) <<"到"<<QString::number(end)<<"最短路径为" << dis[end].path << "距离为" << dis[end].value;
    return dis[end].path;

}
