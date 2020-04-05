#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_input.h"
#include "ui_dialog_input.h"
#include "dialog_attribute.h"
#include "ui_dialog_attribute.h"
#include "dialog_search.h"
#include "ui_dialog_search.h"
#include "dialog_firstpos.h"
#include "ui_dialog_firstpos.h"
#include "math.h"
#include <QGraphicsScene>
#include  <QGraphicsView>
#include <QMenuBar>
#include <QGraphicsItem>
#include <QUndoStack>
#include <QLine>
#include <QLineF>
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTimer>
#include <QTransform>
#include <QDomDocument>
#include <QDomProcessingInstruction>
#include <QTextCodec>
#include <memory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QIcon icon("D:/Study/QTPractise/build-GraphicsPractise-Desktop_Qt_5_11_3_MinGW_32bit-Debug/school.jpg");
    this->setWindowIcon(icon);
    ui->setupUi(this);
    this->setMouseTracking(true);
    this->centralWidget()->setMouseTracking(true);

    undoStack = new QUndoStack(this);
    createActions();
    initStyle();
    initParam();
    scene = new QGraphicsScene;
    scene->setSceneRect(0,0,3840,2160);
    ReferLineItem.setP2(QPointF(200,200));
    ReferLineItem.setP1(QPointF(400,200));


    view = new MyGraphicsView;
    view->setParent(this);
    view->setScene(scene);
    view->viewport()->installEventFilter(this);
    view->setMinimumSize(1093,614);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->centerOn(0,0);//滚动条出现在开头
    view->setRenderHint(QPainter::Antialiasing,true); //反走样

    mainLayout = new QGridLayout;
    mainLayout->addWidget(view);

    setCentralWidget(view);
    resize(550,450);


    initToolbar();

    timer_Initstatus = new QTimer;
    timer_Initstatus->start(100);
    connect(timer_Initstatus,SIGNAL(timeout()),this,SLOT(initStatus()));

//    Btn_test = new QPushButton(tr("测试"));
//    Btn_test->setGeometry(100,100,50,30);
//    Btn_test->setParent(this);
//    connect(Btn_test,SIGNAL(clicked()),this,SLOT(slotBtnTest()));

    LabelScaling = new QLabel(this);
    LabelScaling->move(60,70);
    LabelScaling->setText(tr("比例因子：%1").arg(scalingFactor));

}

void MainWindow::slotBtnTest()
{
    //测试代码
    updateList();

    for(int i = 0;i< list_route.size();i++)
    {
        qDebug() << "路径段号：" << list_route.at(i).path_num << "路径段角度：" <<list_route.at(i).angle << "路径段长度：" << list_route.at(i).length
                 << "路径容量：" << list_route.at(i).capacity << "起点编号：" << list_route.at(i).start_num  << "终点编号：" << list_route.at(i).end_num
                 << "所包含工位点号" << list_route.at(i).contain_station << "单双向使能：" << list_route.at(i).enable_dir << "起点位置：" << list_route.at(i).start_pos<< "终点位置：" << list_route.at(i).end_pos;
    }

    for(int i = 0;i<list_routepoint.size();i++)
    {
        qDebug() << "路径端点号：" << list_routepoint.at(i).node_num << "端点X坐标：" << list_routepoint.at(i).pos_x << "端点Y坐标：" <<list_routepoint.at(i).pos_y
                 << "RFID:" << list_routepoint.at(i).RFID_num << "节点类型：" << list_routepoint.at(i).node_type << "连接入口：" << list_routepoint.at(i).line_enter
                 << "连接出口：" << list_routepoint.at(i).line_exit;
    }

    for(int i = 0;i<list_stationpoint.size();i++)
    {
         qDebug() << "工位号：" << list_stationpoint.at(i).node_num << "所属直线编号：" << list_stationpoint.at(i).attach_num << "距离起点长度" <<list_stationpoint.at(i).length
                  << "RFID:" << list_stationpoint.at(i).RFID_num << "节点类型:" << list_stationpoint.at(i).node_type;
    }


    QList<FloydInfo > floydcontent = createFloydInfo();
    for(int z = 0;z < floydcontent.size();z++)
    {
        qDebug() << floydcontent.at(z).startNo << floydcontent.at(z).endNo << floydcontent.at(z).weight;
    }

    Floyd floyd(list_routepoint.size(),floydcontent);
    qDebug() << "基准点" << reference_PosX << reference_PosY << "flag" << flag_isDefaultPos;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{

    //右键菜单动作
    attribute = new QAction(tr("属性"));
    bindRFID = new QAction(tr("绑定RFID"));
    capacity = new QAction(tr("设置路径容量"));
    DeleteItem = new QAction(tr("删除图元"));
    nodeType = new QAction(tr("设置工位点类型"));
    dirEnable = new QAction(tr("路径使能"));

    //菜单栏信号槽

    //菜单————文件
     connect(ui->Menubar_new,SIGNAL(triggered()),this,SLOT(slotNew()));
     connect(ui->Menubar_save,SIGNAL(triggered()),this,SLOT(slotSave()));
     connect(ui->Menubar_extraSave,SIGNAL(triggered()),this,SLOT(slotExtraSave()));
     connect(ui->Menubar_open,SIGNAL(triggered()),this,SLOT(slotOpen()));
     connect(ui->Menubar_close,SIGNAL(triggered()),this,SLOT(slotClose()));

    //菜单————编辑
    connect(ui->Menubar_del,SIGNAL(triggered()),this,SLOT(slotDelete()));
    connect(ui->Menubar_clear,SIGNAL(triggered()),this,SLOT(slotClear()));
    connect(ui->Menubar_RFID,SIGNAL(triggered()),this,SLOT(slotBindRFID()));
    connect(ui->Menubar_capacity,SIGNAL(triggered()),this,SLOT(slotCapacity()));
    connect(ui->Menubar_dir,SIGNAL(triggered()),this,SLOT(slotDirEnable()));
    connect(ui->Menubar_type,SIGNAL(triggered()),this,SLOT(slotNodeType()));
    connect(ui->Menubar_attribute,SIGNAL(triggered()),this,SLOT(slotAttribute()));
    connect(ui->Menubar_search,SIGNAL(triggered()),this,SLOT(slotSearch()));
    connect(ui->Menubar_selectall,SIGNAL(triggered()),this,SLOT(slotSelectAll()));


    //菜单————画图
    connect(ui->Menubar_first,SIGNAL(triggered()),this,SLOT(slotAction_first()));
    connect(ui->Menubar_input,SIGNAL(triggered()),this,SLOT(slotAddLineItem()));
    connect(ui->Menubar_mouse,SIGNAL(triggered()),this,SLOT(slotAddLineItem_Press()));
//    connect(ui->Menubar_link,SIGNAL(triggered()),this,SLOT(slotAction_linkline()));
    connect(ui->Menubar_twopoints,SIGNAL(triggered()),this,SLOT(slotTwoPoints()));
    connect(ui->Menubar_station,SIGNAL(triggered()),this,SLOT(slotAddStationItem()));
    connect(ui->Menubar_merge,SIGNAL(triggered()),this,SLOT(slotAction_merge()));
    connect(ui->Menubar_loop,SIGNAL(triggered()),this,SLOT(slotLoop()));


    //菜单————视图
//     connect(ui->route_visible,SIGNAL(triggered()),this,SLOT(slotRouteVisible()));
//     connect(ui->route_invisible,SIGNAL(triggered()),this,SLOT(slotRouteInvisible()));
     connect(ui->point_visible,SIGNAL(triggered()),this,SLOT(slotPointVisible()));
     connect(ui->point_invisible,SIGNAL(triggered()),this,SLOT(slotPointInvisible()));
     connect(ui->station_visible,SIGNAL(triggered()),this,SLOT(slotStationVisible()));
     connect(ui->station_invisible,SIGNAL(triggered()),this,SLOT(slotStationInvisible()));
     connect(ui->relation_visible,SIGNAL(triggered()),this,SLOT(slotRelationVisible()));
     connect(ui->relation_invisible,SIGNAL(triggered()),this,SLOT(slotRelationInvisible()));
     connect(ui->point_rmkvisible,SIGNAL(triggered()),this,SLOT(slotPointRemarkVisible()));
     connect(ui->point_rmkinvisible,SIGNAL(triggered()),this,SLOT(slotPointRemarkInvisible()));
     connect(ui->station_rmkvisible,SIGNAL(triggered()),this,SLOT(slotStationRemarkVisible()));
     connect(ui->station_rmkinvisible,SIGNAL(triggered()),this,SLOT(slotStationRemarkInvisible()));


    //菜单————帮助
    connect(ui->Menubar_help,SIGNAL(triggered()),this,SLOT(slotHelp()));
    connect(ui->Menubar_about,SIGNAL(triggered()),this,SLOT(slotAboutus()));

    //右键菜单信号槽
    connect(attribute,SIGNAL(triggered()),this,SLOT(slotAttribute()));
    connect(bindRFID,SIGNAL(triggered()),this,SLOT(slotBindRFID()));
    connect(capacity,SIGNAL(triggered()),this,SLOT(slotCapacity()));
    connect(dirEnable,SIGNAL(triggered()),this,SLOT(slotDirEnable()));
    connect(nodeType,SIGNAL(triggered()),this,SLOT(slotNodeType()));
    connect(DeleteItem,SIGNAL(triggered()),this,SLOT(slotDeleteItem()));

    //主工具栏
    connect(ui->action_new,SIGNAL(triggered()),this,SLOT(slotNew()));
    connect(ui->action_open,SIGNAL(triggered()),this,SLOT(slotOpen()));
    connect(ui->action_close,SIGNAL(triggered()),this,SLOT(slotClose()));
    connect(ui->action_clear,SIGNAL(triggered()),this,SLOT(slotClear()));
    connect(ui->action_help,SIGNAL(triggered()),this,SLOT(slotHelp()));
    connect(ui->action_save,SIGNAL(triggered()),this,SLOT(slotSave()));

}

void MainWindow::initParam()
{
    LineMode2 = false;
    LineMode3 = false;
    LineMode4 = false;
    firstItem_num = -1; //初始化，区分未选中图元的情况
    isgetLineLink = false;  //决定searchlinechild是否具有找出路径段所接触的所以路径端点
    flag_save = false;
    flag_routeDisplay = true;
    flag_pointDisplay = true;
    flag_stationDisplay = true;
    flag_relationDisplay = true;
    flag_pointRmkDisplay = false;
    flag_stationRmkDisplay = false;
    count_routepoint = 0;
    count_route = 0;
    count_stationpoint = 0;
    count_twoPoints = 0;

    SelectCount = 0;
    scalingFactor = 1.0;
    new_realScaling = 1.0;
}

void MainWindow::initStyle()
{
    RoutePen.setWidth(3);
    RoutePen.setColor(Qt::green);
    ArrowPen.setWidth(3);
    ArrowPen.setColor(Qt::black);
    RelationPen.setWidth(2);
    RelationPen.setStyle(Qt::DashLine);
    RelationPen.setColor(Qt::darkGray);
    PointBrush.setColor(Qt::blue);
    PointBrush.setStyle(Qt::SolidPattern);
    StationBrush.setColor(Qt::red);
    StationBrush.setStyle(Qt::SolidPattern);
    RemarkPen.setColor(Qt::darkCyan);
}

void MainWindow::initToolbar()
{
    connect(ui->action_first,SIGNAL(triggered()),this,SLOT(slotAction_first()));
    connect(ui->action_merge,SIGNAL(triggered()),this,SLOT(slotAction_merge()));
    connect(ui->action_loop,SIGNAL(triggered()),this,SLOT(slotLoop()));
    connect(ui->action_station,SIGNAL(triggered()),this,SLOT(slotAddStationItem()));
    connect(ui->action_attribute,SIGNAL(triggered()),this,SLOT(slotAttribute()));
    connect(ui->action_RFID,SIGNAL(triggered()),this,SLOT(slotBindRFID()));
    connect(ui->action_capacity,SIGNAL(triggered()),this,SLOT(slotCapacity()));
    connect(ui->action_dir,SIGNAL(triggered()),this,SLOT(slotDirEnable()));
    connect(ui->action_type,SIGNAL(triggered()),this,SLOT(slotNodeType()));
    connect(ui->action_del,SIGNAL(triggered()),this,SLOT(slotDeleteItem()));

}

void MainWindow::initStatus()
{
    if(view->getFlagScaleChange())
    {
        new_realScaling = view->getRealScaling();
        qDebug() << "新比例" << new_realScaling << "标志" << view->getFlagScaleChange();
        view->setFlagScaleChange(false);
        view->centerOn(0,0);//滚动条出现在开头
    }

    if(scene->items(Qt::DescendingOrder).size() == 7)
    {
        ui->Menubar_loop->setEnabled(false);
        ui->action_loop->setEnabled(false);
    }
    else
    {
       ui->Menubar_loop->setEnabled(true);
       ui->action_loop->setEnabled(true);
    }

    //上方工具和菜单栏不变功能
    ui->action_new->setEnabled(true);
    ui->action_close->setEnabled(true);
    ui->action_save->setEnabled(true);
    ui->action_help->setEnabled(true);
    ui->action_clear->setEnabled(true);

    ui->Menubar_new->setEnabled(true);
    ui->Menubar_close->setEnabled(true);
    ui->Menubar_save->setEnabled(true);
    ui->Menubar_help->setEnabled(true);
    ui->Menubar_clear->setEnabled(true);

    if(scene->items(Qt::DescendingOrder).size() == 0)
    {
        ui->Menubar_search->setEnabled(false);
        ui->Menubar_selectall->setEnabled(false);
    }
    else
    {
        ui->Menubar_search->setEnabled(true);
        ui->Menubar_selectall->setEnabled(true);
    }

    if(scene->selectedItems().size() != 1)
    {
        //右键
        attribute->setEnabled(false);
        if(scene->selectedItems().size() == 0)
        {
            DeleteItem->setEnabled(false);
        }
        else
        {
            DeleteItem->setEnabled(true);
        }
        capacity->setEnabled(false);
        dirEnable->setEnabled(false);
        bindRFID->setEnabled(false);
        nodeType->setEnabled(false);

        //菜单栏
        ui->Menubar_first->setEnabled(false);
        ui->Menubar_merge->setEnabled(false);
        ui->Menubar_station->setEnabled(false);
        if(scene->selectedItems().size() == 0)
        {

           ui->Menubar_del->setEnabled(false);
        }
        else
        {

           ui->Menubar_del->setEnabled(true);
        }

        ui->Menubar_attribute->setEnabled(false);
        ui->Menubar_RFID->setEnabled(false);
        ui->Menubar_dir->setEnabled(false);
        ui->Menubar_capacity->setEnabled(false);
        ui->Menubar_type->setEnabled(false);

        //工具栏
        ui->action_first->setEnabled(false);
        if(scene->selectedItems().size() == 0)
        {
            ui->action_merge->setEnabled(false);
        }
        else
        {
            ui->action_merge->setEnabled(true);
        }
        ui->action_station->setEnabled(false);
        ui->action_RFID->setEnabled(false);
        ui->action_attribute->setEnabled(false);
        ui->action_capacity->setEnabled(false);
        if(scene->selectedItems().size() == 0)
        {

            ui->action_del->setEnabled(false);
        }
        else
        {
            ui->action_del->setEnabled(false);
        }

        ui->action_dir->setEnabled(false);
        ui->action_type->setEnabled(false);

    }
    else
    {
//        //共有的功能
        if(scene->selectedItems().first()->toolTip().split(",").first() != "line")
        {
            ui->Menubar_first->setEnabled(false);
            ui->action_first->setEnabled(false);

            capacity->setEnabled(false);
            ui->Menubar_capacity->setEnabled(false);
            ui->action_capacity->setEnabled(false);

            dirEnable->setEnabled(false);
            ui->Menubar_dir->setEnabled(false);
            ui->action_dir->setEnabled(false);

            ui->Menubar_station->setEnabled(false);
            ui->action_station->setEnabled(false);
        }

        attribute->setEnabled(true);
        ui->Menubar_attribute->setEnabled(true);
        ui->action_attribute->setEnabled(true);

        DeleteItem->setEnabled(true);
        ui->Menubar_del->setEnabled(true);
        ui->action_del->setEnabled(true);

        ui->action_clear->setEnabled(true);
        ui->Menubar_clear->setEnabled(true);


        QString select_type = scene->selectedItems().at(0)->toolTip().split(",").first();

        if(select_type == "line")   //选中路径
        {
            ui->Menubar_first->setEnabled(true);
            ui->action_first->setEnabled(true);
            ui->Menubar_station->setEnabled(true);
            ui->action_station->setEnabled(true);
            capacity->setEnabled(true);
            ui->Menubar_capacity->setEnabled(true);
            ui->action_capacity->setEnabled(true);
            ui->Menubar_dir->setEnabled(true);
            ui->action_dir->setEnabled(true);
            dirEnable->setEnabled(true);
            bindRFID->setEnabled(false);
            ui->Menubar_RFID->setEnabled(false);
            ui->action_RFID->setEnabled(false);
            nodeType->setEnabled(false);
            ui->Menubar_type->setEnabled(false);
            ui->action_type->setEnabled(false);

        }
        else if(select_type == "station")         //选中工位点
        {
            ui->Menubar_RFID->setEnabled(true);
            ui->action_RFID->setEnabled(true);
            bindRFID->setEnabled(true);
            ui->Menubar_type->setEnabled(true);
            ui->action_type->setEnabled(true);
            nodeType->setEnabled(true); 

        }
        else  //选中路径端点
        {
            bindRFID->setEnabled(true);
            ui->Menubar_RFID->setEnabled(true);
            ui->action_RFID->setEnabled(true);

        }
    }
}

void MainWindow::slotNew()
{
//    slotClear();
    MainWindow *newWin = new MainWindow;
    count_window++;
    newWin->count_window = count_window;
    newWin->changeTitle("");
    newWin->move(this->pos().x()+30,this->pos().y()+30);
    newWin->show();

}

void MainWindow::slotOpen()
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

    isFirstReference();
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        QMessageBox::warning(this,tr("Error"),tr("文件只读失败！"));
        file.close();
        return;
    }
    isOpenfile = true;
    openFilename = fileName;
    changeTitle(fileName);
    slotClear();
    flag_routeDisplay = true;
    flag_pointDisplay = true;
    flag_stationDisplay = true;
    flag_relationDisplay = true;
    flag_pointRmkDisplay = false;
    flag_stationRmkDisplay = false;
    QDomElement root = doc.documentElement();




    //画出所有路径段
    int count_xml_route = root.firstChildElement("路径段").childNodes().size();
    QDomNodeList xml_route = root.firstChildElement("路径段").childNodes();
    for(int j = 0;j < count_xml_route;j++)
    {
        //将内容写进结构体,并将结构体追加至链表
        Route temp_route;
        temp_route.path_num = getXmlValue(xml_route.at(j),"Num");
        temp_route.angle = getXmlValueF(xml_route.at(j),"Angle");
        temp_route.capacity = getXmlValue(xml_route.at(j),"Capacity");
        temp_route.enable_dir = getXmlValue(xml_route.at(j),"Dir");
        temp_route.start_num = getXmlValue(xml_route.at(j),"StartNum");
        temp_route.end_num = getXmlValue(xml_route.at(j),"EndNum");
        temp_route.length = getXmlValueF(xml_route.at(j),"Length");
        temp_route.contain_station = getXmlValueList(xml_route.at(j),"ContainStation");
        QList<double> temp_startpos = getXmlPosList(xml_route.at(j),"StartPos");
        temp_route.start_pos = QPointF(temp_startpos.first(),temp_startpos.last());
        double real_endposx = temp_startpos.first()+temp_route.length*cos(temp_route.angle/180.0*M_PI)*scalingFactor;
        double real_endposy = temp_startpos.last()-temp_route.length*sin(temp_route.angle/180.0*M_PI)*scalingFactor;
        temp_route.end_pos = QPointF(real_endposx,real_endposy);
        list_route.append(temp_route);

        //画主体
        QLineF LineItem;
        LineItem.setP1(temp_route.start_pos);
        LineItem.setAngle(temp_route.angle);
        LineItem.setLength(temp_route.length*scalingFactor);
        QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
        item->setPen(RoutePen);
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setToolTip(tr("line,%1").arg(j));
        scene->addItem(item);

        //画路径端点
        //起点
        QRectF startpoint;
        startpoint.setTopLeft(temp_route.start_pos-QPointF(5,5));
        startpoint.setBottomRight(temp_route.start_pos+QPointF(5,5));
        drawRect(startpoint,item,"point",temp_route.start_num);
//        QGraphicsEllipseItem *PointStartItem = new QGraphicsEllipseItem(startpoint);
//        PointStartItem->setBrush(PointBrush);
//        PointStartItem->setFlag(QGraphicsItem::ItemIsSelectable);
//        PointStartItem->setToolTip(tr("point,%1").arg(temp_route.start_num));
//        PointStartItem->setParentItem(item);

        drawRemark(temp_route.start_pos,item,"point",temp_route.start_num);
//        QGraphicsSimpleTextItem *textStartItem = new QGraphicsSimpleTextItem;
//        textStartItem->setPen(RemarkPen);
//        textStartItem->setText(QString("端点%1").arg(temp_route.start_num));
//        textStartItem->setParentItem(item);
//        textStartItem->setToolTip(tr("remark,%1").arg(temp_route.start_num));
//        textStartItem->setPos(temp_route.start_pos+QPointF(-15,10));
//        textStartItem->setVisible(flag_pointRmkDisplay);

        //终点

        QRectF endpoint;
        endpoint.setTopLeft(temp_route.end_pos-QPointF(5,5));
        endpoint.setBottomRight(temp_route.end_pos+QPointF(5,5));
        drawRect(endpoint,item,"point",temp_route.end_num);
//        QGraphicsEllipseItem *PointEndItem = new QGraphicsEllipseItem(endpoint);
//        PointEndItem->setBrush(PointBrush);
//        PointEndItem->setFlag(QGraphicsItem::ItemIsSelectable);
//        PointEndItem->setToolTip(tr("point,%1").arg(temp_route.end_num));
//        PointEndItem->setParentItem(item);

        drawRemark(temp_route.end_pos,item,"point",temp_route.end_num);
//        QGraphicsSimpleTextItem *textEndItem = new QGraphicsSimpleTextItem;
//        textEndItem->setPen(RemarkPen);
//        textEndItem->setText(QString("端点%1").arg(temp_route.end_num));
//        textEndItem->setParentItem(item);
//        textEndItem->setToolTip(tr("remark,%1").arg(temp_route.end_num));
//        textEndItem->setPos(temp_route.end_pos+QPointF(-15,10));
//        textEndItem->setVisible(flag_pointRmkDisplay);

    }
    count_route = count_xml_route;

    //画出所有工位点
    int count_xml_station = root.firstChildElement("工位点").childNodes().size();
    QDomNodeList xml_station = root.firstChildElement("工位点").childNodes();
    for(int k = 0;k < count_xml_station;k++)
    {
        StationPoint station;
        station.node_num = getXmlValue(xml_station.at(k),"Num");
        station.length = getXmlValueF(xml_station.at(k),"Length");
        station.RFID_num = getXmlRFID(xml_station.at(k),"RFID");
        station.attach_num = getXmlValue(xml_station.at(k),"Attach");
        station.node_type = getXmlValue(xml_station.at(k),"Type");

        list_stationpoint.append(station);

        //找到对应的附属路径在图元中的编号
        QList<QGraphicsItem*> total_routes = searchItem("line");
        int attachNum = 0;
        for(int n = 0;n < total_routes.size();n++)
        {
            if(total_routes.at(n)->toolTip().split(",").last().toInt() == station.attach_num)
            {
                attachNum = n;
                break;
            }
        }

        QPointF station_pos = QPointF(list_route[station.attach_num].start_pos.x()+station.length*scalingFactor*cos(list_route[station.attach_num].angle/180.0*M_PI),
                                      list_route[station.attach_num].start_pos.y()-station.length*scalingFactor*sin(list_route[station.attach_num].angle/180.0*M_PI));
        QRectF station_Rect;
        station_Rect.setTopLeft(station_pos+QPointF(-5,-5));
        station_Rect.setBottomRight(station_pos+QPointF(5,5));
        drawRect(station_Rect,total_routes.at(attachNum),"station",k);
//        QGraphicsRectItem *stationitem = new QGraphicsRectItem(station_Rect);
//        stationitem->setBrush(StationBrush);
//        stationitem->setFlag(QGraphicsItem::ItemIsSelectable);
//        stationitem->setParentItem(total_routes.at(attachNum));
//        stationitem->setToolTip(tr("station,%1").arg(k));
//        scene->addItem(stationitem);

        drawRemark(station_Rect.topLeft(),total_routes.at(attachNum),"station",station.node_num);
//        QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem;
//        textItem->setPen(RemarkPen);
//        textItem->setParentItem(total_routes.at(attachNum));
//        textItem->setText(QString("工位点%1").arg(station.node_num));
//        textItem->setToolTip(tr("station_remark,%1").arg(station.node_num));
//        textItem->setPos(station_Rect.topLeft()+QPointF(-10,15));
//        textItem->setVisible(flag_stationRmkDisplay);
//        scene->addItem(textItem);

    }
    count_stationpoint = count_xml_station;

    QList<QGraphicsItem *> total_route = searchItem("line");
    QList<QGraphicsItem *> total_station = searchItem("station");
    QList<QGraphicsItem *> total_point = searchItem("point");
    //路径端点设定父对象

    //箭头绘制和设定父对象(需要同时改动slot_Enable_Dir)
    for(int i = 0;i < total_route.size();i++)
    {
        int temp_routeNum = total_route.at(i)->toolTip().split(",").last().toInt();
        QPointF jointPoint;
        jointPoint = list_route[temp_routeNum].start_pos+(list_route[temp_routeNum].end_pos-list_route[temp_routeNum].start_pos)/2;

        drawArrow(jointPoint,list_route[temp_routeNum].angle,total_route.at(i));
//        QLineF arrowLeft;
//        QLineF arrowRight;
//        arrowLeft.setP1(jointPoint);
//        arrowRight.setP1(jointPoint);
//        arrowLeft.setLength(8);
//        arrowRight.setLength(8);
//        arrowLeft.setAngle(list_route[temp_routeNum].angle+180+30);
//        arrowRight.setAngle(list_route[temp_routeNum].angle+180-30);
//        QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
//        QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
//        arrowLeftItem->setLine(arrowLeft);
//        arrowRightItem->setLine(arrowRight);
//        arrowLeftItem->setPen(ArrowPen);
//        arrowRightItem->setPen(ArrowPen);
//        arrowLeftItem->setParentItem(total_route.at(i));
//        arrowRightItem->setParentItem(total_route.at(i));

    }

    //读取所有路径端点
    int count_xml_point = root.firstChildElement("路径端点").childNodes().size();
    QDomNodeList xml_point = root.firstChildElement("路径端点").childNodes();
    for(int i = 0;i < count_xml_point;i++)
    {
        //将内容写入结构体,将结构体追加到链表
        RoutePoint temp_routepoint;
        temp_routepoint.node_num = getXmlValue(xml_point.at(i),"Num");
        temp_routepoint.pos_x = getXmlValueF(xml_point.at(i),"PosX");
        temp_routepoint.pos_y = getXmlValueF(xml_point.at(i),"PosY");
        temp_routepoint.RFID_num = getXmlRFID(xml_point.at(i),"RFID");
        temp_routepoint.node_type = getXmlValue(xml_point.at(i),"Type");
        int count_line_enter = getXmlValueList(xml_point.at(i),"LineEnter").size();
        for(int m = 0;m < count_line_enter; m++)
        {
            temp_routepoint.line_enter.append(getXmlValueList(xml_point.at(i),"LineEnter").at(m));
        }
        int count_line_exit = getXmlValueList(xml_point.at(i),"LineExit").size();
        for(int m = 0;m < count_line_exit; m++)
        {
            temp_routepoint.line_exit.append(getXmlValueList(xml_point.at(i),"LineExit").at(m));
        }

        list_routepoint.append(temp_routepoint);

    }
    count_routepoint = count_xml_point;
    if(!flag_isDefaultPos)
    {
        list_routepoint[0].pos_x = reference_PosX;
        list_routepoint[0].pos_y = reference_PosY;
    }
    else
    {
        reference_PosX = list_routepoint[0].pos_x;
        reference_PosY = list_routepoint[0].pos_y;
    }



    //工位点设置父对象
    for(int i = 0;i < count_xml_station; i++)
    {
        int temp_index = -1;
        //找该工位点对应的附属线在total_route中的编号
        for(int j = 0;j < total_route.size();j++)
        {
            if(total_route.at(j)->toolTip().split(",").last().toInt() == list_stationpoint[total_station.at(i)->toolTip().split(",").last().toInt()].attach_num)
            {
                temp_index = j;
                break;
            }
        }
        total_station.at(i)->setParentItem(total_route.at(temp_index));
    }

    //画关系线
    QLineF relationLine;
    for(int k = 0;k < count_xml_point;k++)
    {
        QList<QPointF> screen_pos;
        for(int a = 0;a < list_route.size();a++)
        {
           if(list_route[a].start_num == k)
           {
               screen_pos.append(list_route[a].start_pos);
           }
           else if(list_route[a].end_num == k)
           {
               screen_pos.append(list_route[a].end_pos);
           }
           else
           {
               continue;
           }
        }
        if(screen_pos.size() != 1)
        {
            for(int b = 0;b < screen_pos.size()-1;b++)
            {
                for(int m = b+1 ;m < screen_pos.size(); m++)
                {
                    drawRelationLine(relationLine,screen_pos.at(b),screen_pos.at(m),k);
//                    relationLine.setP1(screen_pos.at(b));
//                    relationLine.setP2(screen_pos.at(m));
//                    QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);
//                    relationLineItem->setToolTip(tr("relation,%1").arg(k));
//                    relationLineItem->setPen(RelationPen);
//                    scene->addItem(relationLineItem);

                }
            }
        }

    }

    //读完文件关闭
    file.close();

}


