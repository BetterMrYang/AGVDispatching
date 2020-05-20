#include "agvmonitor.h"
#include "ui_agvmonitor.h"
#include "mygraphicsview.h"
#include "Traffic/mydijkstra.h"
#include "include.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>
#include <QGraphicsEllipseItem>
#include <QDebug>
#include <QInputDialog>
#include <QSqlError>
#include <synchapi.h>


int AGVACCOUNT = 0; //目前设置最多12
QVector<int> ENABLE = {0,0,0,0,0,0,0,0,0,0,0,0};
int MATERIALACCOUNT = 7;

QSemaphore WRAGV(1);

AGVMonitor::AGVMonitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AGVMonitor)
{
    ui->setupUi(this);
    m_myScene = new QGraphicsScene(this);
//    m_myScene->setSceneRect(0,0,960,540);
    m_myScene->setSceneRect(0,0,3840,2160);

//    ui->graphicsView->setGeometry(QRect(460,80, 891, 411));
////    ui->graphicsView->move(460,60);
    ui->graphicsView->setScene(m_myScene);
////    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
////    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->graphicsView->centerOn(0,0);//滚动条出现在开头
    ui->graphicsView->setRenderHint(QPainter::Antialiasing,true); //反走样
//    ui->graphicsView->setParent(this);
    ui->graphicsView->installEventFilter(this);


//    AGVNUM =  QInputDialog::getInt(this,tr("提示："),tr("请输入AGV数目"),1,1,12,1);
//    MATERIALNUM =  QInputDialog::getInt(this,tr("提示："),tr("请输入物料数目"),1,1,999,1);
    initStyle();
    timer_display = new QTimer(this);
    timer_record = new QTimer(this);
    timer_agvPos = new QTimer(this);
    timer_twinkle = new QTimer(this);

    timer_flushCall = new QTimer(this);
//    timer_flushCall->start(500);

    Btn_test = new QPushButton(this);
    Btn_test->setText("测试");
    Btn_test->move(1200,0);

    createConnection();
    m_myDatabase = new MyDatabase();
    m_myDatabase->createDefaultConnection();
    createModel();



}

AGVMonitor::~AGVMonitor()
{
    delete ui;
    delete  m_myDatabase;
}

void AGVMonitor::slot_test()
{
    m_mySerialPort->writeData({"9","1111",QString("%1").arg(100*slider),"120","0"});
    m_mySerialPort->writeData({"9","2222",QString("%1").arg(100*slider),"320","0"});
//    m_mySerialPort->writeData({"9","2222",QString("%1").arg(100*slider),"320",QString("%1").arg(slider%2)});
    slider++;
}

void AGVMonitor::slot_Btn_test()
{
    /*****************测试Dijkstra****************/
//    QList<DijkstraInfo> info;
//    DijkstraInfo a = {0,5,100};
//    info.append(a);
//    DijkstraInfo b = {0,4,30};
//    info.append(b);
//    DijkstraInfo c = {0,2,10};
//    info.append(c);
//    DijkstraInfo d = {1,2,5};
//    info.append(d);
//    DijkstraInfo e = {2,3,50};
//    info.append(e);
//    DijkstraInfo f = {3,5,10};
//    info.append(f);
//    DijkstraInfo g = {4,3,20};
//    info.append(g);
//    DijkstraInfo h = {4,5,60};
//    info.append(h);

//    myDijkstra dijkstra(this,6,8);
//    dijkstra.getNewRoute(0,3,info);

    /*****************测试showPath****************/
//    showPath("0,1,2,3");

    /*****************测试getIntialPath*************************/
    qDebug() << "getInitialPath" << getInitialPath(0,5);


//    qDebug() << readFile("test.txt");
//    /***************************数据库测试****************************/
//    m_myDatabase->insert_MySQL("crossPointStatus",{"3","2","0","1"});
//    m_myDatabase->update_MySQL("crossPointStatus","CrossStatus = 3",3);
//    QList<QString> aa = m_myDatabase->select_MySQL("crossPointStatus",1);
//    for(int i = 0;i < 4; i++)
//    {
//        qDebug() << aa.at(i);
//    }
//    m_myDatabase->delete_MySQL("crossPointStatus",3);
//    Model_taskError->select(); //查询即可更新Model
//    Model_agvError->select();
//    Model_deviceError->select();
//    Model_taskExecuting->select();
//    Model_taskNew->select();
//    Model_taskDispatch->select();
//    Model_taskComplete->select();
//    Model_agvStatus->select();
//    Model_agvStatics->select();
//    Model_materialStatus->select();
//      count++;

//    for(int i = 0;i < m_list_agvInfo.size();i++)
//    {
//         qDebug() << m_list_agvInfo.at(i).agvNo << m_list_agvInfo.at(i).PosX << m_list_agvInfo.at(i).PosY;
//    }

//    QTimer *timer = new QTimer(this); //模拟AGV自动反馈位置
//    connect(timer,SIGNAL(timeout()),this,SLOT(slot_test()));
//    timer->start(1000);

//    twinkleAGV(1111);
//    twinkleAGV(2222);
//    stopTwinkle(1111);
//    stopTwinkle(2222);

//    for(int i = 0;i < 5;i++)
//    {

//        m_mySerialPort->writeData({"9","1111",QString("%1").arg(100*i),"120"});
////        m_mySerialPort->writeData({"4","2222","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20"});
//    }
//    m_mySerialPort->writeData({"9","1111","120","120"});
//      m_mySerialPort->writeData({"4","2222","333","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20"});
//    m_mySerialPort->integrateAGVError(4);
//    m_mySerialPort->integrateDeviceError(8);

//    m_myDatabase->select_recordNum("taskError");
//    item = new QGraphicsRectItem(QRect(30,30,30,20));
//     m_myScene->addItem(item);
//    item->setPos(count*50,30);

}

void AGVMonitor::initStyle()
{
    RoutePen.setWidth(3);
    RoutePen.setColor(Qt::green);
    ArrowPen.setWidth(2);
    ArrowPen.setColor(Qt::black);
    PathPen.setStyle(Qt::DashLine);
    PathPen.setWidth(2);
    PathPen.setColor(Qt::darkGray);
    PointBrush.setColor(Qt::blue);
    PointBrush.setStyle(Qt::SolidPattern);
    StationBrush.setColor(Qt::red);
    StationBrush.setStyle(Qt::SolidPattern);
    LiftAgvBrush.setColor(QColor(255,170,0)); //举升式
    LiftAgvBrush.setStyle(Qt::SolidPattern);
    PullAgvBrush.setColor(QColor(255,85,255));//牵引式
    PullAgvBrush.setStyle(Qt::SolidPattern);
    LatentAgvBrush.setColor(QColor(170,255,255));//潜伏式
    LatentAgvBrush.setStyle(Qt::SolidPattern);


}

