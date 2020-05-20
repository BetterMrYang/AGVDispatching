#include "mydatabase.h"
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QThread>


MyDatabase::MyDatabase()
{
//    connectName = connnectionName;
//    createConnection();
}

void MyDatabase::createDefaultConnection()
{
//    hostName = "127.0.0.1";   // 主机名
//    dbName = "agvdispatching";   // 数据库名称
//    userName = "Nuaa";   // 用户名
//    password = "17504";   // 密码

//    dbconn = QSqlDatabase::addDatabase("QMYSQL",QString("%1").arg(connectName));
    dbconn = QSqlDatabase::addDatabase("QMYSQL");
    dbconn.setHostName(hostName);
    dbconn.setDatabaseName(dbName);
    dbconn.setPort(3306);
    dbconn.setUserName(userName);
    dbconn.setPassword(password);
    dbconn.open();

    qDebug("database open status: %d\n", dbconn.open());
    QSqlError error = dbconn.lastError();
    qDebug() << error.text();
    qDebug() << dbconn.tables();
    qDebug() << "连接名————" << dbconn.connectionName();
     qDebug()<<"MyDatabase::createConnection:"<<QThread::currentThreadId();
}

void MyDatabase::createConnection(QString name)
{
//    hostName = "127.0.0.1";   // 主机名
//    dbName = "agvdispatching";   // 数据库名称
//    userName = "Nuaa";   // 用户名
//    password = "17504";   // 密码

//    dbconn = QSqlDatabase::addDatabase("QMYSQL",QString("%1").arg(connectName));
    dbconn = QSqlDatabase::addDatabase("QMYSQL",name);
    dbconn.setHostName(hostName);
    dbconn.setDatabaseName(dbName);
    dbconn.setPort(3306);
    dbconn.setUserName(userName);
    dbconn.setPassword(password);
    if(dbconn.open())
    {
        qDebug() << "连接名————" << dbconn.connectionName();
        qDebug()<<"MyDatabase::createConnection:"<<QThread::currentThreadId();
    }

    qDebug("database open status: %d\n", dbconn.open());
    QSqlError error = dbconn.lastError();
    qDebug() << error.text();
    qDebug() << dbconn.tables();

//    dbconn.close();
}

void MyDatabase::closeConnection(QString name)
{
//    qDebug() << "关闭数据库";
    dbconn.close();
    QSqlDatabase::removeDatabase(name);
}