void MainWindow::slotExtraSave()
{
    flag_extraSave = true;
    slotSave();
}

void MainWindow::slotSave()
{

    //获取路由信息，并调用弗洛伊德算法
    Floyd floyd(list_routepoint.size(),createFloydInfo());

    //如果有不可达的，弹警告
    for(int m = 0;m < list_routepoint.size();m++)
    {
        bool flag_continue = false; //保证只弹出一次警告
        for(int n = 0;n < list_routepoint.size();n++)
        {
            if(floyd.dis[m][n] == INT_MAX)
            {
               int ret = QMessageBox::warning(this,tr("警告："),tr("该地图存在不可达点，是否继续保存？"),QMessageBox::Yes|QMessageBox::Cancel);
               if(ret == QMessageBox::Yes)
               {
                   flag_continue = true;
                   break;
               }
               else
               {
                   return;
               }
            }
        }
        if(flag_continue)
        {
            break;
        }
    }

    QString fileName;
    if(flag_extraSave) //强制另存为
    {
        fileName = QFileDialog::getSaveFileName(this,tr("save file"),"untitled",tr("XML Files(*.xml)"));
        if(fileName.isEmpty())
        {
            QMessageBox::warning(this,tr("Error"),tr("名字不能为空！"));
            return;
        }
        flag_extraSave = false;
    }
    else
    {
        if(isOpenfile) //若是修改已打开文件，直接保存修改，否则另存为
        {
            fileName = openFilename;
            isOpenfile = false;
        }
        else
        {
            fileName = QFileDialog::getSaveFileName(this,tr("save file"),"untitled",tr("XML Files(*.xml)"));
            if(fileName.isEmpty())
            {
                QMessageBox::warning(this,tr("Error"),tr("名字不能为空！"));
                return;
            }
        }
    }



    updateList();
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        QMessageBox::warning(this,tr("Error"),tr("文件只写失败！"));
        return;
    }

    QDomDocument doc;
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version = \"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);
    QDomElement root = doc.createElement("地图创建模块数据结构");
    doc.appendChild(root);

    QDomElement xml_route = doc.createElement("路径段");
    root.appendChild(xml_route);
    QDomElement xml_point = doc.createElement("路径端点");
    root.appendChild(xml_point);
    QDomElement xml_station = doc.createElement("工位点");
    root.appendChild(xml_station);
    QDomElement xml_distance = doc.createElement("距离表");
    root.appendChild(xml_distance);
    QDomElement xml_path = doc.createElement("路由表");
    root.appendChild(xml_path);

    //添加至路径段
    for(int i = 0;i<list_route.size();i++)
    {
        QDomElement route = doc.createElement("Route");

        QDomAttr route_num = doc.createAttribute("Num");
        route_num.setValue(tr("%1").arg(list_route[i].path_num));
        route.setAttributeNode(route_num);

        QDomAttr route_start = doc.createAttribute("StartNum");
        route_start.setValue(tr("%1").arg(list_route[i].start_num));
        route.setAttributeNode(route_start);

        QDomAttr route_end = doc.createAttribute("EndNum");
        route_end.setValue(tr("%1").arg(list_route[i].end_num));
        route.setAttributeNode(route_end);

        QDomAttr route_length = doc.createAttribute("Length");
        route_length.setValue(tr("%1").arg(list_route[i].length));
        route.setAttributeNode(route_length);

        QDomAttr route_angle = doc.createAttribute("Angle");
        route_angle.setValue(tr("%1").arg(list_route[i].angle));
        route.setAttributeNode(route_angle);

        QDomAttr route_dir = doc.createAttribute("Dir");
        route_dir.setValue(tr("%1").arg(list_route[i].enable_dir));
        route.setAttributeNode(route_dir);

        QDomAttr route_capacity = doc.createAttribute("Capacity");
        route_capacity.setValue(tr("%1").arg(list_route[i].capacity));
        route.setAttributeNode(route_capacity);

        QString temp_routeStart;
        temp_routeStart.append(QString("%1").arg(list_route[i].start_pos.x()));
        temp_routeStart.append(",");
        temp_routeStart.append(QString("%1").arg(list_route[i].start_pos.y()));
        QDomAttr route_startpos = doc.createAttribute("StartPos");
        route_startpos.setValue(temp_routeStart);
        route.setAttributeNode(route_startpos);

        QString temp_routeEnd;
        temp_routeEnd.append(QString("%1").arg(list_route[i].end_pos.x()));
        temp_routeEnd.append(",");
        temp_routeEnd.append(QString("%1").arg(list_route[i].end_pos.y()));
        QDomAttr route_endpos = doc.createAttribute("EndPos");
        route_endpos.setValue(temp_routeEnd);
        route.setAttributeNode(route_endpos);

        QString temp_routeContain;
        for(int m = 0;m < list_route[i].contain_station.size();m++)
        {
            temp_routeContain.append(QString("%1").arg(list_route[i].contain_station.at(m)));
            if( m != list_route[i].contain_station.size()-1)  //最后一个不加逗号
            {
               temp_routeContain.append(",");
            }
        }
        QDomAttr route_contain = doc.createAttribute("ContainStation");
        route_contain.setValue(temp_routeContain);
        route.setAttributeNode(route_contain);

        xml_route.appendChild(route);
    }

    //添加至路径端点
    for(int j = 0;j < list_routepoint.size();j++)
    {
        QDomElement point = doc.createElement("RoutePoint");

        QDomAttr point_num = doc.createAttribute("Num");
        point_num.setValue(tr("%1").arg(list_routepoint[j].node_num));
        point.setAttributeNode(point_num);

        QDomAttr point_RFID = doc.createAttribute("RFID");
        point_RFID.setValue(tr("%1").arg(list_routepoint[j].RFID_num));
        point.setAttributeNode(point_RFID);

        QDomAttr point_class = doc.createAttribute("Type");
        point_class.setValue(tr("%1").arg(list_routepoint[j].node_type));
        point.setAttributeNode(point_class);

        QString temp_pointIn;
        for(int i = 0;i< list_routepoint[j].line_enter.size();i++)
        {
            temp_pointIn.append(QString("%1").arg(list_routepoint[j].line_enter.at(i)));
            if( i != list_routepoint[j].line_enter.size()-1)  //最后一个不加逗号
            {
               temp_pointIn.append(",");
            }
        }
        QDomAttr point_enter = doc.createAttribute("LineEnter");
        point_enter.setValue(temp_pointIn);
        point.setAttributeNode(point_enter);

        QString temp_pointOut;
        for(int i = 0;i< list_routepoint[j].line_exit.size();i++)
        {
            temp_pointOut.append(QString("%1").arg(list_routepoint[j].line_exit.at(i)));
            if( i != list_routepoint[j].line_exit.size()-1)  //最后一个不加逗号
            {
               temp_pointOut.append(",");
            }
        }
        QDomAttr point_exit = doc.createAttribute("LineExit");
        point_exit.setValue(temp_pointOut);
        point.setAttributeNode(point_exit);

        QDomAttr point_x = doc.createAttribute("PosX");
        point_x.setValue(tr("%1").arg(list_routepoint[j].pos_x));
        point.setAttributeNode(point_x);

        QDomAttr point_y = doc.createAttribute("PosY");
        point_y.setValue(tr("%1").arg(list_routepoint[j].pos_y));
        point.setAttributeNode(point_y);

        xml_point.appendChild(point);
    }

    //添加至工位点
    for(int k = 0;k < list_stationpoint.size();k++)
    {
        QDomElement station = doc.createElement("StationPoint");

        QDomAttr station_num = doc.createAttribute("Num");
        station_num.setValue(tr("%1").arg(list_stationpoint[k].node_num));
        station.setAttributeNode(station_num);

        QDomAttr station_RFID = doc.createAttribute("RFID");
        station_RFID.setValue(tr("%1").arg(list_stationpoint[k].RFID_num));
        station.setAttributeNode(station_RFID);

        QDomAttr station_type = doc.createAttribute("Type");
        station_type.setValue(tr("%1").arg(list_stationpoint[k].node_type));
        station.setAttributeNode(station_type);

        QDomAttr station_attach = doc.createAttribute("Attach");
        station_attach.setValue(tr("%1").arg(list_stationpoint[k].attach_num));
        station.setAttributeNode(station_attach);

        QDomAttr station_length = doc.createAttribute("Length");
        station_length.setValue(tr("%1").arg(list_stationpoint[k].length));
        station.setAttributeNode(station_length);

        xml_station.appendChild(station);
    }


    //添加至距离表
    for(int row = 0;row < list_routepoint.size();row++)
    {
        QString strlist;
        for(int col = 0;col < list_routepoint.size();col++)
        {
            strlist.append(QString::number(floyd.dis[row][col]));
            if(col != list_routepoint.size()-1)
            {
                strlist.append(",");
            }
            else
            {
                strlist.append(";");
            }

        }

        QDomElement row_Num = doc.createElement(QString("Row%1").arg(row));
        QDomAttr row_content = doc.createAttribute("Content");
        row_content.setValue(strlist);
        row_Num.setAttributeNode(row_content);
        xml_distance.appendChild(row_Num);
    }

    //添加至路由表
    for(int row = 0;row < list_routepoint.size();row++)
    {
        QString strlist;
        for(int col = 0;col < list_routepoint.size();col++)
        {
            strlist.append(QString::number(floyd.path[row][col]));
            if(col != list_routepoint.size()-1)
            {
               strlist.append(",");
            }
            else
            {
                strlist.append(";");
            }

        }

        QDomElement row_Num = doc.createElement(QString("Row%1").arg(row));
        QDomAttr row_content = doc.createAttribute("Content");
        row_content.setValue(strlist);
        row_Num.setAttributeNode(row_content);
        xml_path.appendChild(row_Num);
    }

    //保存xml
    QTextStream out(&file);
    doc.save(out,4);
    file.close();
    changeTitle(fileName);
    flag_save = true;
}

