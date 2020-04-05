#include "floyd.h"
#include <QDebug>
#include <QMessageBox>
#include <QString>
Floyd::Floyd(int vexnums, QList<FloydInfo> floydInfos)
{
    floydInfo = floydInfos;
    vexnum = vexnums;
    edge = floydInfo.size();
    arc=new double*[vexnum];
    dis=new double*[vexnum];
    path=new int*[vexnum];
    for(int i=0;i<vexnum;i++)
    {
        arc[i]=new double[vexnum];
        dis[i]=new double[vexnum];
        path[i]=new int[vexnum];
        for(int k=0;k<vexnum;k++)
        {
            //邻接矩阵无穷大
            arc[i][k]=INT_MAX;
        }

    }

    createGraph();
//    print();
    floyds();
//    printpath();
    printForm1();
    printForm2();
}

 Floyd::~Floyd()
{
     for(int i=0;i<vexnum;i++)
     {
         delete  arc[i];
         delete  dis[i];
         delete  path[i];

     }
     delete  arc;
     delete  dis;
     delete  path;
}

 bool Floyd::check_edge_value(int start,int end,double weight)
 {
     if(start < 0 || end < 0 || start > vexnum || end > vexnum || weight < 0)
     {
//         QMessageBox::warning(this,tr(""),tr(""));
         return false;
     }
     return  true;
 }

 void Floyd::createGraph()
 {
     int start;
     int end;
     double weight;
     int count=0;
     while(count != edge)
     {
          start = floydInfo.at(count).startNo;
          end = floydInfo.at(count).endNo;
          weight = floydInfo.at(count).weight;
          qDebug() << start << end <<weight<<"一组信息输入完毕";
         //首先判断输入信息是否合法
         while(!check_edge_value(start,end,weight))
         {
             qDebug() << "零阶矩阵不成功";
         }
         //邻接矩阵对应点赋值
         arc[start][end] = weight;
         count++;
         qDebug() << count << edge;
     }
     qDebug()<<"输入完毕";
 }


 void Floyd::print()
 {
     qDebug()<<"图的邻接矩阵为：";
     int count_row = 0;
     int count_col = 0;
     while(count_row != vexnum)
     {
         count_col = 0;
         QStringList strlist;
         while(count_col != vexnum)
         {
             if(arc[count_row][count_col] == INT_MAX)
             {
                 strlist.append("∞");
             }
             else
             {
                 strlist.append(QString::number(arc[count_row][count_col]));
             }
              ++count_col;
         }
         qDebug() << strlist;
         qDebug()<<"\r\n";
         count_row++;
     }
 }

 void Floyd::floyds()
 {
     int row = 0;
     int col = 0;
     for(row = 0;row < vexnum;row++)
     {
         for(col = 0;col < vexnum;col++)
         {
             //把矩阵初始化为邻接矩阵
             dis[row][col] = arc[row][col];
             path[row][col] = col;
         }
     }

     int temp=0;
     int select=0;
     for(temp = 0;temp < vexnum;temp++)
     {
         for(row = 0;row < vexnum;row++)
         {
             for(col = 0;col < vexnum;col++)
             {
//
//                 if(row == col)
//                 {
//                     dis[row][col] = 0;
//                     continue;
//                 }
                 //为了防止溢出，引入selec
                 select=(dis[row][temp] == INT_MAX||dis[temp][col] == INT_MAX)?INT_MAX:(dis[row][temp]+dis[temp][col]);
                 if(dis[row][col]>select)
                 {
                     //更新D矩阵
                     dis[row][col]=select;
                     path[row][col]=path[row][temp];
                 }
             }
         }
     }

     //将自己到自己变为0
     for(int row = 0;row < vexnum;row++)
     {
         for(int col = 0;col < vexnum;col++)
         {
             if(row == col)
             {
                 dis[row][col] = 0;
                 path[row][col] = col;
             }
         }
     }

 }

 void Floyd::printpath()
 {
     qDebug()<<"各个顶点对的最短路径："<<"\r\n";
     int row = 0;
     int col = 0;
     int temp = 0;
     for(row = 0;row < vexnum;row++)
     {
         QString str;
         for(col=row+1;col<vexnum;col++)
         {
             str="v"+QString::number(row) +"----"+"v"+QString::number(col) +"，"+"weight"+QString::number(dis[row][col]) +"，"+"path:"+"v"+QString::number(row);
 //            qDebug()<<"v"<<row+1<<"----"<<"v"<<col+1<<"weight"<<dis[row][col]<<"path:"<<"v"<<row+1;
             temp=path[row][col];
             while(temp!=col)
             {
                 str.append("-->v");
                 str.append(QString::number(temp));
 //                qDebug()<<"-->"<<"v"<<temp+1;
                 temp=path[temp][col];
             }
             str.append("-->v");
             str.append(QString::number(col));
 //           qDebug()<<"-->""v"<<col+1;
             qDebug()<<str<<"\r\n";
         }
          qDebug()<<"\r\n";
     }
 }

// QList<QString> Floyd::getResult(int starts,int ends)
// {
//     int start=starts;
//     int end=ends;
//     while(!checkStartAndEnd(start,end))
//     {
//         QMessageBox::warning(this,tr("错误"),tr("输入输出为同一点，请重新输入"));
//         int newStart=QInputDialog::getInt(this,tr("tips"),tr("新的起点"));
//         int newEnd=QInputDialog::getInt(this,tr("tips"),tr("新的终点"));
//         start=newStart;
//         end=newEnd;
//     }
//     if(start>end)
//     {
//         int temp=0;
//         temp=end;
//         end=start;
//         start=end;
//         qDebug()<<"交换起始/终点";
//     }

//     QList<QString> FloydRoute;
// //    int col=0;
//     int temp=0;
//     FloydRoute.append(QString::number(starts));
// //    for(col=start+1;col<vexnum;col++)//保证正向求路线
// //    {
//         temp=path[start][end];
//         while(temp!=end)
//         {
//            FloydRoute.append(QString::number(temp));
//            temp=path[temp][end];

//         }
// //    }
//     FloydRoute.append(QString::number(ends));
//     return FloydRoute;
// }

// bool Floyd::checkStartAndEnd(int start,int end)
// {
//    if(start==end)
//        return false;
//    else
//        return true;
// }

 void Floyd::printForm1()
 {

     for(int row = 0;row < vexnum;row++)
     {
         QStringList form;
         for(int col = 0;col < vexnum;col++)
         {
             form.append(QString::number(dis[row][col]));
         }
         qDebug() << "第" << row << "行" << form;
     }
 }

 void Floyd::printForm2()
 {
     for(int row = 0;row < vexnum;row++)
     {
         QStringList form;
         for(int col = 0;col < vexnum;col++)
         {
             form.append(QString::number(path[row][col]));
         }
         qDebug() << "第" << row << "行" << form;
     }
 }