/*****************************************************数据库操作函数说明*************************************************************
 * MYSQL
 * 注：数据库所有表名称、字段采用英文命名、所有数据都采用字符串类型,MYSQL中datetime类型需在QT预处理后写入
 *
 *
 *
 * 参数说明：tabName为表名称，clause为条件，content为操作参数链表
tabName:
----------------------------------------------------------------------------------------------------------------------------------
     1."agvRunStatus":   AGV运行状态表(20)       指令类型，AGV编号,设备类型,电量,障碍物状态,运行状态，路径端号，工位点号，交叉点号，故障状态，任务生成编号，任务执行编号，任务状态，物料编号，
                                                装卸动作编号，装卸异常，充电动作编号，充电异常，X坐标，Y坐标
                                                               CommandType，AGVNo,DeviceType,AGVPower,AGVBarrierStatus,AGVRunningStatus,AGVRouteNo,AGVStationNo,AGVCrossNo,
                                                               AGVFaultStatus, TaskCreateNo, TaskExecuteNo, TaskStatus, MaterialNo,LoadActionNo, LoadError,ChargeActionNo,ChargeError，XPosition,YPosition
        指令类型: 4、AGV状态反馈
        AGV障碍物状态: 0无障碍，1远障碍，2近障碍，3碰撞
        装卸动作编号:1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
        装卸异常：1无异常，2装载异常，3卸载异常
        充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
        充电异常：1无异常，2充电异常
----------------------------------------------------------------------------------------------------------------------------------
OK   2."otherDeviceStatus":   其他设备运行状态表(7)      字段：指令类型，设备编号，设备类型，装卸动作编号，装卸异常，充电动作编号，充电异常
                                                             CommandType,DeviceNo,DeviceType,LoadActionNo,LoadError,ChargeActionNo,ChargeError
     注：
        指令类型: 8、其他设备状态反馈
        装卸动作：1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
        装卸异常：1无异常，2装载异常，3卸载异常
        充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
        充电异常：1无异常，2充电异常

----------------------------------------------------------------------------------------------------------------------------------
OK   3."crossPointStatus":  交叉点状态数组（4）            字段：交叉点编号，交叉点状态，占用AGV号，交叉点可读性
                                                        CrossNo，  CrossStatus， OccupyAGVNo， CrossReadability
     注：
        交叉点状态：1、占用，2、空闲
        占用AGV编号：当交叉点被占用时，存储占用该交叉点的AGV编号
----------------------------------------------------------------------------------------------------------------------------------
OK   4."routeStatus":  路径段数组（4）         字段：路径端编号，单双向使能，AGV数量，AGV编号
     注：                                      RouteNo， DirectionEnable， AGVNum，AGVNo
        单双向使能：0：禁止；1：正向可行；2：反向可行；3：双向可行
        AGV编号：按AGV进入路径段的顺序排列，先入先出

----------------------------------------------------------------------------------------------------------------------------------
OK   5."stationPointStatus": 工位点状态数组（4）            字段：工位点编号,工位点状态,占用AGV编号,工位点可读性
                                                   StationNo, StationStatus,  OccupyAGVNo, StationReadability
  工位点状态:1、占用，2、空闲
  占用AGV编号:当工位点被占用时，存储占用该工位点的AGV编号

----------------------------------------------------------------------------------------------------------------------------------
OK 显示  6."taskExecute"：任务执行状态（15）           字段：任务生成编号，任务类型，任务执行编号，搬运AGV编号，任务状态，物料编号，路径段编号，路径端点编号
                                                 工位点编号，装卸动作编号，装卸异常，充电动作编号，充电异常，任务执行时间，任务里程，任务剩余时间
                                                      TaskCreateNo，TaskType，TaskExecuteNo，ExecuteAGVNo， TaskStatus，MaterialNo，RouteNo，
                                                        RoutePointNo， StationNo，LoadActionNo，LoadError，ChargeActionNo，TaskExecuteTim，TaskDistance， TaskRemainTime
       注：任务类型：1装卸载任务，2充电任务，此时物料编号和移载方式均为0。
          装卸动作：1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
          装卸异常：1无异常，2装载异常，3卸载异常
          充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
          充电异常：1无异常，2充电异常
          任务剩余时间：任务时间预测-任务执行时间

----------------------------------------------------------------------------------------------------------------------------------
OK  显示  7.“taskNew”：新产生（12）                   字段：任务生成编号,任务类型,起点工位编号,起点移载方式,物料编号,终点工位编号,终点移载方式,
                                               任务生成时间,任务执行编号,搬运AGV编号,任务状态,任务时间预测
                                                 TaskCreateNo,TaskType,StartStationNo,StartTransferMode, MaterialNo, EndStationNo,EndTransferMode,TaskCreateTime,
                                                 TaskExecuteNo, ExecuteAGVNo,TaskStatus,TaskTimePredict
       注：任务类型：1装卸载任务，2充电任务，此时物料编号和移载方式均为0。
           移载方式：1人工，2顶升式、3辊道式、4推挽式
           任务执行编号：已确定
           搬运AGV编号：已分配
           任务状态：2调度

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 8.“taskError”：任务异常属性（6）            字段：任务生成编号，任务类型，任务执行编号，搬运AGV编号，物料编号，异常状态
                                                          TaskCreateNo，TaskType，TaskExecuteNo，ExecuteAGVNo，MaterialNo，AbnormalStatus
       注：任务类型：1装卸载任务，2充电任务，此时物料编号和移载方式均为0。
          任务执行编号：已确定
          搬运AGV编号：已分配
          异常状态：1、AGV异常，2、装卸异常

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 9."agvRealTimeData" ：AGV运行实时数据（17）                 字段：AGV编号,设备类型,AGV电量,AGV障碍物状态,AGV运行状态,AGV路径段编号,AGV工位点编号,AGV交叉点编号,AGV故障状态,
                                                               任务生成编号,任务执行编号,任务状态,物料编号,装卸动作编号,装卸异常,充电动作编号,充电异常
                                                              AGVNo, DeviceType,AGVPower,AGVBarrierStatus,AGVRunStatus,AGVRouteNo,AGVStationNo,AGVCrossNo, AGVFaultStatus,
                                                              TaskCreateNo,TaskExecuteNo,TaskStatus,MaterialNo, LoadActionNo,LoadError,ChargeActionNo,ChargeError
        注：障碍物状态：0无障碍，1远障碍，2近障碍，3碰撞
           运行状态：1停止、2行驶、3装卸载、4充电
           任务状态：1生成、2调度、3执行、4完成、5异常
           装卸动作：1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
           装卸异常：1无异常，2装载异常，3卸载异常
           充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
           充电异常：1无异常，2充电异常

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 10.“agvRunStatics” ：AGV运行统计（6）           字段：AGV编号，设备类型，AGV累计任务数，AGV累计里程数，AGV累计负载数，AGV利用率
                                    AGVNo，DeviceType， TotalTaskNum，TotalDistanceNum，TotalLoadNum，AGVUtilization
        注：AGV利用率：装卸载任务时间/总时间

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 11.“materialDeliverStatus” :物料配送状态（8）         字段：物料编号，物料类型，任务生成编号，任务执行编号，搬运AGV编号，路径段编号，工位点编号，交叉点编号
                                              MaterialNo，MaterialType，TaskCreateNo，TaskExecuteNo，ExecuteAGVNo， RouteNo，StationNo，CrossNo

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 12."taskDispatched":已调度(同7)

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 13.“taskComplete”:已完成（同6）

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 14.“agvError”:异常agv（8）               字段：编号，设备类型，电量，运行状态，故障状态，任务状态，装卸异常，充电异常
                                                  AGVNo,DeviceType, AGVPower,AGVRunningStatus,AGVFaultStatus,TaskStatus,LoadError, ChargeError
        注：运行状态：1停止、2行驶、3装卸载、4充电
           任务状态：1生成、2调度、3执行、4完成、5异常
           装卸动作：1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
           装卸异常：1无异常，2装载异常，3卸载异常
           充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
           充电异常：1无异常，2充电异常

----------------------------------------------------------------------------------------------------------------------------------
OK  显示 15.otherDeviceError:其他设备异常（6）           字段：设备编号，设备类型，装卸动作号，装卸异常，充电动作号，充电异常
                                                        DeviceNo,DeviceType,LoadActionNo,LoadError,ChargeActionNo,ChargeError

         注：装卸动作：1、AGV停止，2、装载开始，3、装载结束，4、卸载开始，5、卸载结束，6、AGV启动
            装卸异常：1无异常，2装载异常，3卸载异常
            充电动作：1、AGV入站，2、AGV停止，3、充电桩接触，4、充电中，5、充电桩断开，6、AGV启动，7、AGV出站
            充电异常：1无异常，2充电异常
----------------------------------------------------------------------------------------------------------------------------------
OK  16.agvPosition:AGV位置反馈（5）                   字段：指令编号，AGV编号，X坐标，Y坐标,转弯状态
                                                          commandType(9),AGVNo,PosX,PosY，TuringStatus

----------------------------------------------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------------------------------------------------
OK

----------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------
clause: 条件，根据具体情况而定

******************************************************************************************************************************/