void MainWindow::changeTitle(QString filename)
{
        if(filename.isEmpty())
        {
            this->setWindowTitle(QString("地图创建模块————无标题%1").arg(count_window));
        }
        else
        {
            QFileInfo fi(filename);
            QString name = fi.fileName();
            this->setWindowTitle(tr("%1").arg(name));
        }

}

void MainWindow::slotClose()
{
    this->close();
}

void MainWindow::slotClear()
{
    QList<QGraphicsItem *> listItem = scene->items();
    while (!listItem.empty())
    {
        scene->removeItem(listItem.at(0));
        listItem.removeAt(0);
    }
    list_route.clear();
    list_routepoint.clear();
    list_stationpoint.clear();
    list_realpoint.clear();
    existence.clear();
    count_route = 0;
    count_routepoint = 0;
    count_stationpoint = 0;
}


void MainWindow::slotDelete()
{
    delete_item();

}

void MainWindow::slotSelectAll()
{
    QList<QGraphicsItem*> total_items = scene->items(Qt::DescendingOrder);
    for(int i = 0; i < total_items.size(); i++)
    {
        total_items.at(i)->setSelected(true);
    }
}

void MainWindow::slotSearch()
{
    dialog_search = new Dialog_search;
    dialog_search->exec();
    if(dialog_search->flag_search)
    {
        int search_num = dialog_search->getSearchContent().toInt();
        QString search_RFID = dialog_search->getSearchContent();
        int index = -1;
        QList<QGraphicsItem*> total_route = searchItem("line");
        QList<QGraphicsItem*> total_point = searchItem("point");
        QList<QGraphicsItem*> total_station = searchItem("station");
        switch(dialog_search->getSearchIndex())
        {
            case 0: //查找路径编号
                 if(search_num >= list_route.size())
                 {
                     QMessageBox::warning(this,tr("错误："),tr("无法找到该路径"));
                 }
                 else
                 {

                     for(int i = 0;i < total_route.size();i++)
                     {
                         if(total_route.at(i)->toolTip().split(",").last().toInt() == search_num)
                         {
                             total_route.at(i)->setSelected(true);
                         }
                         else
                         {
                             total_route.at(i)->setSelected(false);
                         }
                     }
                 }
                 break;

            case 1: //查找路径端点编号
                 if(search_num >= list_routepoint.size())
                 {
                     QMessageBox::warning(this,tr("错误："),tr("无法找到该路径端点"));
                 }
                 else
                 {

                     for(int i = 0;i < total_point.size();i++)
                     {
                         if(total_point.at(i)->toolTip().split(",").last().toInt() == search_num)
                         {
                             total_point.at(i)->setSelected(true);
                         }
                         else
                         {
                             total_point.at(i)->setSelected(false);
                         }
                     }
                 }
                 break;

           case 2: //查找路径端点RFID

                 for(int i = 0;i < list_routepoint.size();i++)
                 {
                     if(list_routepoint.at(i).RFID_num == search_RFID)
                     {
                         index = i;
                     }
                 }
                 if(index == -1)
                 {
                     QMessageBox::warning(this,tr("错误："),tr("无法找到该路径端点"));
                     return;
                 }
                 for(int j = 0;j < total_point.size();j++)
                 {
                     if(total_point.at(j)->toolTip().split(",").last().toInt() == index)
                     {
                         total_point.at(j)->setSelected(true);
                     }
                     else
                     {
                         total_point.at(j)->setSelected(false);
                     }
                 }
                 break;

           case 3: //查找工位点编号
                if(search_num >= list_stationpoint.size())
                {
                    QMessageBox::warning(this,tr("错误："),tr("无法找到该工位点"));
                }
                else
                {

                    for(int i = 0;i < total_station.size();i++)
                    {
                        if(total_station.at(i)->toolTip().split(",").last().toInt() == search_num)
                        {
                            total_station.at(i)->setSelected(true);
                        }
                        else
                        {
                            total_station.at(i)->setSelected(false);
                        }
                    }
               }
               break;

          case 4: //查找工位点RFID
                for(int i = 0;i < list_stationpoint.size();i++)
                {
                   if(list_stationpoint.at(i).RFID_num == search_RFID)
                   {
                       index = i;
                   }
                }
                if(index == -1)
                {
                    QMessageBox::warning(this,tr("错误："),tr("无法找到该工位点"));
                    return;
                }
                for(int j = 0;j < total_station.size();j++)
                {
                    if(total_station.at(j)->toolTip().split(",").last().toInt() == index)
                    {
                        total_station.at(j)->setSelected(true);
                    }
                    else
                    {
                        total_station.at(j)->setSelected(false);
                    }
                }
                break;


         default:
              break;

        }
    }
    delete dialog_search;
}

void MainWindow::slotAddLineItem()
{
    isFirstReference();

    Dialog_input *dialog_Line = new Dialog_input(this);
    dialog_Line->ui->lineEdit_StartX->setFocus();
    dialog_Line->exec();
    if(dialog_Line->getFlagCancel())
    {
        return;
    }


    //设定直线属性
    QLineF LineItem;
    LineItem.setP1(QPointF(dialog_Line->getInputInfo("X"),dialog_Line->getInputInfo("Y")));
    LineItem.setAngle(dialog_Line->getInputInfo("Angle"));
    LineItem.setLength(dialog_Line->getInputInfo("Length")*scalingFactor);
    QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
    item->setPen(RoutePen);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setToolTip(tr("line,%1").arg(count_route));  //设定备注，利于判别选中的为什么图形
    scene->addItem(item);

    //画路径端点
    drawRoutePoint(dialog_Line,item);
    drawRouteDir(dialog_Line,item);
    delete dialog_Line;

}

void MainWindow::slotAddLineItem_Press()
{

    isFirstReference();
    LineMode2 = true;
    LineMode3 = false;
    LineMode4 = false;
    count_twoPoints = 0;

}



void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if(LineMode2)
    {
            //可提醒鼠标输入第一点
            Dialog_input *dialog_Line = new Dialog_input(this);
            dialog_Line = new Dialog_input(this);
            dialog_Line->ui->lineEdit_StartX->setEnabled(false);
            dialog_Line->ui->lineEdit_StartY->setEnabled(false);
            dialog_Line->ui->SpinBox_Length->setFocus();
            dialog_Line->exec();
            if(dialog_Line->getFlagCancel())
            {
                LineMode2=false;
                return;
            }

            QLineF LineItem;
            LineItem.setP1(QPointF(Press_Pos.x(),Press_Pos.y()));
            LineItem.setAngle(dialog_Line->getInputInfo("Angle"));
            LineItem.setLength(dialog_Line->getInputInfo("Length")*scalingFactor);
            QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
            item->setPen(RoutePen);
            item->setFlag(QGraphicsItem::ItemIsMovable);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
            item->setToolTip(tr("line,%1").arg(count_route));
            scene->addItem(item);

            //画两路径端点
            drawRoutePoint(dialog_Line,item);
            drawRouteDir(dialog_Line,item);
            delete dialog_Line;
            LineMode2=false;

    }
    if(e->modifiers()==Qt::SHIFT)
    {


        if(SelectCount==0)
        {
            SelectItemRect.setTopLeft(Press_Pos);
            SelectCount++;
        }
        else
        {
            SelectItemRect.setBottomRight(Press_Pos);
            SelectCount=0;
            QList<QGraphicsItem *> list=scene->items(SelectItemRect);
            for(int j=0;j<list.count();j++)
            {
                list.at(j)->setSelected(true);
            }
        }
    }


}


/*******************************************键盘事件*************************************************/
void MainWindow::keyPressEvent(QKeyEvent *k)
{
    if(k->key()==Qt::Key_Delete)
    {
        delete_item();
    }
    else if(k->key() == Qt::Key_Escape)
    {
        count_twoPoints = 0;
    }
}

void MainWindow::drawRect(QRectF rect,QGraphicsItem* parent,QString type,int No)
{

    if(type == "point")
    {
        QGraphicsEllipseItem *Item = new QGraphicsEllipseItem(rect);
        Item->setFlag(QGraphicsItem::ItemIsSelectable);
        Item->setParentItem(parent);
        Item->setBrush(PointBrush);
        Item->setToolTip(tr("point,%1").arg(No));
        Item->setVisible(flag_pointDisplay);
    }
    else if(type == "station")
    {
        QGraphicsRectItem *Item = new QGraphicsRectItem(rect);
        Item->setFlag(QGraphicsItem::ItemIsSelectable);
        Item->setParentItem(parent);
        Item->setBrush(StationBrush);
        Item->setToolTip(tr("station,%1").arg(No));
        Item->setVisible(flag_stationDisplay);
    }
    else
    {
        qDebug() << "drawRect失败";
        return;
    }


}

void MainWindow::drawRemark(QPointF point,QGraphicsItem* parent,QString type,int No)
{
    QGraphicsSimpleTextItem *Item = new QGraphicsSimpleTextItem;
    Item->setPen(RemarkPen);
    Item->setParentItem(parent);
    Item->setPos(point+QPointF(-10,15));

    if(type == "point")
    {
        Item->setText(QString("端点%1").arg(No));
        Item->setToolTip(tr("remark,%1").arg(No));
        Item->setVisible(flag_pointRmkDisplay);
    }
    else if(type == "station")
    {
        Item->setText(QString("工位点%1").arg(No));
        Item->setToolTip(tr("station_remark,%1").arg(No));
        Item->setVisible(flag_stationRmkDisplay);
    }
    else
    {
        qDebug() << "drawRemark失败";
        return;
    }


}

void MainWindow::drawArrow(QPointF middlePoint,double angle,QGraphicsItem* parent)
{
    //设定箭头属性
    QLineF arrow_dir_left;
    QLineF arrow_dir_right;
    arrow_dir_left.setP1(middlePoint);
    arrow_dir_right.setP1(middlePoint);
    arrow_dir_left.setLength(8);
    arrow_dir_right.setLength(8);
    arrow_dir_left.setAngle(180.0+angle+30.0);
    arrow_dir_right.setAngle(180.0+angle-30.0);
    QGraphicsLineItem *arrow_item_left = new QGraphicsLineItem(arrow_dir_left);
    QGraphicsLineItem *arrow_item_right = new QGraphicsLineItem(arrow_dir_right);

    arrow_item_left->setPen(ArrowPen);
    arrow_item_right->setPen(ArrowPen);
    arrow_item_left->setParentItem(parent);
    arrow_item_right->setParentItem(parent);
    arrow_item_left->setToolTip("arrow,single");
    arrow_item_right->setToolTip("arrow,single");
}

void MainWindow::drawRelationLine(QLineF& relationLine,const QPointF& one,const QPointF& other,int No)
{
    relationLine.setP1(one);
    relationLine.setP2(other);
    QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);

    relationLineItem->setToolTip(tr("relation,%1").arg(No));
    relationLineItem->setPen(RelationPen);
    relationLineItem->setVisible(flag_relationDisplay);
    scene->addItem(relationLineItem);
}

/**********************************画路径端点**************************************/
void MainWindow::drawRoutePoint(Dialog_input *dialog,QGraphicsLineItem *parent)
{
    //设定端点属性
    //设定起点属性
    QRectF StartPoint;
    if(LineMode2)
    {
        StartPoint.setTopLeft(QPointF(Press_Pos.x()-5,Press_Pos.y()-5));
        StartPoint.setBottomRight(QPointF(Press_Pos.x()+5,Press_Pos.y()+5));
        if(flag_isDefaultPos && scene->items(Qt::DescendingOrder).size() <= 1) //默认模式，并且是第一点(由于先创建了LineItem，故存在个数为1)
        {
            reference_PosX = Press_Pos.x();
            reference_PosY = Press_Pos.y();
        }
    }
    else
    {
        StartPoint.setTopLeft(QPointF(dialog->getInputInfo("X")-5,dialog->getInputInfo("Y")-5));
        StartPoint.setBottomRight(QPointF(dialog->getInputInfo("X")+5,dialog->getInputInfo("Y")+5));
        if(flag_isDefaultPos && scene->items(Qt::DescendingOrder).size() <= 1)
        {
            reference_PosX = dialog->getInputInfo("X");
            reference_PosY = dialog->getInputInfo("Y");
        }
    }

    drawRect(StartPoint,parent,"point",count_routepoint);//创建起点
//    QGraphicsEllipseItem *StartPointItem = new QGraphicsEllipseItem(StartPoint);
//    StartPointItem->setBrush(PointBrush);
//    StartPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//    StartPointItem->setToolTip(tr("point,%1").arg(count_routepoint));
//    StartPointItem->setParentItem(parent);
//    StartPointItem->setVisible(flag_pointDisplay);


    drawRemark(StartPoint.topLeft(),parent,"point",count_routepoint);
//    QGraphicsSimpleTextItem *textStartItem = new QGraphicsSimpleTextItem;
//    textStartItem->setPen(RemarkPen);
//    textStartItem->setText(QString("端点%1").arg(count_routepoint));
//    textStartItem->setParentItem(parent);
//    textStartItem->setToolTip(tr("remark,%1").arg(count_routepoint));
//    textStartItem->setPos(StartPoint.topLeft()+QPointF(-10,15));
//    textStartItem->setVisible(flag_pointRmkDisplay);


    //将创建的起点加入链表
    RoutePoint routepoint_start;
    routepoint_start.node_num = count_routepoint;
    routepoint_start.RFID_num = "0";
    count_routepoint++;


    if(LineMode2)
    {
        routepoint_start.pos_x = Press_Pos.x();
        routepoint_start.pos_y = Press_Pos.y();
    }
    else
    {
        routepoint_start.pos_x = dialog->getInputInfo("X");
        routepoint_start.pos_y = dialog->getInputInfo("Y");
    }


    list_routepoint.append(routepoint_start);

    //设置终点属性
    QRectF EndPoint;
    if(LineMode2)
    {
        EndPoint.setTopLeft(QPointF(Press_Pos.x()+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI)-5,
                                    Press_Pos.y()-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI)-5));
        EndPoint.setBottomRight(QPointF(Press_Pos.x()+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI)+5,
                                        Press_Pos.y()-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI)+5));
    }
    else
    {
        EndPoint.setTopLeft(QPointF(dialog->getInputInfo("X")+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI)-5,
                                    dialog->getInputInfo("Y")-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI)-5));
        EndPoint.setBottomRight(QPointF(dialog->getInputInfo("X")+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI)+5,
                                        dialog->getInputInfo("Y")-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI)+5));
    }

    drawRect(EndPoint,parent,"point",count_routepoint);//创建起点
//    QGraphicsEllipseItem *EndPointItem = new QGraphicsEllipseItem(EndPoint);
//    EndPointItem->setBrush(PointBrush);
//    EndPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//    EndPointItem->setToolTip(tr("point,%1").arg(count_routepoint));
//    EndPointItem->setParentItem(parent);
//    EndPointItem->setVisible(flag_pointDisplay);

    drawRemark(EndPoint.topLeft(),parent,"point",count_routepoint);