void AGVMonitor::createModel()
{
    /*****************************任务表**********************/
    QStringList lsv_str_taskExecuting = {"生成编号","执行编号","任务类型","搬运AGV","任务状态","物料编号","执行时间","任务里程","路径号",
                                         "路径端点号","工位号","装卸动作号","装卸异常","充电动作号","充电异常","剩余时间"};
    QStringList lsv_str_taskNew = {"生成编号","执行编号","任务类型","搬运AGV","物料编号","任务状态","起点工位","终点工位","起点移栽方式","终点移栽方式","生成时间","时间预测"};
    QStringList lsv_str_taskDispatch = {"生成编号","执行编号","任务类型","搬运AGV","物料编号","任务状态","起点工位","终点工位","起点移栽方式","终点移栽方式","生成时间","时间预测"};
    QStringList lsv_str_taskComplete = {"生成编号","执行编号","任务类型","搬运AGV","任务状态","物料编号","执行时间","任务里程","路径号",
                                        "路径端点号","工位号","装卸动作号","装卸异常","充电动作号","充电异常","剩余时间"};

    //正在执行任务表
    Model_taskExecuting = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_taskExecuting->setTable("taskExecute");
    Model_taskExecuting->setSort(0,Qt::AscendingOrder);
    Model_taskExecuting->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_taskExecuting->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表1错误"));
        qDebug() << Model_taskExecuting->lastError().text();
        return;
    }

    //新产生任务表
    Model_taskNew = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_taskNew->setTable("taskNew");
    Model_taskNew->setSort(0,Qt::AscendingOrder);
    Model_taskNew->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_taskNew->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表2错误"));
        qDebug() << Model_taskNew->lastError().text();
        return;
    }
    //已调度任务表
    Model_taskDispatch = new MyQSqlTableModel(this,m_myDatabase->dbconn);//已完成任务与新建任务相同
    Model_taskDispatch->setTable("taskDispatched");
    Model_taskDispatch->setSort(0,Qt::AscendingOrder);
    Model_taskDispatch->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_taskDispatch->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表3错误"));
        qDebug() << Model_taskDispatch->lastError().text();
        return;
    }

    //已完成任务表
    Model_taskComplete = new MyQSqlTableModel(Model_taskExecuting,m_myDatabase->dbconn);//已完成任务与正在执行任务相同
    Model_taskComplete->setTable("taskComplete");
    Model_taskComplete->setSort(0,Qt::AscendingOrder);
    Model_taskComplete->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_taskComplete->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表4错误"));
        qDebug() << Model_taskComplete->lastError().text();
        return;
    }


    for(int i = 0;i < lsv_str_taskExecuting.size();i++)
    {
        Model_taskExecuting->setHeaderData(i,Qt::Horizontal,lsv_str_taskExecuting.at(i));
        Model_taskComplete->setHeaderData(i,Qt::Horizontal,lsv_str_taskComplete.at(i));
    }
    for(int i = 0;i < lsv_str_taskNew.size();i++)
    {
        Model_taskNew->setHeaderData(i,Qt::Horizontal,lsv_str_taskNew.at(i));
        Model_taskDispatch->setHeaderData(i,Qt::Horizontal,lsv_str_taskDispatch.at(i));
    }


    ui->Table_taskExecuting->setModel(Model_taskExecuting);
    ui->Table_taskExecuting->resizeRowsToContents();
    ui->Table_taskExecuting->resizeColumnsToContents();
    ui->Table_taskNew->setModel(Model_taskNew);
    ui->Table_taskNew->resizeRowsToContents();
    ui->Table_taskNew->resizeColumnsToContents();
    ui->Table_taskDispatch->setModel(Model_taskDispatch);
    ui->Table_taskDispatch->resizeRowsToContents();
    ui->Table_taskDispatch->resizeColumnsToContents();
    ui->Table_taskComplete->setModel(Model_taskComplete);
    ui->Table_taskComplete->resizeRowsToContents();
    ui->Table_taskComplete->resizeColumnsToContents();

//    for(int j = 0;j < 20;j++)
//    {
//        ui->Table_taskExecuting->setColumnWidth(j,75);
//        ui->Table_taskExecuting->setRowHeight(j,20);
//        ui->Table_taskNew->setColumnWidth(j,75);
//        ui->Table_taskNew->setRowHeight(j,20);
//        ui->Table_taskDispatch->setColumnWidth(j,75);
//        ui->Table_taskDispatch->setRowHeight(j,20);
//        ui->Table_taskComplete->setColumnWidth(j,75);
//        ui->Table_taskComplete->setRowHeight(j,20);
//    }
//    ui->Table_taskExecuting->setColumnWidth(9,100);
//    ui->Table_taskExecuting->setColumnWidth(11,100);
//    ui->Table_taskExecuting->setColumnWidth(13,100);
//    ui->Table_taskNew->setColumnWidth(8,100); //内容长，需要调整
//    ui->Table_taskNew->setColumnWidth(9,100);
//    ui->Table_taskDispatch->setColumnWidth(8,100);
//    ui->Table_taskDispatch->setColumnWidth(9,100);
//    ui->Table_taskComplete->setColumnWidth(9,100);
//    ui->Table_taskComplete->setColumnWidth(11,100);
//    ui->Table_taskComplete->setColumnWidth(13,100);

    /************************异常*************************/
    QStringList lsv_str_taskError = {"生成编号","任务类型","执行编号","搬运AGV","物料编号","状态"};
    QStringList lsv_str_agvError = {"AGV编号","AGV类型","电量","运行状态","故障状态","任务状态","装卸异常","充电异常"};
    QStringList lsv_str_deviceError = {"设备编号","设备类型","装卸动作号","装卸异常","充电动作号","充电异常"};