int MyDatabase::tabNameTotableNo(QString tabName)
{
    int liv_tableNo = 0;  //将tabName转换为数字
    if(tabName == "agvRunStatus")
        liv_tableNo = 1;
    else if(tabName == "otherDeviceStatus")
        liv_tableNo = 2;
    else if(tabName == "crossPointStatus")
        liv_tableNo = 3;
    else if(tabName == "routeStatus")
        liv_tableNo = 4;
    else if(tabName == "stationPointStatus")
        liv_tableNo = 5;
    else if(tabName == "taskExecute")
        liv_tableNo = 6;
    else if(tabName == "taskNew")
        liv_tableNo = 7;
    else if(tabName == "taskError")
        liv_tableNo = 8;
    else if(tabName == "agvRealTimeData")
        liv_tableNo = 9;
    else if(tabName == "agvRunStatics")
        liv_tableNo = 10;
    else if(tabName == "materialDeliverStatus")
        liv_tableNo = 11;
    else if(tabName == "taskDispatched")
        liv_tableNo = 12;
    else if(tabName == "taskComplete")
        liv_tableNo = 13;
    else if(tabName == "agvError")
        liv_tableNo = 14;
    else if(tabName == "otherDeviceError")
        liv_tableNo = 15;
    else if(tabName == "agvPosition")
        liv_tableNo = 16;
    else
    {
        qDebug() << "表格不存在";
    }
    return  liv_tableNo;

}