//    QGraphicsSimpleTextItem *textEndItem = new QGraphicsSimpleTextItem;
//    textEndItem->setPen(RemarkPen);
//    textEndItem->setText(QString("端点%1").arg(count_routepoint));
//    textEndItem->setParentItem(parent);
//    textEndItem->setToolTip(tr("remark,%1").arg(count_routepoint));
//    textEndItem->setPos(EndPoint.topLeft()+QPointF(-10,15));
//    textEndItem->setVisible(flag_pointRmkDisplay);

    //将创建的终点加入链表

    RoutePoint routepoint_end;
    routepoint_end.node_num = count_routepoint;
    routepoint_end.pos_x = EndPoint.topLeft().x()+5;
    routepoint_end.pos_y = EndPoint.topLeft().y()+5;
    routepoint_end.RFID_num = "0";
    list_routepoint.append(routepoint_end);
    count_routepoint++;

    //将创建的路径加入链表
    Route route;
    route.path_num=count_route;
    route.length=dialog->getInputInfo("Length");
    route.angle=dialog->getInputInfo("Angle");
    route.enable_dir = 1;
    route.capacity = 1;
    route.start_num = count_routepoint-2;
    route.end_num = count_routepoint-1;
    route.start_pos = QPointF(routepoint_start.pos_x,routepoint_start.pos_y);
    route.end_pos = QPointF(routepoint_end.pos_x,routepoint_end.pos_y);
    route.old_start = route.start_pos; //保存上一次位置
    route.old_end = route.end_pos;
    list_route.append(route);
    count_route++;

}

/********************************画路径段方向**************************************/
void MainWindow::drawRouteDir(Dialog_input *dialog,QGraphicsLineItem *parent)
{

      QPointF middle_point;


    //获取中点
    if(LineMode2)
    {
       middle_point = QPointF((Press_Pos.x()+Press_Pos.x()+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI))/2,
                            (Press_Pos.y()+Press_Pos.y()-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI))/2);
    }
    else
    {
       middle_point = QPointF((dialog->getInputInfo("X")+dialog->getInputInfo("X")+dialog->getInputInfo("Length")*scalingFactor*cos(dialog->getInputInfo("Angle")/180.0*M_PI))/2,
                            (dialog->getInputInfo("Y")+dialog->getInputInfo("Y")-dialog->getInputInfo("Length")*scalingFactor*sin(dialog->getInputInfo("Angle")/180.0*M_PI))/2);
    }


    //设定箭头属性
    drawArrow(middle_point,dialog->getInputInfo("Angle"),parent);
//    QLineF arrow_dir_left;
//    QLineF arrow_dir_right;
//    arrow_dir_left.setP1(middle_point);
//    arrow_dir_right.setP1(middle_point);
//    arrow_dir_left.setLength(8);
//    arrow_dir_right.setLength(8);
//    arrow_dir_left.setAngle(180.0+dialog->value_angle+30.0);
//    arrow_dir_right.setAngle(180.0+dialog->value_angle-30.0);
//    QGraphicsLineItem *arrow_item_left = new QGraphicsLineItem(arrow_dir_left);
//    QGraphicsLineItem *arrow_item_right = new QGraphicsLineItem(arrow_dir_right);

//    arrow_item_left->setPen(ArrowPen);
//    arrow_item_right->setPen(ArrowPen);
//    arrow_item_left->setParentItem(parent);
//    arrow_item_right->setParentItem(parent);
//    arrow_item_left->setToolTip("arrow,single");
//    arrow_item_right->setToolTip("arrow,single");

}

//可初始化按钮状态，当且仅当只选中一条直线时，可触发
void MainWindow::slotAddStationItem()
{
    if(scene->selectedItems().size() != 1 || scene->selectedItems().at(0)->toolTip().split(",").first() != "line")
    {
        QMessageBox::warning(this,tr("Error:"),tr("只能选择一条路径添加工位点！"));
        return;
    }
    int route_num = scene->selectedItems().at(0)->toolTip().split(",").last().toInt();
    bool isOk = true ;
    double templength = QInputDialog::getDouble(this,tr("Tips:"),tr("请输入距离该路径起点的距离"),0,0,static_cast<int>(list_route[route_num].length),1,&isOk);
    if(isOk != true)
    {
        return;
    }

    QPointF station_pos = QPointF(list_route[route_num].start_pos.x()+templength*scalingFactor*cos(list_route[route_num].angle/180.0*M_PI),
                                  list_route[route_num].start_pos.y()-templength*scalingFactor*sin(list_route[route_num].angle/180.0*M_PI));
    QRectF station_Rect;
    station_Rect.setTopLeft(station_pos+QPointF(-5,-5));
    station_Rect.setBottomRight(station_pos+QPointF(5,5));
    drawRect(station_Rect,scene->selectedItems().at(0),"station",count_stationpoint);
//    QGraphicsRectItem *item = new QGraphicsRectItem(station_Rect);
//    item->setBrush(StationBrush);
//    item->setFlag(QGraphicsItem::ItemIsSelectable);
//    item->setParentItem(scene->selectedItems().at(0));
//    item->setToolTip(tr("station,%1").arg(count_stationpoint));
//    item->setVisible(flag_stationDisplay);

    drawRemark(station_Rect.topLeft(),scene->selectedItems().at(0),"station",count_stationpoint);
//    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem;
//    textItem->setPen(RemarkPen);
//    textItem->setText(QString("工位点%1").arg(count_stationpoint));
//    textItem->setParentItem(scene->selectedItems().at(0));
//    textItem->setToolTip(tr("station_remark,%1").arg(count_stationpoint));
//    textItem->setPos(station_Rect.topLeft()+QPointF(-10,15));
//    textItem->setVisible(flag_stationRmkDisplay);

    //将创建的工位点加入链表
    StationPoint stationpoint;
    stationpoint.node_num = count_stationpoint;
    stationpoint.length = templength;
    stationpoint.attach_num = route_num;
    stationpoint.node_type = 0;
    stationpoint.RFID_num = "0";
    list_stationpoint.append(stationpoint);
    count_stationpoint++;

}


void MainWindow::delete_item()
{
    //更新一遍链表，方便改动工位点附属线
    updateList();
    //临时存储链表
     QList<Route> temp_list_route = list_route;
     QList<RoutePoint> temp_list_routepoint = list_routepoint;
     QList<StationPoint> temp_list_stationpoint = list_stationpoint;
     QList<QGraphicsItem *> select_items;
     select_items=scene->selectedItems();
     //未选中
     if(select_items.size()<1)
     {
         return;
     }

     //如果只剩最后一个路径，则直接调用清空函数
     if(searchItem("line").size() == 1)
     {
         slotClear();
         return;
     }
     //处理选中图元
     for(int i = 0 ; i< select_items.size(); i++)
     {
         /*******************************删除路径段***********************************/
         if(select_items.at(i)->toolTip().split(",").first() == "line")
         {
             QList<int> temp_station_num = searchLineChild(select_items.at(i),"station");
             int select_route_num = select_items.at(i)->toolTip().split(",").last().toInt();
             QList<int> temp_routepoint_num;
             temp_routepoint_num.append(list_route[select_route_num].start_num);
             temp_routepoint_num.append(list_route[select_route_num].end_num);

             //找到所有line,并保存在temp_sort链表
             QList<QGraphicsItem *> temp_sort = searchItem("line");


             //将删除图元编号之后的tooltip全部减小1
             for(int j = 0;j < temp_sort.size();j++)
             {
                 if(temp_sort[j]->toolTip().split(",").last().toInt() > select_items.at(i)->toolTip().split(",").last().toInt())
                 {

                         temp_sort[j]->setToolTip(tr("line,%1").arg(temp_sort[j]->toolTip().split(",").last().toInt()-1));

                 }
             }
             temp_list_route.removeAt(select_items.at(i)->toolTip().split(",").last().toInt());//从临时列表中移除该路径段
             //将后续编号向前挪动
             for(int z= select_items.at(i)->toolTip().split(",").last().toInt();z<temp_list_route.size();z++)
             {
                 temp_list_route[z].path_num-=1;
             }
             count_route--;


             //删除附属工位点
             if(temp_station_num.size() != 0)
             {
                 //作为子对象的工位点图元tooltip和链表均需要刷新
                 QList<QGraphicsItem *> temp_sort_station = searchItem("station");

                 if(temp_sort_station.size() !=0)
                 {
                     //改变工位点附属线编号
                     for(int s = 0;s < temp_sort_station.size();s++)
                     {
                         if(temp_list_stationpoint[s].attach_num > select_route_num)
                         {
                             temp_list_stationpoint[s].attach_num -= 1;
                         }
                     }


                     for(int t = temp_station_num.size()-1;t >=0 ;t--)
                     {

                         //工位点的tooltip前移
                         for(int j = 0;j < temp_sort_station.size();j++)
                         {
                            //序号后面的，tooltip前移1
                            if(temp_sort_station[j]->toolTip().split(",").last().toInt() > temp_station_num.at(t))
                            {
                                temp_sort_station[j]->setToolTip(tr("station,%1").arg(temp_sort_station[j]->toolTip().split(",").last().toInt()-1));
                            }
                         }

                         temp_list_stationpoint.removeAt(temp_station_num.at(t));

                         //链表号改变
                         for(int z= temp_station_num.at(t);z<temp_list_stationpoint.size();z++)
                         {
                            temp_list_stationpoint[z].node_num-=1;
                         }
                     }

                     //改变工位点个数
                     for(int j = 0;j< temp_station_num.size();j++)
                     {
                         count_stationpoint--;
                     }
                 }
             }

             //删除路径端点————若仍然有该编号的路径端点在，则无需操作
             QList<QGraphicsItem *> total_point = searchItem("point");
             int count_start = 0;
             int count_end = 0;
             bool flag_start = false;
             bool flag_end = false;
             for(int k = 0;k < total_point.size();k++)
             {
                 if(total_point.at(k)->toolTip().split(",").last().toInt() == temp_routepoint_num.first()) //仍然存有起点编号的端点
                 {
                    count_start++;
                 }
                 if(total_point.at(k)->toolTip().split(",").last().toInt() == temp_routepoint_num.last())
                 {
                     count_end++;
                 }
             }
             //查看除了本身，是否还有其他路径共用该点
             if(count_start > 1)
             {
                 flag_start = true;
             }
             if(count_end > 1)
             {
                 flag_end = true;
             }

             if(flag_start && flag_end)
             {

             }
             else if(flag_start && !flag_end) //起点路径端点链表不变，终点改变
             {
                   //路径端点的tooltip前移
                   for(int j = 0;j < total_point.size();j++)
                   {
                      //序号后面的，tooltip前移1个位子
                      if(total_point[j]->toolTip().split(",").last().toInt() > temp_routepoint_num.last())
                       {
                            total_point[j]->setToolTip(tr("point,%1").arg(total_point[j]->toolTip().split(",").last().toInt()-1));
                       }
                   }

                   temp_list_routepoint.removeAt(temp_routepoint_num.last());

                   for(int z= temp_routepoint_num.last();z<temp_list_routepoint.size();z++)
                   {
                         temp_list_routepoint[z].node_num-=1;
                   }
                   count_routepoint--;
             }
             else if(!flag_start && flag_end) //终点路径端点链表不变，起点改变
             {
                 //路径端点的tooltip前移
                 for(int j = 0;j < total_point.size();j++)
                 {
                    //序号后面的，tooltip前移1个位子
                    if(total_point[j]->toolTip().split(",").last().toInt() > temp_routepoint_num.first())
                     {
                          total_point[j]->setToolTip(tr("point,%1").arg(total_point[j]->toolTip().split(",").last().toInt()-1));
                     }
                 }

                 temp_list_routepoint.removeAt(temp_routepoint_num.first());

                 for(int z= temp_routepoint_num.first();z<temp_list_routepoint.size();z++)
                 {
                       temp_list_routepoint[z].node_num-=1;
                 }
                 count_routepoint--;
             }
             else //起点终点均改变
             {
                 for(int t = temp_routepoint_num.size()-1; t >= 0;t--)
                 {
                     //路径端点的tooltip前移
                     for(int j = 0;j < total_point.size();j++)
                      {
                          //序号后面的，tooltip前移1个位子
                          if(total_point[j]->toolTip().split(",").last().toInt() > temp_routepoint_num.at(t))
                          {
                              total_point[j]->setToolTip(tr("point,%1").arg(total_point[j]->toolTip().split(",").last().toInt()-1));
                          }
                       }

                       temp_list_routepoint.removeAt(temp_routepoint_num.at(t));

                       for(int z= temp_routepoint_num.at(t);z<temp_list_routepoint.size();z++)
                       {
                           temp_list_routepoint[z].node_num-=1;
                       }

                        count_routepoint--;
                }
             }

             //删除关系线,只删除与该直线连接的关系线
             QRectF RectStart;
             QRectF RectEnd;

             RectStart.setTopLeft(list_route[select_route_num].start_pos+QPointF(-3,-3));
             RectStart.setBottomRight(list_route[select_route_num].start_pos+QPointF(3,3));
             RectEnd.setTopLeft(list_route[select_route_num].end_pos+QPointF(-3,-3));
             RectEnd.setBottomRight(list_route[select_route_num].end_pos+QPointF(3,3));
             QList<QGraphicsItem *> CollideStartItems = scene->items(RectStart);
             QList<QGraphicsItem *> CollideEndItems = scene->items(RectEnd);
             for(int p = 0;p < CollideStartItems.size();p++)
             {
                 if(CollideStartItems.at(p)->toolTip().split(",").first() == "relation")
                 {
                     scene->removeItem(CollideStartItems.at(p));
                 }
             }
             for(int p = 0;p < CollideEndItems.size();p++)
             {
                 if(CollideEndItems.at(p)->toolTip().split(",").first() == "relation")
                 {
                     scene->removeItem(CollideEndItems.at(p));
                 }
             }

             scene->removeItem(select_items.at(i));

         }
         /********************************删除路径端点（不可删除）******************************/
         else if(select_items.at(i)->toolTip().split(",").first() == "point")
         {
              QMessageBox::warning(this,tr("Error"),tr("路径端点不可删除！"));
         }
         /*********************************删除工位点****************************************/
         else
         {
             scene->removeItem(select_items.at(i));
             QList<QGraphicsItem *> temp_sort_station = searchItem("station");

             //工位点的tooltip前移
             for(int j = 0;j < temp_sort_station.size();j++)
             {
                  //序号后面的，tooltip前移1
                  if(temp_sort_station[j]->toolTip().split(",").last().toInt() > select_items.at(i)->toolTip().split(",").last().toInt())
                  {
                      temp_sort_station[j]->setToolTip(tr("station,%1").arg(temp_sort_station[j]->toolTip().split(",").last().toInt()-1));
                  }
             }

             temp_list_stationpoint.removeAt(select_items.at(i)->toolTip().split(",").last().toInt());

             for(int z= select_items.at(i)->toolTip().split(",").last().toInt();z<temp_list_stationpoint.size();z++)
             {
                 temp_list_stationpoint[z].node_num-=1;
             }
         }
     }

     //将临时链表附给真实链表
     list_route = temp_list_route;
     list_routepoint = temp_list_routepoint;
     list_stationpoint = temp_list_stationpoint;

}

void MainWindow::slotTwoPoints()
{
    isFirstReference();
    LineMode2 = false;
    LineMode3 = true;
    LineMode4 = false;
    count_twoPoints = 0;
}

void MainWindow::slotHelp()
{
    QMessageBox::information(this,tr("Help"),tr("帮助内容未写！"));
}

void MainWindow::slotAboutus()
{
     QMessageBox::aboutQt(this);
}