//    Model_taskError = new QStandardItemModel(this);
//    Model_taskError->setColumnCount(lsv_str_taskError.size());
//    Model_taskError->setRowCount(7);
    //任务异常表
    Model_taskError = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_taskError->setTable("taskError");
    Model_taskError->setSort(0,Qt::AscendingOrder);
    Model_taskError->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_taskError->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表5错误"));
        qDebug() << Model_taskError->lastError().text();
        return;
    }

    //AGV异常表
    Model_agvError = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_agvError->setTable("agvError");
    Model_agvError->setSort(0,Qt::AscendingOrder);
    Model_agvError->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_agvError->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表6错误"));
        qDebug() << Model_agvError->lastError().text();
        return;
    }

    //其他设备异常表
    Model_deviceError = new MyQSqlTableModel(this,m_myDatabase->dbconn);//已完成任务与新建任务相同
    Model_deviceError->setTable("otherDeviceError");
    Model_deviceError->setSort(0,Qt::AscendingOrder);
    Model_deviceError->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_deviceError->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表7错误"));
        qDebug() << Model_deviceError->lastError().text();
        return;
    }

    for(int i = 0;i < lsv_str_taskError.size();i++)
    {
        Model_taskError->setHeaderData(i,Qt::Horizontal,lsv_str_taskError.at(i));
    }
    for(int i = 0;i < lsv_str_agvError.size();i++)
    {
        Model_agvError->setHeaderData(i,Qt::Horizontal,lsv_str_agvError.at(i));
    }
    for(int i = 0;i < lsv_str_deviceError.size();i++)
    {
         Model_deviceError->setHeaderData(i,Qt::Horizontal,lsv_str_deviceError.at(i));
    }

    ui->Table_taskError->setModel(Model_taskError);
    ui->Table_taskError->resizeRowsToContents();
    ui->Table_taskError->resizeColumnsToContents();
    ui->Table_agvError->setModel(Model_agvError);
    ui->Table_agvError->resizeRowsToContents();
    ui->Table_agvError->resizeColumnsToContents();
    ui->Table_deviceError->setModel(Model_deviceError);
    ui->Table_deviceError->resizeRowsToContents();
    ui->Table_deviceError->resizeColumnsToContents();

//    for(int j = 0;j < 20;j++)
//    {
//        ui->Table_taskError->setColumnWidth(j,75);
//        ui->Table_taskError->setRowHeight(j,25);
//        ui->Table_agvError->setColumnWidth(j,85);
//        ui->Table_agvError->setRowHeight(j,25);
//        ui->Table_deviceError->setColumnWidth(j,85);
//        ui->Table_deviceError->setRowHeight(j,25);
//    }

    /************************AGV**************************/
    QStringList lsv_str_agvStatus = {"AGV编号","设备类型","电量","障碍物状态","运行状态","路径段编号","工位点编号","交叉点号","故障状态",
                                     "任务生成号","任务执行号","任务状态","物料编号","装卸动作号","装卸异常","充电动作号","充电异常","X坐标","Y坐标"};
    QStringList lsv_str_agvStatics = {"AGV编号","设备类型","累计任务数","累计里程","累计负载","利用率"};

    //AGV运行状态表
    Model_agvStatus = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_agvStatus->setTable("agvRunStatus");
    Model_agvStatus->removeColumn(0);//移除指令编号
    Model_agvStatus->setSort(0,Qt::AscendingOrder);
    Model_agvStatus->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_agvStatus->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表8错误"));
        qDebug() << Model_agvStatus->lastError().text();
        return;
    }

    //AGV运行统计表
    Model_agvStatics = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_agvStatics->setTable("agvRunStatics");
    Model_agvStatics->setSort(0,Qt::AscendingOrder);
    Model_agvStatics->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_agvStatics->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表9错误"));
        qDebug() << Model_agvStatics->lastError().text();
        return;
    }

    for(int i = 0;i < lsv_str_agvStatus.size();i++)
    {
        Model_agvStatus->setHeaderData(i,Qt::Horizontal,lsv_str_agvStatus.at(i));
    }
    for(int i = 0;i < lsv_str_agvStatics.size();i++)
    {
        Model_agvStatics->setHeaderData(i,Qt::Horizontal,lsv_str_agvStatics.at(i));
    }
    ui->Table_agvStatus->setModel(Model_agvStatus);
    ui->Table_agvStatus->resizeRowsToContents();
    ui->Table_agvStatus->resizeColumnsToContents();
    ui->Table_agvStatics->setModel(Model_agvStatics);
    ui->Table_agvStatics->resizeRowsToContents();
    ui->Table_agvStatics->resizeColumnsToContents();

//    for(int j = 0;j < 20;j++)
//    {
//      ui->Table_agvStatus->setColumnWidth(j,90);
//      ui->Table_agvStatus->setRowHeight(j,20);
//      ui->Table_agvStatics->setColumnWidth(j,91);
//      ui->Table_agvStatics->setRowHeight(j,20);
//    }

    /********************************物料**************************/
    QStringList lsv_str_materialStatus = {"物料编号","物料类型","任务生成号","任务执行号","搬运AGV号","路径段号","工位点号","交叉点号"};
    //物料配送表
    Model_materialStatus = new MyQSqlTableModel(this,m_myDatabase->dbconn);
    Model_materialStatus->setTable("materialDeliverStatus");
    Model_materialStatus->setSort(0,Qt::AscendingOrder);
    Model_materialStatus->setEditStrategy(QSqlTableModel::OnManualSubmit);//修改暂时缓存，手动调用summitAll()或revertAll();
    if(!(Model_materialStatus->select()))
    {
        QMessageBox::critical(this,tr("错误"),tr("打开数据表10错误"));
        qDebug() << Model_materialStatus->lastError().text();
        return;
    }

    for(int i = 0;i < lsv_str_materialStatus.size();i++)
    {
        Model_materialStatus->setHeaderData(i,Qt::Horizontal,lsv_str_materialStatus.at(i));
    }
    ui->Table_materialStatus->setModel(Model_materialStatus);
    ui->Table_materialStatus->resizeRowsToContents();
    ui->Table_materialStatus->resizeColumnsToContents();
//    for(int j = 0;j < 20;j++)
//    {
//      ui->Table_materialStatus->setColumnWidth(j,90);
//      ui->Table_materialStatus->setRowHeight(j,20);
//    }

}