/*******************************************************插入数据库记录**************************************************************************/

void MyDatabase::insert_MySQL(QString tabName,QList<QString> content)
{
    QSqlQuery query(dbconn);
    int liv_tableNo = tabNameTotableNo(tabName);
    switch(liv_tableNo)
    {
       case 1:
              query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)").arg(tabName));
              for(int i = 0;i < 20;i++)
              {
                  query.bindValue(i,content.at(i));
              }
              if(!query.exec())
              {
                  qDebug() << "agvRunStatus插入失败" << query.lastError();
              }
              query.clear();
              break;
       case 2:
              query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?)").arg(tabName));
              for(int i = 0;i < 7;i++)
              {
                  query.bindValue(i,content.at(i));
              }
              if(!query.exec())
              {
                  qDebug() << "otherDeviceStatus插入失败" << query.lastError();
              }
              query.clear();
              break;
       case 3 : case 4 : case 5:
                                query.prepare(QString("insert into %1 values(?,?,?,?)").arg(tabName));
                                for(int i = 0;i < 4;i++)
                                {
                                    query.bindValue(i,content.at(i));
                                }
                                if(!query.exec())
                                {
                                   qDebug() << "routeStatus/crossPointStatus/stationStatus插入失败" << query.lastError();
                                }
                                query.clear();
                                break;
       case 6: case 13:
              query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)").arg(tabName));
              for(int i = 0;i < 15;i++)
              {
                 query.bindValue(i,content.at(i));
              }
              if(!query.exec())
              {
                 qDebug() << "taskExecute/“taskComplete”" << query.lastError();
              }
              query.clear();
              break;
       case 7: case 12:
              query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?,?,?,?,?,?)").arg(tabName));
              for(int i = 0;i < 12;i++)
              {
                 query.bindValue(i,content.at(i));
              }
              if(!query.exec())
              {
                 qDebug() << "taskNew/taskDispatched插入失败" << query.lastError();
              }
              query.clear();
              break;
        case 8: case 10: case 15:
                         query.prepare(QString("insert into %1 values(?,?,?,?,?,?)").arg(tabName));
                         for(int i = 0;i < 6;i++)
                         {
                              query.bindValue(i,content.at(i));
                         }
                         if(!query.exec())
                          {
                              qDebug() << "otherDeviceStatus/taskError/agvRunStatics插入失败" << query.lastError();
                          }
                          query.clear();
                          break;
       case 9:
              query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)").arg(tabName));
             for(int i = 0;i < 17;i++)
             {
                query.bindValue(i,content.at(i));
             }
             if(!query.exec())
             {
                qDebug() << "agvRealTimeData插入失败" << query.lastError();
             }
             query.clear();
             break;
      case 16:
             query.prepare(QString("insert into %1 values(?,?,?,?,?)").arg(tabName));
             for(int i = 0;i < 5;i++)
             {
                 query.bindValue(i,content.at(i));
             }
             if(!query.exec())
             {
                 qDebug() << "agvPosition" << query.lastError();
             }
             query.clear();
             break;
       default: //即11和14
               query.prepare(QString("insert into %1 values(?,?,?,?,?,?,?,?)").arg(tabName));
               for(int i = 0;i < 8;i++)
               {
                   query.bindValue(i,content.at(i));
               }
               if(!query.exec())
               {
                  qDebug() << "taskDispatched/otherDeviceError插入失败" << query.lastError();
               }
               query.clear();
    }

}