/**************************************合并规则****************************************/
//比较第一路径终点编号A与第二路径起点编号B，当B>A时，把A的编号给B；当B<A时，将B的编号赋给A（取小者）
void MainWindow::slotAction_merge()
{
    if( scene->selectedItems().size() != 2)
    {
        QMessageBox::warning(this,tr("Error:"),tr("请重新选则两条路径"));
        return;
    }
    for(int i =0 ;i < 2;i++)
    {
        if(scene->selectedItems().at(i)->toolTip().split(",").first() != "line")
        {
            QMessageBox::warning(this,tr("Error:"),tr("请重新选则两条路径"));
            return;
        }
    }


    //记录两直线的编号
    int firstroute_num = firstItem_num;
    int secondroute_num = 0;
//    int moveline_index=0; //所需移动的直线再选中图元中的下标
    for(int i = 0; i<2 ; i++)
    {
        if(scene->selectedItems().at(i)->toolTip().split(",").last().toInt() != firstItem_num )
        {
            secondroute_num = scene->selectedItems().at(i)->toolTip().split(",").last().toInt();
//            moveline_index = i;
            break;
        }
    }

//    QList<QGraphicsItem*> temp_routepoint = searchItem("point");


    int child_second = list_route[secondroute_num].start_num; //第二路径原先起点
    int child_first = list_route[firstroute_num].end_num; //第一路径原先终点

    //找到所有的端点
    QList<QGraphicsItem *> total_routepoint = searchItem("point");
    QList<QGraphicsItem*> total_route = searchItem("line");

    //从所有端点找到要第二条线的起点
    int change_num = child_first > child_second ? child_first : child_second;
    int other_num = child_first < child_second ? child_first : child_second;
//    if(change_num == child_first)
//    {
//        other_num = child_second;
//    }
//    else
//    {
//        other_num = child_first;
//    }



//    QList<int> change_index;
//    for(int j = 0;j<total_routepoint.size();j++)
//    {
//        if(total_routepoint[j]->toolTip().split(",").last().toInt() == change_num)
//        {
//            change_index.append(j);
//        }
//    }


//    //改变起点与终点tooltip,大编号改小编号
//    for(int k = 0;k < change_index.size();k++)
//    {
//        total_routepoint.at(change_index.at(k))->setToolTip(tr("point,%1").arg(other_num));
//        qDebug() << "原先index" << change_num << "现代改为" << other_num;
//    }
    for(int i = 0; i < total_routepoint.size(); i++)//改
    {
        if(total_routepoint[i]->toolTip().split(",").last().toInt() == change_num)
        {
            total_routepoint[i]->setToolTip(tr("point,%1").arg(other_num));
        }
        else if(total_routepoint[i]->toolTip().split(",").last().toInt() > change_num)
        {
            total_routepoint[i]->setToolTip(tr("point,%1").arg(total_routepoint[i]->toolTip().split(",").last().toInt()-1));
        }

    }

    //更改备注及其tooltip，正好需更改的编号直接改变为小编号，大于大编号的需要减一
    QList<QGraphicsItem*> total_remark = searchItem("remark");
    for(int z = 0;z < total_remark.size();z++)
    {
        QGraphicsSimpleTextItem* tempTextItem = static_cast<QGraphicsSimpleTextItem*>(total_remark.at(z));
        if(tempTextItem->toolTip().split(",").last().toInt() == change_num)
        {
            tempTextItem->setText(QString("端点%1").arg(other_num));
            tempTextItem->setToolTip(tr("remark,%1").arg(other_num));
        }
        else if(tempTextItem->toolTip().split(",").last().toInt() > change_num)
        {
            tempTextItem->setText(QString("端点%1").arg(tempTextItem->toolTip().split(",").last().toInt()-1));
            tempTextItem->setToolTip(tr("remark,%1").arg(tempTextItem->toolTip().split(",").last().toInt()-1));//改
        }
    }



//        static_cast<QGraphicsSimpleTextItem*>(total_remark.at(0))->setText("");
////        QList<QGraphicsTextItem*> temp_total_remark = static_cast<QList<QGraphicsTextItem*>>(total_remark);
//        for(int z = 0;z < total_remark.size();z++)
//        {
//            scene->removeItem(total_remark.at(z)); //不能直接更新text，需要重画
//        }
//        QList<QGraphicsItem*> temp_totalPoints = searchItem("point");
//        for(int m = 0;m < temp_totalPoints.size();m++)
//        {
//            int temp_pointNum = temp_totalPoints.at(m)->toolTip().split(",").last().toInt();
//            QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem;
//            textItem->setPen(RemarkPen);
//            textItem->setText(QString("端点%1").arg(temp_pointNum));
////            textItem->setParentItem(parent);
//            textItem->setToolTip(tr("remark,%1").arg(temp_pointNum));
////            textItem->setPos(temp_pointNum.topLeft()+QPointF(-10,15));
//            textItem->setVisible(flag_pointRmkDisplay);

//        }




    //改变路径连接关系
    for(int k = 0;k < list_route.size();k++)
    {
        if(list_route[k].start_num == change_num)
        {
            list_route[k].start_num = other_num;
        }
        if(list_route[k].end_num == change_num)
        {
            list_route[k].end_num = other_num;
        }
    }


//    //端点tooltip前移
//    for(int i = 0; i < total_routepoint.size(); i++)
//    {
//        if(total_routepoint[i]->toolTip().split(",").last().toInt() > change_num)
//        {
//            total_routepoint[i]->setToolTip(tr("point,%1").arg(total_routepoint[i]->toolTip().split(",").last().toInt()-1));
//        }
//    }

//    for(int i = 0;i < total_remark.size();i++)
//    {
//        if(total_remark[i]->toolTip().split(",").last().toInt() > change_num)
//        {
//            total_remark[i]->setToolTip(tr("remark,%1").arg(total_remark[i]->toolTip().split(",").last().toInt()-1));
//        }
//    }


    //删除一个节点后，改变路径端点链表其后的编号
    for(int z = change_num;z < list_routepoint.size();z++)
    {
        list_routepoint[z].node_num-=1;
    }
    //路径链接中起点、终点若大于等于删除点编号，则需改动
    for(int p = 0;p < list_route.size();p++)
    {
        if(list_route[p].start_num > change_num)
        {
            list_route[p].start_num -=1;
        }
        else if(list_route[p].start_num == change_num) //本身起点编号等于改动点
        {
            list_route[p].start_num = other_num;
        }
        else if(list_route[p].end_num == change_num) //本身终点编号等于改动点
        {
            list_route[p].end_num = other_num;
        }
        if(list_route[p].end_num > change_num)
        {
            list_route[p].end_num -=1;
        }
    }
    list_routepoint.removeAt(change_num);
    count_routepoint--;

    //画关系线
    QLineF relationLine;
    QList<QPointF> relationPoint;
    //记录所有与移动线条相同编号点的坐标
    for(int j = 0 ; j < list_route.size(); j++)
    {
        if(list_route[j].start_num == other_num)
        {
            relationPoint.append(list_route[j].start_pos);
        }
        else if(list_route[j].end_num == other_num)
        {
            relationPoint.append(list_route[j].end_pos);
        }
        else
        {
            continue;
        }

    }

    //画编号相同的点
    for(int k = 0;k < relationPoint.size()-1;k++)
    {
        for(int m = k+1 ;m < relationPoint.size();m++)
        {
            drawRelationLine(relationLine,relationPoint.at(k),relationPoint.at(m),other_num);
//            relationLine.setP1(relationPoint.at(k));
//            relationLine.setP2(relationPoint.at(m));
//            QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);
//            relationLineItem->setToolTip(tr("relation,%1").arg(other_num));
//            relationLineItem->setPen(RelationPen);
//            relationLineItem->setVisible(flag_relationDisplay);
//            scene->addItem(relationLineItem);

        }
    }

}

void MainWindow::slotLoop()
{
   LineMode4 = true;
   LineMode2 = false;
   LineMode3 = false;
   count_twoPoints = 0;
}

void MainWindow::slotAction_first()
{
    //保存指定的第一点
    if(scene->selectedItems().size() != 1)
    {
        QMessageBox::warning(this,tr("Error"),tr("指定第一点个数必须为1"));
        return;
    }
    if(scene->selectedItems().first()->toolTip().split(",").first() == "station")
    {
        QMessageBox::warning(this,tr("Error"),tr("工位点不能指定为第一点"));
        return;
    }
    firstItem_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
}


QList<int> MainWindow::searchLineChild(QGraphicsItem *parent,QString type)
{
    QList<int> childnum;
    if(type == "station")
    {

        for(int i =0;i < parent->childItems().size();i++)
        {
            if(parent->childItems().at(i)->toolTip().split(",").first() == "station")
            {
                childnum.append(parent->childItems().at(i)->toolTip().split(",").last().toInt());
            }
        }
        //工位点从小到大排序
        quickSort(childnum,0,childnum.size()-1);
//        for(int j = 0;j < childnum.size();j++)
//        {
//            for(int k = 0;k < childnum.size()-1;k++)
//            {
//                if(childnum[k]>childnum[k+1])
//                {
//                    int temp;
//                    temp = childnum[k];
//                    childnum[k] = childnum[k+1];
//                    childnum[k+1] = temp;
//                }
//            }
//        }
    }
    else if(type == "point") //未用
    {

        if(!isgetLineLink)
        {
            int count = 0;//记录point个数，以区分只含有一个端点或无端点
            int index = 0;
            for(int i = 0;i < parent->childItems().size();i++)
            {
                if(parent->childItems().at(i)->toolTip().split(",").first() == "point")
                {
                   count++;
                   index = i;
                }
            }
            if(count == 0)
            {
                childnum.clear();
                return childnum;
            }
            if(count == 1)  //以防删除直线时将不属于的端点一起删除
            {
               childnum.append(parent->childItems().at(index)->toolTip().split(",").last().toInt());
               return childnum; //该路径的尾端
            }
        }

         QTransform transform;
         int temp_routenum = parent->toolTip().split(",").last().toInt() ;
         int temp_start_num = 0;
         int temp_end_num = 0;


         //确保返回的是起点的编号
         if(scene->itemAt(list_route[temp_routenum].start_pos+QPointF(-4.5,-2.2),transform)->toolTip().split(",").first() == "point")
         {
             temp_start_num = scene->itemAt(list_route[temp_routenum].start_pos+QPointF(-4.5,-2.2),transform)->toolTip().split(",").last().toInt();  //端点的位置进行找相交的路径端点
         }
         else
         {
             QGraphicsItem *truePoint;
             QList<QGraphicsItem *>invisibleItem;
             truePoint = scene->itemAt(list_route[temp_routenum].start_pos+QPointF(-4.5,-2.2),transform);
             while(truePoint->toolTip().split(",").first() != "point")
             {
                 truePoint->setVisible(false);  //设置不可见，以消除不是端点带来的影响
                 invisibleItem.append(truePoint);
                 truePoint = scene->itemAt(list_route[temp_routenum].start_pos+QPointF(-4.5,-2.2),transform);
             }
             temp_start_num = truePoint->toolTip().split(",").last().toInt();
             for(int i = 0;i< invisibleItem.size();i++)
             {
                 invisibleItem[i]->setVisible(true);
             }
         }

         //确保返回的是终点编号
         if(scene->itemAt(list_route[temp_routenum].end_pos+QPointF(-4.5,-2.2),transform)->toolTip().split(",").first() == "point")
         {
             temp_end_num = scene->itemAt(list_route[temp_routenum].end_pos+QPointF(-4.5,-2.2),transform)->toolTip().split(",").last().toInt();  //端点的位置进行找相交的路径端点
         }
         else
         {
             QGraphicsItem *truePoint;
             QList<QGraphicsItem *>invisibleItem;
             truePoint = scene->itemAt(list_route[temp_routenum].end_pos+QPointF(-4.5,-2.2),transform);
             while(truePoint->toolTip().split(",").first() != "point")
             {
                 truePoint->setVisible(false);  //设置不可见，以消除不是端点带来的影响
                 invisibleItem.append(truePoint);
                 truePoint = scene->itemAt(list_route[temp_routenum].end_pos+QPointF(-4.5,-2.2),transform);
             }
             temp_end_num = truePoint->toolTip().split(",").last().toInt();
             for(int i = 0;i< invisibleItem.size();i++)
             {
                 invisibleItem[i]->setVisible(true);
             }
         }

         childnum.append(temp_start_num);  //起点
         childnum.append(temp_end_num);  //终点


    }
    else
    {
        QMessageBox::warning(this,tr("Error:"),tr("该路径不存在此子类型！"));
    }
    return childnum;

}

/************************************找到所有指定类型的图元*******************************************/
QList<QGraphicsItem*> MainWindow::searchItem(QString type)
{
    QList<QGraphicsItem*> output;
    if(type == "line" || type == "station" || type == "point" || type == "relation" || type == "remark" || type == "station_remark")
    {
        for(int i = 0 ; i < scene->items(Qt::DescendingOrder).size(); i++)
        {
            if(scene->items(Qt::DescendingOrder).at(i)->toolTip().split(",").first() == type)
            {
                output.append(scene->items(Qt::DescendingOrder).at(i));
            }
        }
    }
    else
    {
        QMessageBox::warning(this,tr("Error"),tr("无法找到该类型图元"));
    }
    return output;

}


/***************************************右键菜单*****************************************/
void MainWindow::contextMenuEvent(QContextMenuEvent *)
{
    ContextMenu = new QMenu;
    //将动作加入右键菜单
    ContextMenu->addAction(attribute);
    ContextMenu->addSeparator();
    ContextMenu->addAction(bindRFID);
    ContextMenu->addAction(capacity);
    ContextMenu->addAction(dirEnable);
    ContextMenu->addAction(nodeType);
    ContextMenu->addSeparator();
    ContextMenu->addAction(DeleteItem);
    ContextMenu->exec(QCursor::pos());
    delete  ContextMenu;
}

void MainWindow::slotAttribute()
{
    //需刷新一波
    updateList();
    if(scene->selectedItems().size() == 0)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("查看对象不能为空！"));
        return;
    }
    else if(scene->selectedItems().size() > 1)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("同时只能查看一个对象属性！"));
        return;
    }
    else
    {
        QString select_type = scene->selectedItems().first()->toolTip().split(",").first();
        int select_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
         dialog_attribute = new Dialog_attribute;
        if(select_type == "line")
        {
            dialog_attribute->ui->tabWidget->setCurrentIndex(0);
            dialog_attribute->ui->tabWidget->setTabEnabled(1,false);
            dialog_attribute->ui->tabWidget->setTabEnabled(2,false);
            //给弹窗添加内容
            dialog_attribute->ui->routeType->setText("路径段");
            dialog_attribute->ui->routeNum->setText(QString("%1").arg(list_route[select_num].path_num));
            dialog_attribute->ui->routeStart->setText(QString("%1").arg(list_route[select_num].start_num));
            dialog_attribute->ui->routeEnd->setText(QString("%1").arg(list_route[select_num].end_num));
            dialog_attribute->ui->routeLength->setText(QString("%1").arg(list_route[select_num].length));
            dialog_attribute->ui->routeAngle->setText(QString("%1").arg(list_route[select_num].angle));
            dialog_attribute->ui->routeDir->setText(QString("%1").arg(list_route[select_num].enable_dir));
            dialog_attribute->ui->routeCapacity->setText(QString("%1").arg(list_route[select_num].capacity));
            QString temp_routeContain;
            for(int i = 0;i< list_route[select_num].contain_station.size();i++)
            {
                temp_routeContain.append(QString("%1").arg(list_route[select_num].contain_station.at(i)));
                if( i != list_route[select_num].contain_station.size()-1)  //最后一个不加逗号
                {
                   temp_routeContain.append(",");
                }
            }
            dialog_attribute->ui->routeContain->setText(temp_routeContain);

        }
        else if(select_type == "station")
        {
            dialog_attribute->ui->tabWidget->setCurrentIndex(1);
            dialog_attribute->ui->tabWidget->setTabEnabled(0,false);
            dialog_attribute->ui->tabWidget->setTabEnabled(2,false);
            //给弹窗添加内容
            dialog_attribute->ui->stationType->setText("工位点");
            dialog_attribute->ui->stationNum->setText(QString("%1").arg(list_stationpoint[select_num].node_num));
            dialog_attribute->ui->stationRFID->setText(QString("%1").arg(list_stationpoint[select_num].RFID_num));
            dialog_attribute->ui->stationClass->setText(QString("%1").arg(list_stationpoint[select_num].node_type));
            dialog_attribute->ui->stationAttach->setText(QString("%1").arg(list_stationpoint[select_num].attach_num));
            dialog_attribute->ui->stationLength->setText(QString("%1").arg(list_stationpoint[select_num].length));

        }
        else
        {
            dialog_attribute->ui->tabWidget->setCurrentIndex(2);
            dialog_attribute->ui->tabWidget->setTabEnabled(0,false);
            dialog_attribute->ui->tabWidget->setTabEnabled(1,false);
            //给弹窗添加内容
            dialog_attribute->ui->pointType->setText("路径端点");
            dialog_attribute->ui->pointNum->setText(QString("%1").arg(list_routepoint[select_num].node_num));
            dialog_attribute->ui->pointRFID->setText(QString("%1").arg(list_routepoint[select_num].RFID_num));
            dialog_attribute->ui->pointClass->setText(QString("%1").arg(list_routepoint[select_num].node_type));
            QString temp_pointIn;
            for(int i = 0;i< list_routepoint[select_num].line_enter.size();i++)
            {
                temp_pointIn.append(QString("%1").arg(list_routepoint[select_num].line_enter.at(i)));
                if( i != list_routepoint[select_num].line_enter.size()-1)  //最后一个不加逗号
                {
                   temp_pointIn.append(",");
                }
            }
            dialog_attribute->ui->pointIn->setText(temp_pointIn);

            QString temp_pointOut;
            for(int i = 0;i< list_routepoint[select_num].line_exit.size();i++)
            {

                temp_pointOut.append(QString("%1").arg(list_routepoint[select_num].line_exit.at(i)));
                if( i != list_routepoint[select_num].line_exit.size()-1)  //最后一个不加逗号
                {
                   temp_pointOut.append(",");
                }
            }

            dialog_attribute->ui->pointOut->setText(tr("%1").arg(temp_pointOut));
            dialog_attribute->ui->pointXpos->setText(QString("%1").arg(list_routepoint[select_num].pos_x));
            dialog_attribute->ui->pointYpos->setText(QString("%1").arg(list_routepoint[select_num].pos_y));
        }
        dialog_attribute->exec();
        if(!dialog_attribute->getFlagValid() && !dialog_attribute->getFlagStation())
        {
            return;
        }
        else if(dialog_attribute->getFlagValid())//更新路径形状
        {
            list_route[select_num].length = dialog_attribute->ui->routeLength->text().toDouble();
            list_route[select_num].angle = dialog_attribute->ui->routeAngle->text().toDouble();
            list_route[select_num].end_pos = QPointF(list_route[select_num].start_pos.x()+list_route[select_num].length*cos(list_route[select_num].angle/180.0*M_PI),
                                                     list_route[select_num].start_pos.y()-list_route[select_num].length*sin(list_route[select_num].angle/180.0*M_PI));
            update_route(scene->selectedItems().first());
        }
        else //更新工位点
        {
            double templength = dialog_attribute->ui->stationLength->text().toDouble();
            list_stationpoint[select_num].length = templength;
            scene->removeItem(scene->selectedItems().first());
            //找到对应的备注，并删除
            QList<QGraphicsItem*> total_stationRemarks = searchItem("station_remark");
            int delRemarkNum = 0;
            for(int m = 0;m < total_stationRemarks.size();m++)
            {
                if(total_stationRemarks.at(m)->toolTip().split(",").last().toInt() == select_num)
                {
                    delRemarkNum = m;
                    break;
                }
            }
            scene->removeItem(total_stationRemarks.at(delRemarkNum));

            //找到对应的附属路径在图元中的编号
            QList<QGraphicsItem*> total_routes = searchItem("line");
            int attachNum = 0;
            for(int n = 0;n < total_routes.size();n++)
            {
                if(total_routes.at(n)->toolTip().split(",").last().toInt() == list_stationpoint[select_num].attach_num)
                {
                    attachNum = n;
                    break;
                }
            }
            int route_num = list_stationpoint[select_num].attach_num;
            QPointF station_pos = QPointF(list_route[route_num].start_pos.x()+templength*scalingFactor*cos(list_route[route_num].angle/180.0*M_PI),
                                          list_route[route_num].start_pos.y()-templength*scalingFactor*sin(list_route[route_num].angle/180.0*M_PI));
            QRectF station_Rect;
            station_Rect.setTopLeft(station_pos+QPointF(-5,-5));
            station_Rect.setBottomRight(station_pos+QPointF(5,5));
            drawRect(station_Rect,total_routes.at(attachNum),"station",select_num);
//            QGraphicsRectItem *item = new QGraphicsRectItem(station_Rect);
//            item->setBrush(StationBrush);
//            item->setFlag(QGraphicsItem::ItemIsSelectable);
//            item->setParentItem(total_routes.at(attachNum));
//            item->setToolTip(tr("station,%1").arg(select_num));
//            item->setVisible(flag_stationDisplay);

            drawRemark(station_Rect.topLeft(),total_routes.at(attachNum),"station",select_num);
//            QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem;
//            textItem->setPen(RemarkPen);
//            textItem->setText(QString("工位点%1").arg(select_num));
//            textItem->setParentItem(total_routes.at(attachNum));
//            textItem->setToolTip(tr("station_remark,%1").arg(select_num));
//            textItem->setPos(station_Rect.topLeft()+QPointF(-10,15));
//            textItem->setVisible(flag_stationRmkDisplay);

        }
    }
    delete  dialog_attribute;
}