void AGVMonitor::createConnection()
{
    connect(Btn_test,SIGNAL(clicked()),this,SLOT(slot_Btn_test()));
    connect(ui->Menubar_importMap,SIGNAL(triggered()),this,SLOT(slot_importMap()));
    connect(ui->Menubar_serialPort,SIGNAL(triggered()),this,SLOT(slot_serialPort()));
    connect(timer_display,SIGNAL(timeout()),this,SLOT(slot_timer_display()));
    connect(timer_record,SIGNAL(timeout()),this,SLOT(slot_timer_record()));
    connect(timer_agvPos,SIGNAL(timeout()),this,SLOT(slot_timer_agvPos()));
    connect(timer_flushCall,SIGNAL(timeout()),this,SLOT(slot_timer_flushCall()));
    connect(timer_twinkle,SIGNAL(timeout()),this,SLOT(slot_timer_twinkle()));
    connect(ui->Menubar_agvAccount,SIGNAL(triggered()),this,SLOT(slot_agvAccount()));
    connect(ui->Menubar_agvSettings,SIGNAL(triggered()),this,SLOT(slot_agvSettings()));
}

bool AGVMonitor::eventFilter(QObject *obj,QEvent *event)
{
//    Q_UNUSED(event);
    if(obj == ui->graphicsView)
    {
        if(event->type() == QEvent::Leave)
        {
            timer_display->stop();
        }
        else
        {
            timer_display->start(500);
        }

    }

    return false;
}

QList<QPointF> AGVMonitor::readFile(QString fileName)
{
//    QString linestr;
    QList<QPointF> strlist;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
         qDebug()<<file.errorString()<<fileName<<"  open failed!";
    }
    QTextStream stream(&file);

    while(!stream.atEnd())
    {
         QString linestr(stream.readLine()); //按行读取
         QList<QString> lineList(linestr.split(","));
         QPointF pos(lineList.at(0).toDouble(),lineList.at(1).toDouble());
         strlist.append(pos);//追加到字符串列表

    }
    stream.flush();
    file.close();
    return strlist;
}


void AGVMonitor::mapOffset(QString fileName)
{
    QList<QPointF> totalPoints = readFile(fileName);
    //保存极值点，X最大、X最小、Y最大、Y最小,假设形状和电子地图一样，故只需一个点进行映射（左上角的点与基准点映射）
    QVector<double> extremePoint = {totalPoints.at(0).x(),totalPoints.at(0).y(),totalPoints.at(0).x(),totalPoints.at(0).y()};
    for(int i = 1;i < totalPoints.size();i++)
    {
        if(totalPoints.at(i).x() > extremePoint.at(0)) //X最大
        {
            extremePoint.replace(0,totalPoints.at(i).x());
        }
        else if(totalPoints.at(i).x() < extremePoint.at(1)) //X最小
        {
            extremePoint.replace(1,totalPoints.at(i).x());
        }

        if(totalPoints.at(i).x() > extremePoint.at(2)) //Y最大
        {
            extremePoint.replace(2,totalPoints.at(i).y());
        }
        else if(totalPoints.at(i).x() < extremePoint.at(3))//Y最小
        {
            extremePoint.replace(3,totalPoints.at(i).y());
        }
    }
    double MapTopLeftX = m_list_routePoint.at(0).pos_x; //电子地图模块参考点X
    double MapTopLeftY = m_list_routePoint.at(0).pos_y;
    offsetX =extremePoint.at(1)-MapTopLeftX;
    offsetY =extremePoint.at(3)-MapTopLeftY;

}
/*********************************************************************槽函数********************************************************************************/
void AGVMonitor::slot_importMap()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("open file"),"/home",tr("XML Files(*.xml)"));
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this,tr("Error"),tr("打开文件不能为空！"));
        return;
    }

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,tr("Error"),tr("文件只读失败！"));
        return;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        QMessageBox::warning(this,tr("Error"),tr("文件只读失败！"));
        file.close();
        return;
    }

    QDomElement root = doc.documentElement();

    /************************************获取信息***************************************/
    int count_xml_point = root.firstChildElement("路径端点").childNodes().size();
    QDomNodeList xml_point = root.firstChildElement("路径端点").childNodes();
    //获取路径端点信息
    for(int i = 0;i < count_xml_point;i++)
    {
        RoutePoint liv_routePoint;
        liv_routePoint.node_num = getXmlValue(xml_point.at(i),"Num");
        liv_routePoint.pos_x = getXmlValueF(xml_point.at(i),"PosX");
        liv_routePoint.pos_y = getXmlValueF(xml_point.at(i),"PosY");
        liv_routePoint.RFID_num = getXmlRFID(xml_point.at(i),"RFID");
        liv_routePoint.node_type = getXmlValue(xml_point.at(i),"Type");
        liv_routePoint.line_enter = getXmlValueList(xml_point.at(i),"LineEnter");
        liv_routePoint.line_exit = getXmlValueList(xml_point.at(i),"LineExit");

        m_list_routePoint.append(liv_routePoint);

    }
    //获取路径信息
    int count_xml_route = root.firstChildElement("路径段").childNodes().size();
    QDomNodeList xml_route = root.firstChildElement("路径段").childNodes();
    for(int j = 0;j < count_xml_route;j++)
    {
        Route liv_route;
        liv_route.path_num = getXmlValue(xml_route.at(j),"Num");
        liv_route.angle = getXmlValueF(xml_route.at(j),"Angle");
        liv_route.capacity = getXmlValue(xml_route.at(j),"Capacity");
        liv_route.enable_dir = getXmlValue(xml_route.at(j),"Dir");
        liv_route.start_num = getXmlValue(xml_route.at(j),"StartNum");
        liv_route.end_num = getXmlValue(xml_route.at(j),"EndNum");
        liv_route.length = getXmlValueF(xml_route.at(j),"Length");
        liv_route.contain_station = getXmlValueList(xml_route.at(j),"ContainStation");
        liv_route.start_pos = QPointF(m_list_routePoint[liv_route.start_num].pos_x,m_list_routePoint[liv_route.start_num].pos_y);
        liv_route.end_pos = QPointF(m_list_routePoint[liv_route.end_num].pos_x,m_list_routePoint[liv_route.end_num].pos_y);

        m_list_route.append(liv_route);
    }
    //获取工位点信息
    //画出所有工位点
    int count_xml_station = root.firstChildElement("工位点").childNodes().size();
    QDomNodeList xml_station = root.firstChildElement("工位点").childNodes();
    StationPoint liv_stationpoint;//为了加入位置信息
    for(int k = 0;k < count_xml_station;k++)
    {

        liv_stationpoint.node_num = getXmlValue(xml_station.at(k),"Num");
        liv_stationpoint.length = getXmlValueF(xml_station.at(k),"Length");
        liv_stationpoint.RFID_num = getXmlRFID(xml_station.at(k),"RFID");
        liv_stationpoint.attach_num = getXmlValue(xml_station.at(k),"Attach");
        liv_stationpoint.node_type = getXmlValue(xml_station.at(k),"Type");

        m_list_stationPoint.append(liv_stationpoint);
    }

    /************************************画地图***************************************/
    //端点
    for(int a = 0;a < count_xml_point;a++)
    {
        QRectF routepoint;
        routepoint.setTopLeft(QPointF(m_list_routePoint.at(a).pos_x,m_list_routePoint.at(a).pos_y)-QPointF(5,5));
        routepoint.setBottomRight(QPointF(m_list_routePoint.at(a).pos_x,m_list_routePoint.at(a).pos_y)+QPointF(5,5));
        QGraphicsEllipseItem *PointItem = new QGraphicsEllipseItem(routepoint);
        PointItem->setBrush(PointBrush);
        PointItem->setFlag(QGraphicsItem::ItemIsSelectable);
        PointItem->setToolTip(tr("point,%1").arg(a));

        //备注
        drawRemark(routepoint.topLeft(),PointItem,"point",a);
        m_myScene->addItem(PointItem);
    }
    //路径
    for(int b = 0;b < count_xml_route;b++)
    {
        //画主体
        QLineF LineItem;
        LineItem.setP1(m_list_route.at(b).start_pos);
        LineItem.setP2(m_list_route.at(b).end_pos);
        QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
        item->setPen(RoutePen);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setToolTip(tr("line,%1").arg(b));
        m_myScene->addItem(item);

        //箭头
        QPointF jointPoint;
        jointPoint = m_list_route[b].start_pos+(m_list_route[b].end_pos-m_list_route[b].start_pos)/2;
        QLineF arrowLeft;
        QLineF arrowRight;
        arrowLeft.setP1(jointPoint);
        arrowRight.setP1(jointPoint);
        arrowLeft.setLength(6);
        arrowRight.setLength(6);
        arrowLeft.setAngle(m_list_route[b].angle+180+30);
        arrowRight.setAngle(m_list_route[b].angle+180-30);
        QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
        QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
        arrowLeftItem->setLine(arrowLeft);
        arrowRightItem->setLine(arrowRight);
        arrowLeftItem->setPen(ArrowPen);
        arrowRightItem->setPen(ArrowPen);
        arrowLeftItem->setParentItem(item);
        arrowRightItem->setParentItem(item);
    }
    //工位点
    for(int c = 0;c < count_xml_station;c++)
    {
        QRectF station_Rect;
        int liv_attachNum = m_list_stationPoint.at(c).attach_num;
        QPointF station_pos = QPointF(m_list_route[liv_attachNum].start_pos.x()+m_list_stationPoint.at(c).length*cos(m_list_route[liv_attachNum].angle/180.0*M_PI),
                                      m_list_route[liv_attachNum].start_pos.y()-m_list_stationPoint.at(c).length*sin(m_list_route[liv_attachNum].angle/180.0*M_PI));
        station_Rect.setTopLeft(station_pos+QPointF(-5,-5));
        station_Rect.setBottomRight(station_pos+QPointF(5,5));
        QGraphicsRectItem *stationitem = new QGraphicsRectItem(station_Rect);
        stationitem->setBrush(StationBrush);
        stationitem->setFlag(QGraphicsItem::ItemIsSelectable);
        stationitem->setToolTip(tr("station,%1").arg(c));

        //备注
        drawRemark(station_Rect.topLeft(),stationitem,"station",c);

        m_myScene->addItem(stationitem);

        //追加至链表
        liv_stationpoint.PosX = station_pos.x();
        liv_stationpoint.PosY = station_pos.y();

    }
    qDebug() << m_myScene->items(Qt::DescendingOrder).size();

    //获取距离表和路由表
    int count_distance = root.firstChildElement("距离表").childNodes().size();
    QDomNodeList xml_distance = root.firstChildElement("距离表").childNodes();
    for(int z = 0;z < count_distance;z++)
    {
        QString value = xml_distance.at(z).toElement().attributeNode("Content").value();
        m_list_distance.append(value);
    }
    qDebug() << "距离表" << m_list_distance;
    int count_floyd = root.firstChildElement("路由表").childNodes().size();
    QDomNodeList xml_floyd = root.firstChildElement("路由表").childNodes();
    for(int z = 0;z < count_floyd;z++)
    {
        QString value = xml_floyd.at(z).toElement().attributeNode("Content").value();
        m_list_floyd.append(value);
    }
    qDebug() << "路由表" << m_list_floyd;


}