/****************************************************删除数据库记录*************************************************************/
void MyDatabase::delete_MySQL(QString tabName, int pkey)
{
    QSqlQuery query(dbconn);
    int liv_tableNo = tabNameTotableNo(tabName);
    switch(liv_tableNo)
    {
       case 1: case 9: case 10: case 14: case 16:
                     if(!query.exec(QString("delete from %1 where AGVNo = %2").arg(tabName).arg(pkey)))
                     {
                        qDebug() << QString("delete from %1 where AGVNo = %2").arg(tabName).arg(pkey) << query.lastError();
                     }
                     query.clear();
                     break;
      case 2: case 15:
            if(!query.exec(QString("delete from %1 where DeviceNo = %3").arg(tabName).arg(pkey)))
            {
                qDebug() << QString("delete from %1 where DeviceNo = %3").arg(tabName).arg(pkey) << query.lastError();
            }
            query.clear();
            break;
       case 3:
             if(!query.exec(QString("delete from %1 where CrossNo = %2").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("delete from %1 where CrossNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 4:
             if(!query.exec(QString("delete from %1 where RouteNo = %2").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("delete from %1 where RouteNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 5:
             if(!query.exec(QString("delete from %1 where StationNo = %2").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("delete from %1 where StationNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 6: case 7: case 8: case 12: case 13:
                             if(!query.exec(QString("delete from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey)))
                             {
                                qDebug() << QString("delete from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey) << query.lastError();
                             }
                             query.clear();
                             break;
       default://即11
               if(!query.exec(QString("delete from %1 where MaterialNo = %3").arg(tabName).arg(pkey)))
               {
                  qDebug() << QString("delete from %1 where MaterialNo = %3").arg(tabName).arg(pkey) << query.lastError();
               }
               query.clear();
               break;


    }

}

/****************************************************修改数据库记录*******************************************************************/
//更新部分字段
void MyDatabase::update_MySQL(QString tabName,QString content,int pkey)
{
    qDebug()<<"MyDatabase::update_MySQL:"<<QThread::currentThreadId();
    QSqlQuery query(dbconn);
    int liv_tableNo = tabNameTotableNo(tabName);
    switch(liv_tableNo)
    {
       case 1: case 9:case 10: case 14 :case 16:
                     if(!query.exec(QString("update %1 set %2 where AGVNo = %3").arg(tabName).arg(content).arg(pkey)))
                     {
                        qDebug() << QString("update %1 set %2 where AGVNo = %3 失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
                     }
                     query.clear();
                     break;
      case 2:case 15:
             if(!query.exec(QString("update %1 set %2 where DeviceNo = %3").arg(tabName).arg(content).arg(pkey)))
             {
                qDebug() << QString("update %1 set %2 where DeviceNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 3:
             if(!query.exec(QString("update %1 set %2 where CrossNo = %3").arg(tabName).arg(content).arg(pkey)))
             {
                qDebug() << QString("update %1 set %2 where CrossNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
             }
            query.clear();
            break;
       case 4:
             if(!query.exec(QString("update %1 set %2 where RouteNo = %3").arg(tabName).arg(content).arg(pkey)))
             {
                qDebug() << QString("update %1 set %2 where RouteNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 5:
             if(!query.exec(QString("update %1 set %2 where StationNo = %3").arg(tabName).arg(content).arg(pkey)))
             {
                qDebug() << QString("update %1 set %2 where StationNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
             }
             query.clear();
             break;
       case 6: case 7: case 8: case 12: case 13:
                             if(!query.exec(QString("update %1 set %2 where TaskCreateNo = %3").arg(tabName).arg(content).arg(pkey)))
                             {
                                qDebug() << QString("update %1 set %2 where TaskCreateNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
                             }
                             query.clear();
                             break;
       default:
               if(!query.exec(QString("update %1 set %2 where MaterialNo = %3").arg(tabName).arg(content).arg(pkey)))
               {
                   qDebug() << QString("update %1 set %2 where MaterialNo = %3失败").arg(tabName).arg(content).arg(pkey) << query.lastError();
               }
               query.clear();
               break;

    }

}

//更新一条记录
//对于已完成任务和已调度任务，只保留最近N条，故不能删除重新插入；其余表格以删除重新插入方式更新——————效率？？
void MyDatabase::update_record(QString tabName,QList<QString>& content,int pkey)
{
    QSqlQuery query(dbconn);
    int liv_tableNo = tabNameTotableNo(tabName);
    switch(liv_tableNo)
    {
        case 12:  //datetime类型需处理
                if(!query.exec(QString("update %1 set TaskCreateNo = %2,TaskType = %3,StartStationNo = %4,StartTransferMode = %5, MaterialNo = %6, EndStationNo = %7,"
                                       "EndTransferMode = %8,TaskCreateTime = %9,TaskExecuteNo = %10, ExecuteAGVNo = %11,TaskStatus = %12,TaskTimePredict = %13 where TaskCreateNo = %14")
                                       .arg(tabName).arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(content.at(3)).arg(content.at(4)).arg(content.at(5)).arg(content.at(6))
                                       .arg(content.at(7)).arg(content.at(8)).arg(content.at(9)).arg(content.at(10)).arg(content.at(11)).arg(content.at(12)).arg(content.at(13)).arg(pkey)))
                 {
                     qDebug() << QString("update %1  where TaskCreateNo = %2 失败").arg(tabName).arg(pkey) << query.lastError();
                 }
                 query.clear();
                 break;
        case 13: //datetime类型需处理
                if(!query.exec(QString("update %1 set TaskCreateNo = %2,TaskType = %3,TaskExecuteNo = %4,ExecuteAGVNo = %5,TaskStatus = %6, MaterialNo = %7, RouteNo = %8,"
                               "RoutePointNo = %9,StationNo = %10,LoadActionNo = %11, LoadError = %12,ChargeActionNo = %13,TaskExecuteTime = %14，TaskDistance = %15，"
                               "TaskRemainTime = %16 where TaskCreateNo = %17").arg(tabName).arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(content.at(3))
                               .arg(content.at(4)).arg(content.at(5)).arg(content.at(6)).arg(content.at(7)).arg(content.at(8)).arg(content.at(9)).arg(content.at(10))
                               .arg(content.at(11)).arg(content.at(12)).arg(content.at(13)).arg(content.at(14)).arg(pkey)))
                {
                    qDebug() << QString("update %1  where TaskCreateNo = %2 失败").arg(tabName).arg(pkey) << query.lastError();
                }
                query.clear();
                break;
        default:
                delete_MySQL(tabName,pkey);
                insert_MySQL(tabName,content);
//              if(!query.exec(QString("update %1 set CommandType = %2，AGVNo = %3,DeviceType = %4,AGVPower = %5,AGVBarrierStatus = %6,AGVRunningStatus = %7,"
//                                     "AGVRouteNo = %8,AGVStationNo = %9,AGVCrossNo = %10,AGVFaultStatus = %11,TaskCreateNo = %12,TaskExecuteNo = %13,"
//                                     "TaskStatus = %14,MaterialNo = %15,LoadActionNo = %16,LoadError = %17,ChargeActionNo = %18,ChargeError = %19，"
//                                     "XPosition = %20,YPosition = %21 where AGVNo = %22").arg(tabName).arg(content.at(0)).arg(content.at(1)).arg(content.at(2))
//                                     .arg(content.at(3)).arg(content.at(4)).arg(content.at(5)).arg(content.at(6)).arg(content.at(7)).arg(content.at(8)).arg(content.at(9))
//                                     .arg(content.at(10)).arg(content.at(11)).arg(content.at(12)).arg(content.at(13)).arg(content.at(14)).arg(content.at(15))
//                                     .arg(content.at(16)).arg(content.at(17)).arg(content.at(18)).arg(content.at(19)).arg(pkey)))
//              {
//                  qDebug() << QString("update %1 set %2 where AGVNo = %3 失败").arg(tabName).arg(content.at(0)).arg(pkey) << query.lastError();
//              }
//              query.clear();


    }
}

/*****************************************************************查询数据库(只查询需要显示的)****************************************************************/
QList<QString> MyDatabase::select_MySQL(QString tabName,int pkey)
{
   QList<QString> select_data;
   QSqlQuery query(dbconn);
   int liv_tableNo = tabNameTotableNo(tabName);
   switch(liv_tableNo)
   {
      case 1:
            if(!query.exec(QString("select * from %1 where AGVNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select* from %1 where AGVNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next()) //不能省略，下文用到了query.value，需要移动
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 18;i++)
            {
                select_data.append(query.value(i).toString());
            }
            query.clear();
            break;
      case 2:
             if(!query.exec(QString("select * from %1 where AGVNo = %2").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("select * from %1 where AGVNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next())
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 7;i++)
             {
                 select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
      case 3:
             qDebug() << "开始查询表3";
            if(!query.exec(QString("select * from %1 where CrossNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select * from %1 where CrossNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next())
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 4;i++)
            {
                select_data.append(query.value(i).toString());
            }
            query.clear();
            break;
      case 4:
            if(!query.exec(QString("select * from %1 where RouteNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select * from %1 where RouteNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next())
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 4;i++)
            {
                select_data.append(query.value(i).toString());
            }
            query.clear();
            break;
      case 5:
            if(!query.exec(QString("select * from %1 where StationNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select * from %1 where StationNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next())
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 4;i++)
            {
                select_data.append(query.value(i).toString());
            }

            query.clear();
            break;
      case 6: case 13:
            if(!query.exec(QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next())
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 15;i++)
            {
               select_data.append(query.value(i).toString());
            }
            query.clear();
            break;
      case 7: case 12:
            if(!query.exec(QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey)))
            {
               qDebug() << QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey) << query.lastError();
            }
            if(!query.next())
            {
                qDebug() << "未获取下哟个字段";
            }
            for(int i = 0;i < 12;i++)
            {
               select_data.append(query.value(i).toString());
            }
            query.clear();
            break;
      case 8:
             if(!query.exec(QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey)))
             {
                 qDebug() << QString("select * from %1 where TaskCreateNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next())
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 6;i++)
             {
                select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
      case 9:
             if(!query.exec(QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next())
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 17;i++)
             {
                select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
      case 10:
             if(!query.exec(QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey)))
             {
                 qDebug() << QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next())
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 6;i++)
             {
                 select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
      case 11:
              if(!query.exec(QString("select * from %1 where MaterialNo = %3").arg(tabName).arg(pkey)))
              {
                  qDebug() << QString("select * from %1 where MaterialNo = %3").arg(tabName).arg(pkey) << query.lastError();
              }
              if(!query.next())
              {
                  qDebug() << "未获取下哟个字段";
              }
              for(int i = 0;i < 8;i++)
              {
                 select_data.append(query.value(i).toString());
              }
              query.clear();
              break;
      case 14:
              if(!query.exec(QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey)))
              {
                 qDebug() << QString("select * from %1 where AGVNo = %3").arg(tabName).arg(pkey) << query.lastError();
              }
              if(!query.next())
              {
                 qDebug() << "未获取下哟个字段";
              }
              for(int i = 0;i < 8;i++)
              {
                 select_data.append(query.value(i).toString());
              }
              query.clear();
              break;
      case 15:
             if(!query.exec(QString("select * from %1 where DeviceNo = %3").arg(tabName).arg(pkey)))
             {
                 qDebug() << QString("select * from %1 where DeviceNo = %3").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next())
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 6;i++)
             {
                select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
      default: //即16
             if(!query.exec(QString("select * from %1 where AGVNo = %2").arg(tabName).arg(pkey)))
             {
                qDebug() << QString("select* from %1 where AGVNo = %2").arg(tabName).arg(pkey) << query.lastError();
             }
             if(!query.next()) //不能省略，下文用到了query.value，需要移动
             {
                 qDebug() << "未获取下哟个字段";
             }
             for(int i = 0;i < 5;i++)
             {
                 select_data.append(query.value(i).toString());
             }
             query.clear();
             break;
   }
   return select_data;
}

/*****************************************************************查询表记录数(只查询需要的)****************************************************************/
int MyDatabase::select_recordNum(QString tabName)
{
    QSqlQuery query(dbconn);
    if(! query.exec(QString("select * from %1").arg(tabName)))
    {
       qDebug() << QString("select * from %1").arg(tabName) << query.lastError();
    }
    qDebug() <<"记录数"<< query.size();
    return query.size();


}


/*****************************************************************删除表前n条记录****************************************************************/
void MyDatabase::delete_limitRecords(QString tabName,int num)
{
    QSqlQuery query(dbconn);
    if(! query.exec(QString("delete from %1 limit %2").arg(tabName).arg(num)))
    {
       qDebug() << QString("delete from %1 limit %2").arg(tabName).arg(num) << query.lastError();
    }
}