void MainWindow::slotBindRFID()
{
    //判断选中操作是否有效
    if(scene->selectedItems().size() == 0)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("必须选中一个工位点或路径端点！"));
        return;
    }
    else if(scene->selectedItems().size() > 1)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("选中工位点或路径端点个数只能为1！"));
        return;
    }
    else
    {
        if(scene->selectedItems().first()->toolTip().split(",").first() == "line")
        {
            QMessageBox::warning(this,tr("Tips:"),tr("路径端不可绑定RFID！"));
            return;
        }
    }

    int select_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
    QString select_type = scene->selectedItems().first()->toolTip().split(",").first();
    bool isOk = true;
    if(select_type == "point")
    {
        QString temp_pointRFID = QInputDialog::getText(this,tr("Tips:"),tr("请输入路径端点的RFID号"),QLineEdit::Normal,tr("4位十六进制数"),&isOk);
        if(isOk != true)
        {
            return;
        }
        while(!isRFID(temp_pointRFID))
        {
            qDebug()<< "输入RFID" << temp_pointRFID;
            temp_pointRFID = QInputDialog::getText(this,tr("Tips:"),tr("请输入路径端点的RFID号"),QLineEdit::Normal,tr("4位十六进制数"),&isOk); //千万不能再声明一次，这样while里面的变量为之前的tempPointRFID

            if(isOk != true)
            {
                return;
            }
        }
        list_routepoint[select_num].RFID_num = temp_pointRFID;
    }
    else
    {
        QString temp_stationRFID = QInputDialog::getText(this,tr("Tips:"),tr("请输入工位点的RFID号"),QLineEdit::Normal,tr("4位十六进制数"),&isOk);
        if(isOk != true)
        {
            return;
        }
        while(!isRFID(temp_stationRFID))
        {
            QString temp_stationRFID = QInputDialog::getText(this,tr("Tips:"),tr("请输入工位点的RFID号"),QLineEdit::Normal,tr("4位十六进制数"),&isOk);
            if(isOk != true)
            {
                return;
            }
        }
        list_stationpoint[select_num].RFID_num = temp_stationRFID;
    }
}

bool MainWindow::isRFID(const QString& content)
{
    if(content.size() != 4)
    {
        QMessageBox::warning(this,tr("错误："),tr("RFID号只能4位十六进制数！"));
        return false;
    }
    QByteArray array = content.toLatin1();
    const char *s = array.data();
    bool flag_isRFID = true;
    while (*s)
    {
        if((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'E'))
        {
        }
        else
        {
            flag_isRFID = false;
            QMessageBox::warning(this,tr("错误："),tr("RFID只能由十六进制数组成！"));
            break;
        }
        s++;
    }
    return  flag_isRFID;
}

void MainWindow::slotCapacity()
{
    qDebug() << "capacity";
    //判断选中操作是否有效
    if(scene->selectedItems().size() == 0)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("必须选中一条路径段！"));
        return;
    }
    else if(scene->selectedItems().size() > 1)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("选中工路径段个数只能为1！"));
        return;
    }
    else
    {
        if(scene->selectedItems().first()->toolTip().split(",").first() != "line")
        {
            QMessageBox::warning(this,tr("Tips:"),tr("该图元不可设定容量！"));
            return;
        }
    }

    bool isOk = true;
    int select_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
    int temp_routecapacity = QInputDialog::getInt(this,tr("Tips:"),tr("输入路径容量"),1,0,99,1,&isOk);
    if(isOk != true)
    {
        return;
    }
    list_route[select_num].capacity = temp_routecapacity;
}

void MainWindow::slotDirEnable()
{
    qDebug() << "dir";
    if(scene->selectedItems().size() == 0)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("必须选中一条路径段！"));
        return;
    }
    else if(scene->selectedItems().size() > 1)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("选中工路径段个数只能为1！"));
        return;
    }
    else
    {
        if(scene->selectedItems().first()->toolTip().split(",").first() != "line")
        {
            QMessageBox::warning(this,tr("Tips:"),tr("该图元不可设定容量！"));
            return;
        }
    }

    bool isOk = true;
    int select_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
    int old_enable_dir = list_route[select_num].enable_dir;
    int temp_routeDir = QInputDialog::getInt(this,tr("Tips:"),tr("输入使能代号：0.禁止 1.正向可行 2.反向可行 3.双向可行"),1,0,3,1,&isOk);
    if(isOk != true)
    {
        return;
    }
    list_route[select_num].enable_dir = temp_routeDir;

    //判断是否已经有双箭头，若有则略过，没有则补画，方便后面显示/隐藏，也为了避免每次都添加图元
    bool double_exist = false;
    QList<QGraphicsItem *> select_child_items = scene->selectedItems().first()->childItems();
    for(int i = 0;i < select_child_items.size();i++)
    {
        if(select_child_items.at(i)->toolTip().split(",").last() == "double")
        {
            double_exist = true;
        }
    }


    //若原先没有arrow,double，则补充该箭头
    if(!double_exist)
    {
        QPointF jointPoint = list_route[select_num].start_pos+(list_route[select_num].end_pos-list_route[select_num].start_pos)/4; //靠近起点的1/4处

        QLineF arrowLeft;
        QLineF arrowRight;
        arrowLeft.setP1(jointPoint);
        arrowRight.setP1(jointPoint);
        arrowLeft.setLength(8);
        arrowRight.setLength(8);
        arrowLeft.setAngle(list_route[select_num].angle-30);
        arrowRight.setAngle(list_route[select_num].angle+30);
        QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
        QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
        arrowLeftItem->setLine(arrowLeft);
        arrowRightItem->setLine(arrowRight);
        arrowLeftItem->setPen(ArrowPen);
        arrowRightItem->setPen(ArrowPen);
        arrowLeftItem->setParentItem(scene->selectedItems().first());
        arrowRightItem->setParentItem(scene->selectedItems().first());
        arrowLeftItem->setToolTip("arrow,double");
        arrowRightItem->setToolTip("arrow,double");
    }

    //记录arrow,single;arrow,double
    QList<int> arrow_single;
    QList<int> arrow_double;
    QList<QGraphicsItem *> new_child_items = scene->selectedItems().first()->childItems();
    for(int i = 0;i < new_child_items.size();i++)
    {
        if(new_child_items.at(i)->toolTip().split(",").last() == "double")
        {
            arrow_double.append(i);
        }
        else if(new_child_items.at(i)->toolTip().split(",").last() == "single")
        {
            arrow_single.append(i);
        }
    }

    if(temp_routeDir == 0)//若现在输入0,未改变列表,使起点终点清空，其他与该路径有关的如何处置？
    {
        if(old_enable_dir == 0)//如果原先禁止，则不操作
        {
            return;
        }
        else if(old_enable_dir == 1)//若原先就是正向，隐藏arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 0;
            list_route[select_num].forbid_save.append(list_route[select_num].start_num);
            list_route[select_num].forbid_save.append(list_route[select_num].end_num);

        }
        else if(old_enable_dir == 2) //若原先是反向，隐藏arrow,double
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 0;
        }
        else //原先是双向，隐藏arrow,double和arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 0;
        }
    }
    else if(temp_routeDir == 1)//若现在输入1
    {
        if(old_enable_dir == 0)//如果原先禁止，则需显示箭头arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 1;
        }
        else if(old_enable_dir == 1)//若原先就是单向，则不操作
        {
            return;
        }
        else if(old_enable_dir == 2) //若原先是反向，隐藏arrow,double，显示arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 1;
            //对调起止点编号和新旧点位置
            int temp_start_num = list_route[select_num].start_num;
            QPointF temp_start_pos = list_route[select_num].start_pos;
            QPointF temp_old_start_pos = list_route[select_num].old_start;

            list_route[select_num].start_num = list_route[select_num].end_num;
            list_route[select_num].end_num = temp_start_num;

            list_route[select_num].start_pos = list_route[select_num].end_pos;
            list_route[select_num].end_pos = temp_start_pos;

            list_route[select_num].old_start = list_route[select_num].old_end;
            list_route[select_num].old_end = temp_old_start_pos;
        }
        else //原先是双向，隐藏arrow,double,不改变链表
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(false);
            new_child_items.at(arrow_double.last())->setVisible(false);
            list_route[select_num].enable_dir = 1;
        }
    }
    else if(temp_routeDir == 2) //若现在输入2
    {
        if(old_enable_dir == 0)//如果原先禁止，则需显示箭头arrow,double
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 2;
            //对调起止点编号和新旧点位置
            int temp_start_num = list_route[select_num].start_num;
            QPointF temp_start_pos = list_route[select_num].start_pos;
            QPointF temp_old_start_pos = list_route[select_num].old_start;

            list_route[select_num].start_num = list_route[select_num].end_num;
            list_route[select_num].end_num = temp_start_num;

            list_route[select_num].start_pos = list_route[select_num].end_pos;
            list_route[select_num].end_pos = temp_start_pos;

            list_route[select_num].old_start = list_route[select_num].old_end;
            list_route[select_num].old_end = temp_old_start_pos;

        }
        else if(old_enable_dir == 1)//若原先就是正向，隐藏arrow,single显示arrow,double
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 2;
            //对调起止点编号和新旧点位置
            int temp_start_num = list_route[select_num].start_num;
            QPointF temp_start_pos = list_route[select_num].start_pos;
            QPointF temp_old_start_pos = list_route[select_num].old_start;

            list_route[select_num].start_num = list_route[select_num].end_num;
            list_route[select_num].end_num = temp_start_num;

            list_route[select_num].start_pos = list_route[select_num].end_pos;
            list_route[select_num].end_pos = temp_start_pos;

            list_route[select_num].old_start = list_route[select_num].old_end;
            list_route[select_num].old_end = temp_old_start_pos;
        }
        else if(old_enable_dir == 2) //若原先是反向，不操作
        {
            return;
        }
        else //原先是双向，隐藏arrow,single,不修改链表
        {
            new_child_items.at(arrow_single.first())->setVisible(false);
            new_child_items.at(arrow_single.last())->setVisible(false);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 2;
        }
    }
    else //现在输入3
    {
        if(old_enable_dir == 0)//如果原先禁止，则需显示箭头arrow,double和arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 3;
        }
        else if(old_enable_dir == 1)//若原先就是正向，显示arrow,double
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 3;
        }
        else if(old_enable_dir == 2) //若原先是反向，显示arrow,single
        {
            new_child_items.at(arrow_single.first())->setVisible(true);
            new_child_items.at(arrow_single.last())->setVisible(true);
            new_child_items.at(arrow_double.first())->setVisible(true);
            new_child_items.at(arrow_double.last())->setVisible(true);
            list_route[select_num].enable_dir = 3;
        }
        else //原先是双向，不操作
        {
            return;
        }
    }



}

void MainWindow::slotNodeType()
{

    //判断选中操作是否有效
    if(scene->selectedItems().size() == 0)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("必须选中一个图元！"));
        return;
    }
    else if(scene->selectedItems().size() > 1)
    {
        QMessageBox::warning(this,tr("Tips:"),tr("选中图元个数只能为1！"));
        return;
    }
    else
    {
        QString selectType = scene->selectedItems().first()->toolTip().split(",").first();
        if(selectType != "station")
        {
            QMessageBox::warning(this,tr("Tips:"),tr("该图元不可设定类型！"));
            return;
        }
        bool isOk = true;
        int select_num = scene->selectedItems().first()->toolTip().split(",").last().toInt();
        int temp_nodeType = 0;
        temp_nodeType = QInputDialog::getInt(this,tr("Tips:"),tr("输入工位点类型代号：0.通用点 1.装卸点 2.停泊点 3.充电点"),0,0,3,1,&isOk);

        if(isOk != true)
        {
            return;
        }
        while((temp_nodeType != 1) && (temp_nodeType != 2) && (temp_nodeType != 3))
        {
            QMessageBox::warning(this,tr("Error:"),tr("输入代号不合法，请重新输入"));
            temp_nodeType = QInputDialog::getInt(this,tr("Tips:"),tr("输入工位点类型代号：1.装卸点 2.停泊点 3.充电点"));
        }
        list_stationpoint[select_num].node_type = temp_nodeType;
    }


}

void MainWindow::slotDeleteItem()
{
    delete_item();
}

void MainWindow::getLinelink()
{
    isgetLineLink = true;
    QList<QGraphicsItem*> temp_totalRoute = searchItem("line");
    for(int i = 0;i<temp_totalRoute.size();i++)
    {
        int temp_num = temp_totalRoute.at(i)->toolTip().split(",").last().toInt();
        QList<int> temp_linelink_num = searchLineChild(temp_totalRoute.at(i),"point");
        list_route[temp_num].start_num = temp_linelink_num.first();
        list_route[temp_num].end_num = temp_linelink_num.last();
    }
    isgetLineLink = false;
}

void MainWindow::getContainStation()
{
    QList<QGraphicsItem*> temp_totalRoute = searchItem("line");
    for(int i = 0;i < temp_totalRoute.size();i++)
    {        
        int route_num = temp_totalRoute.at(i)->toolTip().split(",").last().toInt();
        list_route[route_num].contain_station.clear();
        QList<int> temp_contain = searchLineChild(temp_totalRoute.at(i),"station");
        for(int j = 0;j< temp_contain.size();j++)
        {
            list_route[route_num].contain_station.append(temp_contain.at(j));
        }
    } 
//    for(int i = 0;i<temp_totalRoute.size();i++)
//    {
//        int route_num = temp_totalRoute.at(i)->toolTip().split(",").last().toInt();
//        QList<int> temp_contain = searchLineChild(temp_totalRoute.at(i),"station");
//        for(int j = 0;j< temp_contain.size();j++)
//        {
//            list_route[route_num].contain_station.append(temp_contain.at(j));
//        }
//    }
}

void MainWindow::getInOrOutNum(QString type)
{
    //在getlinelink后才执行该函数
    isgetLineLink = true;
    for(int i = 0;i<list_routepoint.size();i++)
    {
        int point_num = i;
        QVector<int> temp_line;
        for(int j = 0;j<list_route.size();j++)
        {
            //选择入口线还是出口线
            if(type == "in")
            {
                if(list_route.at(j).end_num == point_num)
                {
                    temp_line.append(j);
                }
            }
            else if(type == "out")
            {
                if(list_route.at(j).start_num == point_num)
                {
                    temp_line.append(j);
                }
            }
            else
            {
                QMessageBox::warning(this,tr("Error"),tr("输入类型无效！"));
                return;
            }

        }

        //从小到大排序
        quickSort(temp_line,0,temp_line.size()-1);
//        for(int z = 0;z < temp_line.size();z++)
//        {
//            for(int k = 0;k < temp_line.size()-1;k++)
//            {
//                if(temp_line[k]>temp_line[k+1])
//                {
//                    int temp;
//                    temp = temp_line[k];
//                    temp_line[k] = temp_line[k+1];
//                    temp_line[k+1] = temp;
//                }
//            }
//        }
        if(type == "in")
        {
            list_routepoint[point_num].line_enter.clear();
        }
        else
        {
          list_routepoint[point_num].line_exit.clear();
        }

        //将调序后的数组赋给链表
        for( int m = 0;m < temp_line.size();m++)
        {
            if(type == "in")
            {

                list_routepoint[point_num].line_enter.append(temp_line[m]);
            }
            else
            {

                list_routepoint[point_num].line_exit.append(temp_line[m]);
            }

        }
        temp_line.clear();
    }
    isgetLineLink = false;
}

//1.汇聚点 2. 分支点 3.复合点 4. 拐弯点 5.孤立点
void MainWindow::getPointType()
{

    for(int i = 0;i < list_routepoint.size();i++)
    {
        int enter_num = list_routepoint[i].line_enter.size();
        int exit_num = list_routepoint[i].line_exit.size();
        if((enter_num == 1 && exit_num == 0) ||(exit_num == 1 && enter_num == 0))
        {
            list_routepoint[i].node_type = 5;
        }
        else if(enter_num > exit_num)  //汇聚点
        {
            list_routepoint[i].node_type = 1;
        }
        else if(enter_num < exit_num)  //分支点
        {
            list_routepoint[i].node_type = 2;
        }
        else if(enter_num==2&&exit_num==2)  //复合点
        {
            list_routepoint[i].node_type = 3;
        }
        else  //拐弯点
        {
            list_routepoint[i].node_type = 4;
        }
    }
}

void MainWindow::updateList()
{
    //测试代码
    getInOrOutNum("in");
    getInOrOutNum("out");
    getPointType();
    getContainStation();
    getRealPos();
}