void AGVMonitor::drawRemark(QPointF point,QGraphicsItem* parent,QString type,int RFID)
{
        QGraphicsSimpleTextItem *Item = new QGraphicsSimpleTextItem;
        QPen pen(Qt::darkCyan);
        Item->setPen(pen);
        Item->setParentItem(parent);


        if(type == "point")
        {
            Item->setText(QString("%1").arg(RFID));
//            Item->setToolTip(tr("remark,%1").arg(No));
            Item->setPos(point+QPointF(-20,-18));
        }
        else if(type == "station")
        {
            Item->setText(QString("%1").arg(RFID));
//            Item->setToolTip(tr("station_remark,%1").arg(No));
            Item->setPos(point+QPointF(9,12));
        }
        else
        {
            qDebug() << "drawRemark失败";
            return;
        }

}





void AGVMonitor::slot_serialPort()
{
    if(serialPortStatus == 0)
    {
//       qDebug() << "";
       m_mySerialPort = new MySerialPort(this);
//       if(count_serialPort == 0)
//       {
////           m_mySerialPort->myDatabase = m_myDatabase;
//           count_serialPort = 1;
//       }

       serialPortStatus = 1;
    }
    else
    {
        m_mySerialPort->show();
    }

}

void AGVMonitor::slot_timer_display()
{
     QList<QGraphicsItem*>  selectItem = m_myScene->selectedItems();
     if(selectItem.size() == 0) //确保第二次点击同一个图元仍可显示
     {
         last_Type.clear();
         last_TypeNo = -1;
     }
     else if(selectItem.size() == 1)
     {
         QString lsv_selectType = selectItem.at(0)->toolTip().split(",").first();
         int liv_selectNo = selectItem.at(0)->toolTip().split(",").last().toInt();
         qDebug() << "选中1" << "last_type" << last_Type << "现在" << lsv_selectType << "lastno" << last_TypeNo << "现在" << liv_selectNo;
         if(!(last_Type.isEmpty() && last_TypeNo == -1)) //只要不是初始状态，就需要判断
         {
             if(last_Type == lsv_selectType && last_TypeNo == liv_selectNo)
             {
                 return;
             }
         }
         last_Type = lsv_selectType;
         last_TypeNo = liv_selectNo;
         Dialog_ViewStatus dialog_view; //局部变量，自动析构
         if(lsv_selectType == "line")
         {
             QList<QString> routeStatusInfo = m_myDatabase->select_MySQL("routeStatus",liv_selectNo);
             for(int i = 0;i < routeStatusInfo.size();i++)
             {
                 qDebug() << routeStatusInfo.at(i);
             }
             dialog_view.setSelectNo(lsv_selectType,liv_selectNo);
             dialog_view.fillInfo(routeStatusInfo.at(2),routeStatusInfo.at(3));
             dialog_view.exec();
         }
         else if(lsv_selectType == "point")
         {
             QList<QString> crossPointInfo = m_myDatabase->select_MySQL("crossPointStatus",liv_selectNo);

             for(int i = 0;i < crossPointInfo.size();i++)
             {
                 qDebug() << crossPointInfo.at(i);
             }
             dialog_view.setSelectNo(lsv_selectType,liv_selectNo);
             dialog_view.fillInfo(crossPointInfo.at(1),crossPointInfo.at(2));
             dialog_view.exec();
         }
         else
         {
             QList<QString> stationPointInfo = m_myDatabase->select_MySQL("stationPointStatus",liv_selectNo);

             for(int i = 0;i < stationPointInfo.size();i++)
             {
                 qDebug() << stationPointInfo.at(i);
             }
             dialog_view.setSelectNo(lsv_selectType,liv_selectNo);
             dialog_view.fillInfo(stationPointInfo.at(1),stationPointInfo.at(2));
             dialog_view.exec();
         }
     }
}