void MainWindow::update_route(QGraphicsItem* route_item)
{
    int item_num = route_item->toolTip().split(",").last().toInt();

    //主线主体
    QLineF LineItem;
    LineItem.setP1(list_route[item_num].start_pos);
    LineItem.setAngle(list_route[item_num].angle);
    LineItem.setLength(list_route[item_num].length*scalingFactor);
    QGraphicsLineItem *item=new QGraphicsLineItem(LineItem);
    item->setPen(RoutePen);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    item->setToolTip(tr("line,%1").arg(list_route[item_num].path_num));
    scene->addItem(item);

    //路径端点
    //设定起点属性
    QRectF StartPoint;
    StartPoint.setTopLeft(list_route[item_num].start_pos-QPointF(5,5));
    StartPoint.setBottomRight(list_route[item_num].start_pos+QPointF(5,5));
    drawRect(StartPoint,item,"point",list_route[item_num].start_num);
//    QGraphicsEllipseItem *StartPointItem = new QGraphicsEllipseItem(StartPoint);
//    StartPointItem->setBrush(PointBrush);
//    StartPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//    StartPointItem->setToolTip(tr("point,%1").arg(list_route[item_num].start_num));
//    StartPointItem->setParentItem(item);
//    StartPointItem->setVisible(flag_pointDisplay);

    drawRemark(StartPoint.topLeft(),item,"point",list_route[item_num].start_num);
//    QGraphicsSimpleTextItem *textStartItem = new QGraphicsSimpleTextItem;
//    textStartItem->setPen(RemarkPen);
//    textStartItem->setText(QString("端点%1").arg(list_route[item_num].start_num));
//    textStartItem->setParentItem(item);
//    textStartItem->setToolTip(tr("remark,%1").arg(list_route[item_num].start_num));
//    textStartItem->setPos(StartPoint.topLeft()+QPointF(-10,15));
//    textStartItem->setVisible(flag_pointRmkDisplay);

    //设定终点属性
    QRectF EndPoint;
    EndPoint.setTopLeft(list_route[item_num].end_pos-QPointF(5,5));
    EndPoint.setBottomRight(list_route[item_num].end_pos+QPointF(5,5));
    drawRect(EndPoint,item,"point",list_route[item_num].end_num);
//    QGraphicsEllipseItem *EndPointItem = new QGraphicsEllipseItem(EndPoint);
//    EndPointItem->setBrush(PointBrush);
//    EndPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//    EndPointItem->setToolTip(tr("point,%1").arg(list_route[item_num].end_num));
//    EndPointItem->setParentItem(item);
//    EndPointItem->setVisible(flag_pointDisplay);

    drawRemark(EndPoint.topLeft(),item,"point",list_route[item_num].end_num);
//    QGraphicsSimpleTextItem *textEndItem = new QGraphicsSimpleTextItem;
//    textEndItem->setPen(RemarkPen);
//    textEndItem->setText(QString("端点%1").arg(list_route[item_num].end_num));
//    textEndItem->setParentItem(item);
//    textEndItem->setToolTip(tr("remark,%1").arg(list_route[item_num].end_num));
//    textEndItem->setPos(EndPoint.topLeft()+QPointF(-10,15));
//    textEndItem->setVisible(flag_pointRmkDisplay);

    //画箭头

    QPointF  jointPoint = list_route[item_num].start_pos+(list_route[item_num].end_pos-list_route[item_num].start_pos)/2;
    drawArrow(jointPoint,list_route[item_num].angle,item);
//    QLineF arrowLeft;
//    QLineF arrowRight;
//    arrowLeft.setP1(jointPoint);
//    arrowRight.setP1(jointPoint);
//    arrowLeft.setLength(8);
//    arrowRight.setLength(8);
//    arrowLeft.setAngle(list_route[item_num].angle+180+30);
//    arrowRight.setAngle(list_route[item_num].angle+180-30);
//    QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
//    QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
//    arrowLeftItem->setLine(arrowLeft);
//    arrowRightItem->setLine(arrowRight);
//    arrowLeftItem->setPen(ArrowPen);
//    arrowRightItem->setPen(ArrowPen);
//    arrowLeftItem->setParentItem(item);
//    arrowRightItem->setParentItem(item);

    //画关系线
    QList<QGraphicsItem*> total_relation = searchItem("relation");
    QLineF relationLine;
    QList<QPointF> relationStartPoint;
    QList<QPointF> relationEndPoint;

    //删除有关关系线
    for(int a = 0;a < total_relation.size();a++)
    {
        if(total_relation.at(a)->toolTip().split(",").last().toInt() == list_route[item_num].start_num
                || total_relation.at(a)->toolTip().split(",").last().toInt() == list_route[item_num].end_num)
        {
            scene->removeItem(total_relation.at(a));
        }
    }


    //记录所有与移动线条相同编号点的坐标
    for(int j = 0 ; j < list_route.size(); j++)
    {
        if(list_route[j].start_num == list_route[item_num].start_num)
        {
            relationStartPoint.append(list_route[j].start_pos);
        }
        else if(list_route[j].end_num == list_route[item_num].start_num)
        {
            relationStartPoint.append(list_route[j].end_pos);
        }
        if(list_route[j].end_num == list_route[item_num].end_num)
        {
            relationEndPoint.append(list_route[j].end_pos);
        }
        else if(list_route[j].start_num == list_route[item_num].end_num)
        {
            relationEndPoint.append(list_route[j].start_pos);
        }

    }
    //画与起点编号相同的点
    for(int k = 0;k < relationStartPoint.size()-1;k++)
    {
        for(int m = k+1 ;m < relationStartPoint.size();m++)
        {
            drawRelationLine(relationLine,relationStartPoint.at(k),relationStartPoint.at(m),list_route[item_num].start_num);
//            relationLine.setP1(relationStartPoint.at(k));
//            relationLine.setP2(relationStartPoint.at(m));
//            QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);

//            relationLineItem->setToolTip(tr("relation,%1").arg(list_route[item_num].start_num));
//            relationLineItem->setPen(RelationPen);
//            relationLineItem->setVisible(flag_relationDisplay);
//            scene->addItem(relationLineItem);

        }
    }

    //画与终点编号相同的点
    for(int k = 0;k < relationEndPoint.size()-1;k++)
    {
        for(int m = k+1 ;m < relationEndPoint.size();m++)
        {
            relationLine.setP1(relationEndPoint.at(k));
            relationLine.setP2(relationEndPoint.at(m));
            QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);
            relationLineItem->setToolTip(tr("relation,%1").arg(list_route[item_num].end_num));
            relationLineItem->setPen(RelationPen);
            relationLineItem->setVisible(flag_relationDisplay);
            scene->addItem(relationLineItem);

        }
    }


    //画工位点
    QList<QGraphicsItem*> total_stationpoint = searchItem("station");
    QList<int> redrawNum;
    for(int n = 0 ;n < total_stationpoint.size();n++)
    {
        int stationNum = total_stationpoint.at(n)->toolTip().split(",").last().toInt();
        if(list_stationpoint[stationNum].attach_num == item_num)
        {
            redrawNum.append(stationNum);
        }
    }

    for(int p = 0 ;p < redrawNum.size();p++)
    {
        QPointF station_pos = QPointF(list_route[item_num].start_pos.x()+list_stationpoint[redrawNum.at(p)].length*scalingFactor*cos(list_route[item_num].angle/180.0*M_PI),
                                      list_route[item_num].start_pos.y()-list_stationpoint[redrawNum.at(p)].length*scalingFactor*sin(list_route[item_num].angle/180.0*M_PI));
        QRectF station_Rect;
        station_Rect.setTopLeft(station_pos+QPointF(-5,-5));
        station_Rect.setBottomRight(station_pos+QPointF(5,5));
        drawRect(station_Rect,item,"station",list_stationpoint[redrawNum.at(p)].node_num);
//        QGraphicsRectItem *stationitem = new QGraphicsRectItem(station_Rect);
//        stationitem->setBrush(StationBrush);
//        stationitem->setFlag(QGraphicsItem::ItemIsSelectable);
//        stationitem->setParentItem(item);
//        stationitem->setVisible(flag_stationDisplay);
//        stationitem->setToolTip(tr("station,%1").arg(list_stationpoint[redrawNum.at(p)].node_num));

        drawRemark(station_Rect.topLeft(),item,"station",list_stationpoint[redrawNum.at(p)].node_num);
//        QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem;
//        textItem->setPen(RemarkPen);
//        textItem->setText(QString("工位点%1").arg(list_stationpoint[redrawNum.at(p)].node_num));
//        textItem->setParentItem(item);
//        textItem->setToolTip(tr("station_remark,%1").arg(list_stationpoint[redrawNum.at(p)].node_num));
//        textItem->setPos(station_Rect.topLeft()+QPointF(-10,15));
//        textItem->setVisible(flag_stationRmkDisplay);
    }

    scene->removeItem(route_item);

}



void MainWindow::closeEvent(QCloseEvent *e)
{
    if(!flag_save)
    {
        int value = QMessageBox::question(this,tr("Tips:"),tr("文件还未保存，是否进行保存？"),QMessageBox::Yes|QMessageBox::No);
        if(value == QMessageBox::Yes)
        {
            slotSave();
        }
    }
    int ret = QMessageBox::question(this,tr("Warning:"),tr("确定要关闭吗"),QMessageBox::Yes|QMessageBox::Cancel);
    if(ret == QMessageBox::Cancel)
    {
        e->ignore();
    }

}

//template<typename T>
int MainWindow::getXmlValue(const QDomNode &node,QString type)
{
     int value = node.toElement().attributeNode(type).value().toInt();
     return value;
}

double MainWindow::getXmlValueF(const QDomNode &node,QString type)
{
    double value = node.toElement().attributeNode(type).value().toDouble();
    return value;
}

QString MainWindow::getXmlRFID(const QDomNode &node,QString type)
{
    QString value = node.toElement().attributeNode(type).value();
    return value;
}