void AGVMonitor::slot_timer_record()
{


}

void AGVMonitor::slot_timer_agvPos()
{
    qDebug() << "刷新AGV位置" << "AGVACCOUNT" << AGVACCOUNT;
    if(AGVACCOUNT == 0)
    {
        if(timer_agvPos->isActive())
        {
           timer_agvPos->stop();
        }
        return;
    }
    //获取场景上所有AGV
//    QList<QGraphicsItem*> total_items = m_myScene->items(Qt::DescendingOrder);
//    QList<QGraphicsItem*> total_agvs;
//    for(int i = 0;i < total_items.size();i++)
//    {
//        if(total_items.at(i)->toolTip().split(",").first() == "agv")
//        {
//            total_agvs.append(total_items.at(i));
//        }
//    }
    for(int j = 0;j < AGVACCOUNT;j++)
    {
        int agvNo = m_list_agvInfo.at(j).agvNo;
        QGraphicsItem* item = searchAGV(agvNo);

        QList<QString> carInfo = m_myDatabase->select_MySQL("agvPosition",agvNo);
        m_list_agvInfo[j].agvNo = carInfo.at(1).toInt();
        m_list_agvInfo[j].PosX = carInfo.at(2).toDouble();
        m_list_agvInfo[j].PosY = carInfo.at(3).toDouble();
        //判断是否在转弯
        if(carInfo.last().toInt()) //正在转弯
        {
            qDebug() << "正在转弯";
            if(!m_list_agvInfo[j].twinkleStatus) //刚进入弯道，即上个状态twinkleStatus = 0；
            {
                twinkleAGV(carInfo.at(1).toInt());
                m_list_agvInfo[j].twinkleStatus = 1;

            }

        }
        else  //直线行驶
        {
            qDebug() << "直线行驶";
            if(m_list_agvInfo[j].twinkleStatus) //刚驶出弯道，即上个状态twinkleStatus = 1；
            {
                m_list_agvInfo[j].twinkleStatus = 0;
                stopTwinkle(m_list_agvInfo[j].agvNo);
            }
            item->setPos(QPointF(m_list_agvInfo[j].PosX-offsetX,m_list_agvInfo[j].PosY-offsetY)); //更新位置需映射到地图上
        }

        m_myScene->update(); //不刷新就不动
    }

//    QList<QGraphicsItem*> total_items = m_myScene->items(Qt::DescendingOrder);
//    count++;
//    for(int i = 0;i < total_items.size();i++)
//    {
//        if(total_items.at(i)->toolTip().split(",").first() == "agv")
//        {
//            total_items.at(i)->setPos(50*count,30);

////            qDebug() << total_items.at(i)->toolTip().split(",").last().toInt() << "agv位置" << total_items.at(i)->pos();
//            break;
//        }
//    }
}