QList<int> MainWindow::getXmlValueList(const  QDomNode &node,QString type)
{
     int count_comma = node.toElement().attributeNode(type).value().count(',');
     QList<int> value;
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

QList<double> MainWindow::getXmlPosList(const QDomNode &node,QString type)
{
    int count_comma = node.toElement().attributeNode(type).value().count(',');
    QList<double> value;
    QString content = node.toElement().attributeNode(type).value();
    if(count_comma == 0)
    {
        value.append(content.toDouble());
    }
    else
    {
        for(int i = 0;i<count_comma+1;i++)
        {
           value.append(content.split(",").at(i).toDouble());
        }
    }
    return value;
}

 bool MainWindow::eventFilter(QObject *obj,QEvent *event)
 {
     if(obj == view->viewport())
     {
         if(event->type() == QEvent::MouseButtonPress)
         {
             QMouseEvent *mousePressEvent =static_cast<QMouseEvent*>(event);
             if(mousePressEvent->button() == Qt::LeftButton)
             {
                 QPointF windowPos = mousePressEvent->pos()/new_realScaling; //相对于窗口位置
                 QPointF scenePos = view->mapFromScene(mousePressEvent->pos())/(new_realScaling*new_realScaling);//区分view和scene，场景中实际位置，向右拖动则整个场景向右移动，
                                                                                                                 //可能产生负值，向window+offset转化；本身view也已经放大1.2
                 QPointF offsetPos = windowPos-scenePos;//偏移
                 Press_Pos = windowPos+offsetPos;
                 qDebug() << "原先" << mousePressEvent->pos() << "现在" << Press_Pos << "scene" << scenePos;
                 if(LineMode3 == true)
                 {
                     count_twoPoints++;
                     switch (count_twoPoints)
                     {
                        case 1:
                              startPos = Press_Pos;
                              if(flag_isDefaultPos && !scene->items(Qt::DescendingOrder).size())
                              {
                                  reference_PosX = Press_Pos.x();
                                  reference_PosY = Press_Pos.y();
                              }
                              break;
                        case 2:
                              endPos = Press_Pos;
                              count_twoPoints = 0;
                              LineMode3 = false;
                              qDebug() << "start" << startPos << "end" << endPos;

                              //画本体
                              QLineF Line;
                              Line.setP1(startPos);
                              Line.setP2(endPos);
                              QGraphicsLineItem * lineItem = new QGraphicsLineItem(Line);
                              lineItem->setPen(RoutePen);
                              lineItem->setFlag(QGraphicsItem::ItemIsMovable);
                              lineItem->setFlag(QGraphicsItem::ItemIsSelectable);
                              lineItem->setToolTip(tr("line,%1").arg(count_route));
                              scene->addItem(lineItem);


                              Route route;
                              route.path_num = count_route;
                              double tempLength_X = endPos.x()-startPos.x();
                              double tempLength_Y = endPos.y()-startPos.y();
                              route.length = sqrt(tempLength_X*tempLength_X+tempLength_Y*tempLength_Y)/scalingFactor;
                              double Atan_value = std::abs(tempLength_Y/tempLength_X);
                              if(tempLength_X>0 && tempLength_Y<0) //第一象限
                              {
                                  route.angle = atan(Atan_value)/M_PI*180;
                              }
                              else if(tempLength_X<0 && tempLength_Y<0) //第二象限
                              {
                                  route.angle = 180-atan(Atan_value)/M_PI*180;
                              }
                              else if(tempLength_X<0 && tempLength_Y>0) //第三象限
                              {
                                  route.angle = atan(Atan_value)/M_PI*180+180;
                              }
                              else //第四象限
                              {
                                  route.angle = -atan(Atan_value)/M_PI*180;
                              }

                              route.enable_dir = 1;
                              route.capacity = 1;
                              route.start_num = count_routepoint;
                              route.end_num = count_routepoint+1;
                              route.start_pos = startPos;
                              route.end_pos = endPos;
                              list_route.append(route);
                              count_route++;

                              //路径端点
                               //设定起点属性
                              QRectF StartPoint;
                              StartPoint.setTopLeft(startPos-QPointF(5,5));
                              StartPoint.setBottomRight(startPos+QPointF(5,5));
                              drawRect(StartPoint,lineItem,"point",count_routepoint);
//                              QGraphicsEllipseItem *StartPointItem = new QGraphicsEllipseItem(StartPoint);
//                              StartPointItem->setBrush(PointBrush);
//                              StartPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//                              StartPointItem->setToolTip(tr("point,%1").arg(count_routepoint));
//                              StartPointItem->setParentItem(lineItem);
//                              StartPointItem->setVisible(flag_pointDisplay);

                              drawRemark(StartPoint.topLeft(),lineItem,"point",count_routepoint);
//                              QGraphicsSimpleTextItem *textStartItem = new QGraphicsSimpleTextItem;
//                              textStartItem->setPen(RemarkPen);
//                              textStartItem->setText(QString("端点%1").arg(count_routepoint));
//                              textStartItem->setParentItem(lineItem);
//                              textStartItem->setToolTip(tr("remark,%1").arg(count_routepoint));
//                              textStartItem->setPos(StartPoint.topLeft()+QPointF(-10,15));
//                              textStartItem->setVisible(flag_pointRmkDisplay);

                              RoutePoint routepoint_start;
                              routepoint_start.node_num = count_routepoint;
                              routepoint_start.RFID_num = "0";
                              routepoint_start.pos_x = startPos.x();
                              routepoint_start.pos_y = startPos.y();
                              count_routepoint++;
                              list_routepoint.append(routepoint_start);

                              //设定终点属性
                              QRectF EndPoint;
                              EndPoint.setTopLeft(endPos-QPointF(5,5));
                              EndPoint.setBottomRight(endPos+QPointF(5,5));
                              drawRect(EndPoint,lineItem,"point",count_routepoint);
//                              QGraphicsEllipseItem *EndPointItem = new QGraphicsEllipseItem(EndPoint);
//                              EndPointItem->setBrush(PointBrush);
//                              EndPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//                              EndPointItem->setToolTip(tr("point,%1").arg(count_routepoint));
//                              EndPointItem->setParentItem(lineItem);
//                              EndPointItem->setVisible(flag_pointDisplay);

                              drawRemark(EndPoint.topLeft(),lineItem,"point",count_routepoint);
//                              QGraphicsSimpleTextItem *textEndItem = new QGraphicsSimpleTextItem;
//                              textEndItem->setPen(RemarkPen);
//                              textEndItem->setText(QString("端点%1").arg(count_routepoint));
//                              textEndItem->setParentItem(lineItem);
//                              textEndItem->setToolTip(tr("remark,%1").arg(count_routepoint));
//                              textEndItem->setPos(EndPoint.topLeft()+QPointF(-10,15));
//                              textEndItem->setVisible(flag_pointRmkDisplay);

                              RoutePoint routepoint_end;
                              routepoint_end.node_num = count_routepoint;
                              routepoint_end.RFID_num = "0";
                              routepoint_end.pos_x = endPos.x();
                              routepoint_end.pos_y = endPos.y();
                              count_routepoint++;
                              list_routepoint.append(routepoint_end);

                              //画箭头
//                              QPen arrowpen;
//                              arrowpen.setWidth(3);

                              QPointF  jointPoint = startPos+(endPos-startPos)/2;
                              drawArrow(jointPoint,list_route[count_route-1].angle,lineItem);
//                              QLineF arrowLeft;
//                              QLineF arrowRight;
//                              arrowLeft.setP1(jointPoint);
//                              arrowRight.setP1(jointPoint);
//                              arrowLeft.setLength(8);
//                              arrowRight.setLength(8);
//                              arrowLeft.setAngle(list_route[count_route-1].angle+180+30);
//                              arrowRight.setAngle(list_route[count_route-1].angle+180-30);
//                              QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
//                              QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
//                              arrowLeftItem->setLine(arrowLeft);
//                              arrowRightItem->setLine(arrowRight);
//                              arrowLeftItem->setPen(ArrowPen);
//                              arrowRightItem->setPen(ArrowPen);
//                              arrowLeftItem->setParentItem(lineItem);
//                              arrowRightItem->setParentItem(lineItem);
                              break;
                     }

                 }
                 else if(LineMode4)
                 {
                     updateList();
                     dialog_loop = new Dialog_loop;
                     dialog_loop->exec();
                     if(dialog_loop->getFlagLoop())
                     {
                         int start_num = dialog_loop->ui->spinBox_start->value();
                         int end_num = dialog_loop->ui->spinBox_end->value();
                         if(start_num > list_routepoint.size())
                         {
                             QMessageBox::warning(this,tr("错误："),tr("输入的起点不存在！"));
                             return false;
                         }
                         else if(end_num > list_routepoint.size())
                         {
                             QMessageBox::warning(this,tr("错误："),tr("输入的终点不存在！"));
                             return false;
                         }
                         else if(start_num == end_num)
                         {
                             QMessageBox::warning(this,tr("错误："),tr("封闭环操作不能为同一个点！"));
                             return false;
                         }

                         QPointF real_start_pos = QPointF(list_routepoint[start_num].pos_x,list_routepoint[start_num].pos_y); //真实的坐标算出长度角度
                         QPointF real_end_pos = QPointF(list_routepoint[end_num].pos_x,list_routepoint[end_num].pos_y);
                         double tempLength_X = real_end_pos.x()-real_start_pos.x();
                         double tempLength_Y = real_end_pos.y()-real_start_pos.y();

                         //画本体
                         Route route;
                         route.path_num = count_route;
                         route.length = sqrt(tempLength_X*tempLength_X+tempLength_Y*tempLength_Y);
                         double Atan_value = std::abs(tempLength_Y/tempLength_X);
                         if(tempLength_X == 0 && tempLength_Y > 0)
                         {
                             route.angle = -90;

                         }
                         else if(tempLength_X == 0 && tempLength_Y < 0)
                         {
                             route.angle = 90;
                         }
                         else if(tempLength_X > 0 && tempLength_Y == 0)
                         {
                             route.angle = 0;
                         }
                         else if(tempLength_X < 0 && tempLength_Y == 0)
                         {
                             route.angle = 180;
                         }
                         else if(tempLength_X>0 && tempLength_Y<0) //第一象限
                         {
                             route.angle = atan(Atan_value)/M_PI*180;
                         }
                         else if(tempLength_X<0 && tempLength_Y<0) //第二象限
                         {
                             route.angle = 180-atan(Atan_value)/M_PI*180;
                         }
                         else if(tempLength_X<0 && tempLength_Y>0) //第三象限
                         {
                             route.angle = atan(Atan_value)/M_PI*180+180;
                         }
                         else //第四象限
                         {
                             route.angle = -atan(Atan_value)/M_PI*180;
                         }
                         qDebug()<< tempLength_X << tempLength_Y << "angle" << route.angle;

                         //计算显示的端点
                         QPointF start_pos = Press_Pos;
                         QPointF end_pos = QPointF(Press_Pos.x()+route.length*scalingFactor*cos(route.angle/180.0*M_PI),
                                                   Press_Pos.y()-route.length*scalingFactor*sin(route.angle/180.0*M_PI));
                         route.enable_dir = 1;
                         route.capacity = 1;
                         route.start_num = start_num;
                         route.end_num = end_num;
                         route.start_pos = start_pos;
                         route.end_pos = end_pos;
                         list_route.append(route);

                         QLineF Line;
                         Line.setP1(start_pos);
                         Line.setP2(end_pos);

                         QGraphicsLineItem * lineItem = new QGraphicsLineItem(Line);
                         lineItem->setPen(RoutePen);
                         lineItem->setFlag(QGraphicsItem::ItemIsMovable);
                         lineItem->setFlag(QGraphicsItem::ItemIsSelectable);
                         lineItem->setToolTip(tr("line,%1").arg(count_route));
                         scene->addItem(lineItem);
                         count_route++;

                         //路径端点
                         //设定起点属性
                         QRectF StartPoint;
                         StartPoint.setTopLeft(start_pos-QPointF(5,5));
                         StartPoint.setBottomRight(start_pos+QPointF(5,5));
                         drawRect(StartPoint,lineItem,"point",route.start_num);
//                         QGraphicsEllipseItem *StartPointItem = new QGraphicsEllipseItem(StartPoint);
//                         StartPointItem->setBrush(PointBrush);
//                         StartPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//                         StartPointItem->setToolTip(tr("point,%1").arg(route.start_num));
//                         StartPointItem->setParentItem(lineItem);
//                         StartPointItem->setVisible(flag_pointDisplay);

                         drawRemark(StartPoint.topLeft(),lineItem,"point",route.start_num);
//                         QGraphicsSimpleTextItem *textStartItem = new QGraphicsSimpleTextItem;
//                         textStartItem->setPen(RemarkPen);
//                         textStartItem->setText(QString("端点%1").arg(route.start_num));
//                         textStartItem->setParentItem(lineItem);
//                         textStartItem->setToolTip(tr("remark,%1").arg(route.start_num));
//                         textStartItem->setPos(StartPoint.topLeft()+QPointF(-10,15));
//                         textStartItem->setVisible(flag_pointRmkDisplay);


                         //设定终点属性
                         QRectF EndPoint;
                         EndPoint.setTopLeft(end_pos-QPointF(5,5));
                         EndPoint.setBottomRight(end_pos+QPointF(5,5));
                         drawRect(EndPoint,lineItem,"point",route.end_num);
//                         QGraphicsEllipseItem *EndPointItem = new QGraphicsEllipseItem(EndPoint);
//                         EndPointItem->setBrush(PointBrush);
//                         EndPointItem->setFlag(QGraphicsItem::ItemIsSelectable);
//                         EndPointItem->setToolTip(tr("point,%1").arg(route.end_num));
//                         EndPointItem->setParentItem(lineItem);
//                         EndPointItem->setVisible(flag_pointDisplay);

                         drawRemark(EndPoint.topLeft(),lineItem,"point",route.end_num);
//                         QGraphicsSimpleTextItem *textEndItem = new QGraphicsSimpleTextItem;
//                         textEndItem->setPen(RemarkPen);
//                         textEndItem->setText(QString("端点%1").arg(route.end_num));
//                         textEndItem->setParentItem(lineItem);
//                         textEndItem->setToolTip(tr("remark,%1").arg(route.end_num));
//                         textEndItem->setPos(EndPoint.topLeft()+QPointF(-10,15));
//                         textEndItem->setVisible(flag_pointRmkDisplay);


                         //画箭头

                         QPointF  jointPoint = start_pos+(end_pos-start_pos)/2;
                         drawArrow(jointPoint,list_route[count_route-1].angle,lineItem);
//                         QLineF arrowLeft;
//                         QLineF arrowRight;
//                         arrowLeft.setP1(jointPoint);
//                         arrowRight.setP1(jointPoint);
//                         arrowLeft.setLength(8);
//                         arrowRight.setLength(8);
//                         arrowLeft.setAngle(list_route[count_route-1].angle+180+30);
//                         arrowRight.setAngle(list_route[count_route-1].angle+180-30);
//                         QGraphicsLineItem *arrowLeftItem = new QGraphicsLineItem;
//                         QGraphicsLineItem *arrowRightItem = new QGraphicsLineItem;
//                         arrowLeftItem->setLine(arrowLeft);
//                         arrowRightItem->setLine(arrowRight);
//                         arrowLeftItem->setPen(ArrowPen);
//                         arrowRightItem->setPen(ArrowPen);
//                         arrowLeftItem->setParentItem(lineItem);
//                         arrowRightItem->setParentItem(lineItem);

                         //画关系线
                         QList<QGraphicsItem*> total_relation = searchItem("relation");
                         QLineF relationLine;
                         QList<QPointF> relationStartPoint;
                         QList<QPointF> relationEndPoint;

                         //删除有关关系线
                         for(int a = 0;a < total_relation.size();a++)
                         {
                             if(total_relation.at(a)->toolTip().split(",").last().toInt() == route.start_num
                                     || total_relation.at(a)->toolTip().split(",").last().toInt() == route.end_num)
                             {
                                 scene->removeItem(total_relation.at(a));
                             }
                         }



                         //记录所有与移动线条相同编号点的坐标
                         for(int j = 0 ; j < list_route.size(); j++)
                         {
                             if(list_route[j].start_num == route.start_num)
                             {
                                 relationStartPoint.append(list_route[j].start_pos);
                             }
                             else if(list_route[j].end_num == route.start_num)
                             {
                                 relationStartPoint.append(list_route[j].end_pos);
                             }
                             if(list_route[j].end_num == route.end_num)
                             {
                                 relationEndPoint.append(list_route[j].end_pos);
                             }
                             else if(list_route[j].start_num == route.end_num)
                             {
                                 relationEndPoint.append(list_route[j].start_pos);
                             }

                         }
                         //画与起点编号相同的点
                         for(int k = 0;k < relationStartPoint.size()-1;k++)
                         {
                             for(int m = k+1 ;m < relationStartPoint.size();m++)
                             {
                                 drawRelationLine(relationLine,relationStartPoint.at(k),relationStartPoint.at(m),route.start_num);
//                                 relationLine.setP1(relationStartPoint.at(k));
//                                 relationLine.setP2(relationStartPoint.at(m));
//                                 QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);
//                                 relationLineItem->setToolTip(tr("relation,%1").arg(route.start_num));
//                                 relationLineItem->setPen(RelationPen);
//                                 relationLineItem->setVisible(flag_relationDisplay);
//                                 scene->addItem(relationLineItem);

                             }
                         }

                         //画与终点编号相同的点
                         for(int k = 0;k < relationEndPoint.size()-1;k++)
                         {
                             for(int m = k+1 ;m < relationEndPoint.size();m++)
                             {
                                 relationLine.setP1(relationEndPoint.at(k));
                                 relationLine.setP2(relationEndPoint.at(m));
                                 QGraphicsLineItem *relationLineItem = new QGraphicsLineItem(relationLine);
                                 relationLineItem->setToolTip(tr("relation,%1").arg(route.end_num));
                                 relationLineItem->setPen(RelationPen);
                                 relationLineItem->setVisible(flag_relationDisplay);
                                 scene->addItem(relationLineItem);

                             }
                         }

                         LineMode4 = false;
                         delete dialog_loop;
                     }
                     else
                     {
                         LineMode4 = false;
                     }

                 }

             }
             return false;
         }
         else if(event->type() == QEvent::MouseButtonRelease)
         {
             QMouseEvent *mouseReleaseEvent =static_cast<QMouseEvent*>(event);
             Release_Pos = mouseReleaseEvent->pos()/new_realScaling;
             QPointF scenePos = view->mapFromScene(mouseReleaseEvent->pos())/(new_realScaling*new_realScaling);//区分view和scene，场景中实际位置，向右拖动则整个场景向右移动，可能产生负值，向window+offset转化
             QPointF offsetPos = Release_Pos-scenePos;//偏移
             Offset_Pos = (Release_Pos+offsetPos-Press_Pos);
             if(Offset_Pos != QPointF(0,0))
             {
                 QList<QGraphicsItem*> total_items = scene->selectedItems();
                 for(int i = 0;i < total_items.size();i++)
                 {
                     if(total_items.at(i)->toolTip().split(",").first() == "line") //可整体移动
                     {

                         int select_num = total_items.at(i)->toolTip().split(",").last().toInt();

                         list_route[select_num].start_pos += Offset_Pos;
                         list_route[select_num].end_pos += Offset_Pos;

                         update_route(total_items.at(i));
                     }
                 }

             }
             return false;
         }
         else
         {
             return false;
         }
     }
     else
     {
         return false;
     }
 }

// void MainWindow::slotRouteVisible()
// {
//     viewDisplay(flag_routeDisplay,true,"line");
// }

 void MainWindow::slotPointVisible()
 {
     viewDisplay(flag_pointDisplay,true,"point");
 }

 void MainWindow::slotStationVisible()
 {
     viewDisplay(flag_stationDisplay,true,"station");
 }

 void MainWindow::slotRelationVisible()
 {
     viewDisplay(flag_relationDisplay,true,"relation");
 }

 void MainWindow::slotStationRemarkVisible()
 {
     viewDisplay(flag_stationRmkDisplay,true,"station_remark");
 }

 void MainWindow::slotPointRemarkVisible()
 {
     viewDisplay(flag_pointRmkDisplay,true,"remark");
 }

// void MainWindow::slotRouteInvisible()
// {
//     viewDisplay(flag_routeDisplay,false,"line");
// }

 void MainWindow::slotPointInvisible()
 {
     viewDisplay(flag_pointDisplay,false,"point");
 }

 void MainWindow::slotStationInvisible()
 {
     viewDisplay(flag_stationDisplay,false,"station");
 }

 void MainWindow::slotRelationInvisible()
 {
     viewDisplay(flag_relationDisplay,false,"relation");
 }

 void MainWindow::slotPointRemarkInvisible()
 {
     viewDisplay(flag_pointRmkDisplay,false,"remark");
 }

 void MainWindow::slotStationRemarkInvisible()
 {
     viewDisplay(flag_stationRmkDisplay,false,"station_remark");
 }


 void MainWindow::viewDisplay(bool &flag_type,bool flag,QString type)
 {
     if(flag_type == flag) //状态未改变
     {
         return;
     }
     else
     {
         QList<QGraphicsItem *> total_item = searchItem(type);
         if(!flag_type && flag) //本身不显示，现需显示
         {
             for(int i = 0;i < total_item.size();i++)
             {
                 total_item.at(i)->setVisible(true);
             }
             flag_type = true;
         }
         else //本身显示，现不显示
         {
             for(int i = 0;i < total_item.size();i++)
             {
                 total_item.at(i)->setVisible(false);
             }
             flag_type = false;
         }

     }
 }

 void MainWindow::getRealPos()
 {
     list_realpoint.clear();
     existence.clear();
     //先写入第一点
     RealPoint first_realpoint;
     first_realpoint.point_num = 0;
     first_realpoint.point_posx = reference_PosX;
     first_realpoint.point_posy = reference_PosY;
     list_realpoint.append(first_realpoint);
     existence.append(0);
     startWithNum(0);


     //将list_realpoint写入list_routepoint链表
     for(int j = 0;j < list_routepoint.size();j++)
     {
         for(int k = 0;k < list_realpoint.size();k++)
         {
             if(list_realpoint.at(k).point_num == j)
             {
                 list_routepoint[j].pos_x = list_realpoint[k].point_posx;
                 list_routepoint[j].pos_y = list_realpoint[k].point_posy;
                 break;

             }
         }
     }

     bool flag_isComplementary = false;
     //补充寻找孤立点
     for(int m = 0; m < list_routepoint.size(); m++)
     {
         if(!isExistence(m))
         {
             flag_isComplementary = true;
             for(int n = 0 ; n < list_route.size(); n++) //找到该孤立点
             {
                 if(list_route[n].start_num == m)
                 {
                     double num_posx = list_routepoint[list_route[n].end_num].pos_x;
                     double num_posy = list_routepoint[list_route[n].end_num].pos_y;
                     double num_angle = list_route[n].angle - 180;
                     double temp_goalPosX = num_posx+list_route[n].length*cos(num_angle/180.0*M_PI);
                     double temp_goalPosY = num_posy-list_route[n].length*sin(num_angle/180.0*M_PI);
                     RealPoint realpoint;
                     realpoint.point_num = m;
                     realpoint.point_posx = temp_goalPosX;
                     realpoint.point_posy = temp_goalPosY;
                     list_realpoint.append(realpoint);
                     existence.append(m);

                     list_routepoint[m].pos_x = temp_goalPosX;
                     list_routepoint[m].pos_y = temp_goalPosY;

                     break;
                 }

             }
             startWithNum(m); //以孤立点为起点继续递归
         }
     }

     if(flag_isComplementary)
     {
         //若进行了补充，重新将list_realpoint写入list_routepoint链表
         for(int j = 0;j < list_routepoint.size();j++)
         {
             for(int k = 0;k < list_realpoint.size();k++)
             {
                 if(list_realpoint.at(k).point_num == j)
                 {
                     list_routepoint[j].pos_x = list_realpoint[k].point_posx;
                     list_routepoint[j].pos_y = list_realpoint[k].point_posy;
                     break;

                 }
             }
         }
     }

 }

 bool MainWindow::isExistence(int num)
 {
    bool flag_exixtence = false;
    for(int i = 0 ; i < existence.size(); i++)
    {
        if(existence.at(i) == num)
        {
            flag_exixtence = true;
        }
        else
        {
            continue;
        }
    }
    return  flag_exixtence;
 }

/***************************************************递归寻找以某点为起点的所有端点（若存在孤立点，则找不到该孤立点）**************************************************************/
void MainWindow::startWithNum(int num)
 {
     if(existence.size() == list_routepoint.size())
     {
         return;
     }
     for(int i = 0; i < list_route.size();i++)
     {
         if(list_route[i].start_num == num)
         {
             //若该终点不存在
             if(!isExistence(list_route[i].end_num))
             {
                 int index = -1;//找到该点在链表中的下标,由此获得Num坐标
                 for(int j = 0;j < list_realpoint.size();j++)
                 {
                     if(list_realpoint.at(j).point_num == num)
                     {
                         index = j;
                     }
                 }
                 double num_posx = list_realpoint[index].point_posx;
                 double num_posy = list_realpoint[index].point_posy;
                 double temp_endPosx = num_posx+list_route[i].length*cos(list_route[i].angle/180.0*M_PI);
                 double temp_endPosy = num_posy-list_route[i].length*sin(list_route[i].angle/180.0*M_PI);
                 RealPoint realpoint;
                 realpoint.point_num = list_route[i].end_num;
                 realpoint.point_posx = temp_endPosx;
                 realpoint.point_posy = temp_endPosy;
                 list_realpoint.append(realpoint);
                 existence.append(list_route[i].end_num);

                 startWithNum(list_route[i].end_num);
             }

         }
         else
         {
             continue;
         }
     }

 }

void MainWindow::isFirstReference()
{

    if(scene->items(Qt::DescendingOrder).size() == 0) //创建第一点时，才需指定基点坐标
    {
        flag_isDefaultPos = true;
        int ret = QMessageBox::question(this,tr("提示："),tr("您想要自己指定基点坐标嘛？（若不指定，则以默认方式创建基点坐标）"),QMessageBox::Yes|QMessageBox::No);
        if(ret == QMessageBox::Yes)
        {
            Dialog_firstPos *dialog_firstpos = new Dialog_firstPos;
            dialog_firstpos->ui->lineEdit_referenceX->setFocus();
            dialog_firstpos->exec();
            if(dialog_firstpos->getFlagFirstPos())
            {
                flag_isDefaultPos = false;
                reference_PosX = dialog_firstpos->getFirstPos("X");
                reference_PosY = dialog_firstpos->getFirstPos("Y");
            }
        }
        scalingFactor = QInputDialog::getDouble(this,tr("设置比例因子:"),tr("比例因子："),1.0,0.1,10.0,2);
        LabelScaling->setText(tr("比例因子：%1").arg(scalingFactor));


    }
}

/******************************************************创建Floyd算法所需参数*********************************************************************/
QList<FloydInfo> MainWindow::createFloydInfo()
{
    for(int i = 0;i < list_routepoint.size();i++)
    {
        for(int j = 0;j < list_route.size();j++)
        {
            if(list_route.at(j).start_num == i)
            {
                FloydInfo floydinfo;
                floydinfo.startNo = i;
                floydinfo.endNo = list_route.at(j).end_num;
                floydinfo.weight = list_route.at(j).length;
                list_floydinfo.append(floydinfo);
                qDebug() << floydinfo.startNo << floydinfo.endNo << floydinfo.weight;
            }
        }
    }
    return list_floydinfo;
}

/******************************************************快速排序*********************************************************************/
template<typename T>
void MainWindow::quickSort(T& a,int low,int high)
{
    if (low > high)
    {
        return;
    }
    //标记
    int i = low;
    int j = high;
    int key = a[low];

     // 完成一趟快排
     while (i < j)
     {
        // 1.从右往左找到第一个小于key的数
         while (i < j && a[j] >key)
         {
            j--;
         }
         // 2. 从左往右找到第一个大于key的数

         while (i < j && a[i] <= key)
         {
            i++;
         }
         // 3. 交换
         if (i < j) {
         int temp = a[i];
         a[i] = a[j];
         a[j] = temp;
         }
     }

     // 4.调整key的位置 (将key放到i的位置)
     a[low] = a[i];
     a[i] = key;

     // 递归过程
     // 5. 对key左边的数快排
     quickSort(a, low, i - 1);
     // 6. 对key右边的数快排
     quickSort(a, i + 1, high);

}