void AGVMonitor::slot_agvAccount()
{
    qDebug() << "agv数量管理";
    Dialog_agvManagement *dialog_agvManagement = new Dialog_agvManagement;
    dialog_agvManagement->ui->Stack_agvManagement->setCurrentIndex(0);
    //接收删除agv信号
    connect(dialog_agvManagement,SIGNAL(deleteAGVNo(int)),this,SLOT(slot_deleteAGVNo(int)));
    dialog_agvManagement->exec();
    if(dialog_agvManagement->getFlag_addAGV())
    {
//        //AGV图标
        int agvNo = dialog_agvManagement->ui->LineEdit_agvNo->text().toInt();
        QRect AGVShape;
        AGVShape.setTopLeft(QPoint(-15,-10));
        AGVShape.setTopRight(QPoint(15,10));
        QGraphicsRectItem *agvItem = new QGraphicsRectItem(AGVShape);
        agvItem->setToolTip(QString("agv,%1").arg(agvNo));
        if(dialog_agvManagement->getAGVType() == lift)
        {
            agvItem->setBrush(LiftAgvBrush);
        }
        else if(dialog_agvManagement->getAGVType() == pull)
        {
            agvItem->setBrush(PullAgvBrush);
        }
        else
        {
            agvItem->setBrush(LatentAgvBrush);
        }
        //创建文字
        QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(QString("%1").arg(agvNo));
        QFont font("黑体", 6, 75);
        textItem->setFont(font);
//        textItem->setToolTip(QString("text,%1").arg(agvNo));
        textItem->setParentItem(agvItem);
        textItem->setPos(AGVShape.topLeft()-QPoint(-5,10));


        //将信息存入链表
        AGVInfo agvCar;
        agvCar.agvNo = agvNo;
        if(dialog_agvManagement->getAppearType() == position) //指定位置型
        {
            agvCar.PosX = dialog_agvManagement->ui->LineEdit_appearX->text().toDouble();
            agvCar.PosY = dialog_agvManagement->ui->LineEdit_appearY->text().toDouble();
            qDebug()<< "1" << agvCar.PosX << agvCar.PosY;
        }
        else //指定RFID
        {
            for(int i = 0;i < m_list_routePoint.size();i++)
            {
                if(m_list_routePoint.at(i).RFID_num == dialog_agvManagement->ui->LineEdit_appearRFID->text().toInt()) //路径端点处
                {
                    agvCar.PosX = m_list_routePoint.at(i).pos_x;
                    agvCar.PosX = m_list_routePoint.at(i).pos_y;
                    break;
                }
                if(m_list_stationPoint.at(i).RFID_num == dialog_agvManagement->ui->LineEdit_appearRFID->text().toInt()) //路径端点处
                {
                    agvCar.PosX = m_list_stationPoint.at(i).PosX;
                    agvCar.PosX = m_list_stationPoint.at(i).PosY;
                    break;
                }
            }
        }

        m_list_agvInfo.append(agvCar);
        agvItem->setPos(agvCar.PosX,agvCar.PosY);
        m_myScene->addItem(agvItem);
        ENABLE[AGVACCOUNT] = 1;
        AGVACCOUNT++;

        //内容插入数据库
        qDebug() << "插入数据库";
        m_myDatabase->insert_MySQL("agvPosition",{"9",QString("%1").arg(agvCar.agvNo),QString("%1").arg(agvCar.PosX),QString("%1").arg(agvCar.PosY),"0"});

        ui->CBX_AGVNo->addItem(QString("%1").arg(agvCar.agvNo));

        //开启刷新AGV位置的定时器
        if(AGVACCOUNT == 1)
        {
           timer_agvPos->start(125);
           timer_flushCall->start(500);
        }
    }

    
}

void AGVMonitor::slot_deleteAGVNo(int AGVNo)
{
    qDebug() << "slot_deleteAGVNo";
    for(int i = 0;i < m_list_agvInfo.size();i++)
    {
        if(m_list_agvInfo.at(i).agvNo == AGVNo)
        {
            m_myDatabase->delete_MySQL("agvPosition",AGVNo); //删除数据库中有关的,多张表,此处不全
            m_list_agvInfo.removeAt(i); //链表中删除相关的

            WRAGV.acquire(1);
            AGVACCOUNT--;
            ENABLE[AGVACCOUNT] = 0; //对应位置0
            WRAGV.release(1);
            m_myScene->removeItem(searchAGV(AGVNo)); //地图中删除
            break;
        }
    }
    if(twinkle_no.contains(AGVNo)) //闪烁AGV中删除相关的
    {
        twinkle_no.removeOne(AGVNo);
    }
}

void AGVMonitor::slot_agvSettings()
{

}

void AGVMonitor::slot_timer_flushCall()
{
    qDebug() << "slot_timer_flushCall";
    //AGV使能
//    for(int i = AGVACCOUNT;i < 12;i++)
//    {
//        ENABLE[i] = 0;
//    }
    WRAGV.acquire(1); //是否会导致对ENABLE和AGVACCOUNT操作的阻塞
    ui->Label_call1->setVisible(ENABLE[0]);
    if(ENABLE[0])
    {
        ui->Label_call1->setText(QString("%1").arg(m_list_agvInfo.at(0).agvNo));
    }
    ui->Label_call2->setVisible(ENABLE[1]);
    if(ENABLE[1])
    {
        ui->Label_call2->setText(QString("%1").arg(m_list_agvInfo.at(1).agvNo));
    }
    ui->Label_call3->setVisible(ENABLE[2]);
    if(ENABLE[2])
    {
        ui->Label_call3->setText(QString("%1").arg(m_list_agvInfo.at(2).agvNo));
    }
    ui->Label_call4->setVisible(ENABLE[3]);
    if(ENABLE[3])
    {
        ui->Label_call4->setText(QString("%1").arg(m_list_agvInfo.at(3).agvNo));
    }
    ui->Label_call5->setVisible(ENABLE[4]);
    if(ENABLE[4])
    {
        ui->Label_call5->setText(QString("%1").arg(m_list_agvInfo.at(4).agvNo));
    }
    ui->Label_call6->setVisible(ENABLE[5]);
    if(ENABLE[5])
    {
        ui->Label_call6->setText(QString("%1").arg(m_list_agvInfo.at(5).agvNo));
    }
    ui->Label_call7->setVisible(ENABLE[6]);
    if(ENABLE[6])
    {
        ui->Label_call7->setText(QString("%1").arg(m_list_agvInfo.at(6).agvNo));
    }
    ui->Label_call8->setVisible(ENABLE[7]);
    if(ENABLE[7])
    {
        ui->Label_call8->setText(QString("%1").arg(m_list_agvInfo.at(7).agvNo));
    }
    ui->Label_call9->setVisible(ENABLE[8]);
    if(ENABLE[8])
    {
        ui->Label_call9->setText(QString("%1").arg(m_list_agvInfo.at(8).agvNo));
    }
    ui->Label_call10->setVisible(ENABLE[9]);
    if(ENABLE[9])
    {
        ui->Label_call10->setText(QString("%1").arg(m_list_agvInfo.at(9).agvNo));
    }
    ui->Label_call11->setVisible(ENABLE[10]);
    if(ENABLE[10])
    {
        ui->Label_call11->setText(QString("%1").arg(m_list_agvInfo.at(10).agvNo));
    }
    ui->Label_call12->setVisible(ENABLE[11]);
    if(ENABLE[11])
    {
        ui->Label_call12->setText(QString("%1").arg(m_list_agvInfo.at(11).agvNo));
    }


    if(AGVACCOUNT == 0)
    {
        if(timer_flushCall->isActive())
        {
           timer_flushCall->stop();
        }
    }
    WRAGV.release(1);
}
/***************************************************************************槽函数*******************************************************************************/
/***************************************************************************************************************************************************************/


/***************************************************************************XML操作******************************************************************************/
int AGVMonitor::getXmlValue(const QDomNode &node,QString type)
{
     int value = node.toElement().attributeNode(type).value().toInt();
     return value;
}

double AGVMonitor::getXmlValueF(const QDomNode &node,QString type)
{
    double value = node.toElement().attributeNode(type).value().toDouble();
    return value;
}

QString AGVMonitor::getXmlRFID(const QDomNode &node,QString type)
{
    QString value = node.toElement().attributeNode(type).value();
    return value;
}

QVector<int> AGVMonitor::getXmlValueList(const QDomNode &node,QString type)
{
    int count_comma = node.toElement().attributeNode(type).value().count(',');
    QVector<int> value;
    QString content = node.toElement().attributeNode(type).value();
    if(count_comma == 0)
    {
        value.append(content.toInt());
    }
    else
    {
        for(int i = 0;i<count_comma+1;i++)
        {
           value.append(content.split(",").at(i).toInt());
        }
    }
    return value;
}
/***************************************************************************XML操作******************************************************************************/
/***************************************************************************************************************************************************************/


/****************************************************************************路网监控****************************************************************************/
void AGVMonitor::updateRoad(QString type,int number)
{

}

/*!
 * @brief 整合floyd生成路径
 * @fun getInitialPath
 * @param start、end——起点终点
 * @return 经过的路径点，形如“1,2,3,4”
*/
QString AGVMonitor::getInitialPath(int start,int end)
{
    QString lsv_path(QString::number(start)+",");
    int row = start,col = end;
    if(m_list_floyd.at(row).split(",").at(col).toInt() == end) //直达
    {
        lsv_path += QString::number(end);
    }
    else
    {
        while(m_list_floyd.at(row).split(",").at(col).toInt() != end)
        {
            int col_value = m_list_floyd.at(row).split(",").at(col).toInt();
            lsv_path += QString::number(col_value) + ",";
            row = col_value;
            qDebug() << "row" << row;
        }
        lsv_path += QString::number(end);
    }
    return lsv_path;
}

/*!
 * @brief 路径预览
 * @fun showPath
 * @param path 经过的路径点，形如“1,2,3,4”
 * @return QGraphicsItemGroup*——路径组
 * @remark 手动删除组——destroyItemGroup()
*/
QGraphicsItemGroup* AGVMonitor::showPath(const QString& path)
{
    QGraphicsItemGroup* itemGroup = new QGraphicsItemGroup;
    QList<QString> PathPoint = path.split(",");
    for(auto point : PathPoint)
    {
        int liv_no = point.toInt();
        QLineF LineItem;
        LineItem.setP1(m_list_route.at(liv_no).start_pos);
        LineItem.setP2(m_list_route.at(liv_no).end_pos);
        QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
        item->setPen(PathPen);
        m_myScene->addItem(item);
        itemGroup->addToGroup(item);
    }
    m_myScene->addItem(itemGroup);
    return itemGroup;
}


/****************************************************************************路网监控****************************************************************************/
QGraphicsItem* AGVMonitor::searchAGV(int number)
{
    QList<QGraphicsItem*> totalItems =  m_myScene->items(Qt::DescendingOrder);
    for(auto item : totalItems)
    {
        QString lsv_Type = item->toolTip().split(",").first();
        int liv_No = item->toolTip().split(",").last().toInt();
        if(lsv_Type == "agv" && liv_No == number)
        {
            return item;
        }
    }
    qDebug() << "找不到该agv";
    return NULL;
}

void AGVMonitor::twinkleAGV(int number)
{
    twinkle_no.append(number); //加入闪烁的agv编号
    timer_twinkle->start(300);
}

void AGVMonitor::slot_timer_twinkle()
{
    for(int NO : twinkle_no)
    {
        QGraphicsItem* item = searchAGV(NO);
        bool flag = !item->isVisible();
        item->setVisible(flag);
        m_myScene->update();
    }

}

void AGVMonitor::stopTwinkle(int number)
{
    twinkle_no.removeOne(number); //链表中去除不需闪烁的agv
    searchAGV(number)->setVisible(true); //保证能显示
    if(!twinkle_no.size()) //均不需闪烁
    {
        timer_twinkle->stop();
    }
}

//转向结合路径？
bool AGVMonitor::isTurning(int number)
{

}

/*******************************************控制面板*******************************************/
void AGVMonitor::on_TBtn_up_pressed()
{
    qDebug() << "上！！！！！！";
    directionPressed(1);

}

void AGVMonitor::on_TBtn_up_released()
{
    directionReleased();
}

void AGVMonitor::on_TBtn_down_pressed()
{
    qDebug() << "下！！！！！！";
    directionPressed(2);
}

void AGVMonitor::on_TBtn_down_released()
{
    directionReleased();
}

void AGVMonitor::on_TBtn_left_pressed()
{
    qDebug() << "左！！！！！！";
    directionPressed(3);
}

void AGVMonitor::on_TBtn_left_released()
{
    directionReleased();
}

void AGVMonitor::on_TBtn_right_pressed()
{
    qDebug() << "右！！！！！！";
    directionPressed(4);
}

void AGVMonitor::on_TBtn_right_released()
{
    directionReleased();
}

void AGVMonitor::directionPressed(int dir)
{
    if(ui->CBX_AGVNo->currentIndex() != 0 && ui->RBtn_manual->isChecked())
    {
        direction = dir;
        timer_direction = new QTimer;
        connect(timer_direction,SIGNAL(timeout()),this,SLOT(slot_direction()));
        timer_direction->start(200);
    }

}

void AGVMonitor::directionReleased()
{
    if(ui->CBX_AGVNo->currentIndex() != 0 && ui->RBtn_manual->isChecked())
    {
        if(timer_direction->remainingTime()) //确保没到200ms也能触发指令
        {
            slot_direction();
        }
        direction = 0;
        timer_direction->stop();
        delete timer_direction;
    }
}

void AGVMonitor::slot_direction()
{
        qDebug() << "串口控制小车协议！！！！！！";
//        m_mySerialPort->writeData();  根据spinbox调整发一帧数据移动的距离
}
/*******************************************控制面板*******************************************/


void AGVMonitor::on_RBtn_manual_pressed()
{
    if(ui->RBtn_auto->isChecked())
    {
        ui->RBtn_auto->setChecked(false);
    }
    if(ui->RBtn_manual->isChecked())
    {
        return;
    }
}

void AGVMonitor::on_RBtn_auto_pressed()
{
    if(ui->RBtn_manual->isChecked())
    {
        ui->RBtn_manual->setChecked(false);
    }
    if(ui->RBtn_auto->isChecked())
    {
        return;
    }
}
