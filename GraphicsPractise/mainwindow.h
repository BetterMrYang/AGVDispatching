#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "dialog_input.h"
#include "ui_dialog_input.h"
#include "dialog_attribute.h"
#include "dialog_search.h"
#include "dialog_firstpos.h"
#include "dialog_loop.h"
#include "ui_dialog_loop.h"
#include "mygraphicsview.h"
#include "floyd.h"
#include <QMainWindow>
#include <QGraphicsScene>
#include  <QGraphicsView>
#include <QMenuBar>
#include <QGraphicsItem>
#include <QUndoStack>
#include <QMouseEvent>
#include <QWaitCondition>
#include <QKeyEvent>
#include <QTimer>
#include <QPushButton>
#include <QVector>
#include <QContextMenuEvent>
#include <QCloseEvent>
#include <QDomDocument>
#include <QDomProcessingInstruction>
#include <QGridLayout>



namespace Ui {
class MainWindow;
}

//结构体——路径端点
struct RoutePoint
{
    int node_num;
    QString RFID_num;
    double pos_x; //保存在路径段中
    double pos_y; //保存在路径段中
    QVector<int> line_enter;
    QVector<int> line_exit;
    int node_type;

};

struct Route
{
    int path_num;
    int start_num;
    int end_num;
    double angle;
    double length;
    int enable_dir;
    QList<int> contain_station;
    int capacity;
    QPointF start_pos; //显示图元坐标
    QPointF end_pos;
    QPointF old_start;
    QPointF old_end;
    QList<int> forbid_save;
};

struct StationPoint
{
    int node_num;
    QString RFID_num;
    int attach_num;
    double length;
    int node_type;
};

struct RealPoint
{
    int point_num;
    double point_posx;
    double point_posy;
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initParam();
    void createActions();
    void initStyle();
    void mousePressEvent(QMouseEvent *e);

    void contextMenuEvent(QContextMenuEvent *);
    void closeEvent(QCloseEvent *);  //检查是否保存过
    void keyPressEvent(QKeyEvent *k);
    bool eventFilter(QObject *obj,QEvent *event);
    void drawRoutePoint(Dialog_input *,QGraphicsLineItem *);
    void drawRouteDir(Dialog_input *,QGraphicsLineItem *);
    void drawRect(QRectF,QGraphicsItem*,QString,int);
    void drawRemark(QPointF,QGraphicsItem*,QString,int);
    void drawArrow(QPointF,double,QGraphicsItem*);
    void drawRelationLine(QLineF&,const QPointF&,const QPointF&,int);
    void delete_item();  //删除图元,修改对应链表
    void initToolbar();
    void getLinelink();  //补充获得路径段的起止点编号(未用)
    void getContainStation();
    void getInOrOutNum(QString); //补充获得路径端点的入/出口路径编号
    void getPointType();  //补充获得路径端点的类型
    void updateList();
    void changeTitle(QString);
    void update_route(QGraphicsItem*);
    int getXmlValue(const QDomNode &,QString);
    double getXmlValueF(const QDomNode &,QString);
    QString getXmlRFID(const QDomNode &,QString);
    void getRealPos(); //获取端点的真实位置
    bool isExistence(int); //检查是否该编号的点已经存在

    template<typename T>
    void quickSort(T&,int,int);

    void startWithNum(int);
    void isFirstReference();
    bool isRFID(const QString&);
    QList<FloydInfo> createFloydInfo();
    QList<int> getXmlValueList(const QDomNode &,QString);
    QList<double> getXmlPosList(const QDomNode &,QString);


    QList<int> searchLineChild(QGraphicsItem *,QString);
    QList<QGraphicsItem *> searchItem(QString);



public slots:
    //菜单栏功能
    void slotNew();
    void slotSave();
    void slotExtraSave();
    void slotOpen();
    void slotClose();
    void slotClear();
    void slotDelete();
    void slotSelectAll();
    void slotSearch();
    void slotAddLineItem();
    void slotAddLineItem_Press();
    void slotTwoPoints();
    void slotLoop();
    void slotAddStationItem();
    void slotHelp();
    void slotAboutus();

    //左侧工具栏功能
    void slotAction_first();
    void slotAction_merge();

    //初始化菜单栏状态
    void initStatus();
    void slotBtnTest();

    //右键菜单栏函数
    void slotAttribute();
    void slotBindRFID();
    void slotCapacity();
    void slotDirEnable();
    void slotNodeType();
    void slotDeleteItem();

    //视图显示
//    void slotRouteVisible();
    void slotPointVisible();
    void slotStationVisible();
    void slotRelationVisible();
//    void slotRouteInvisible();
    void slotPointInvisible();
    void slotStationInvisible();
    void slotRelationInvisible();
    void slotPointRemarkVisible();
    void slotPointRemarkInvisible();
    void slotStationRemarkVisible();
    void slotStationRemarkInvisible();
    void viewDisplay(bool &,bool,QString);







private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
//    QGraphicsView *view;
    MyGraphicsView *view;

    //右键菜单动作
    QAction *attribute;
    QAction *bindRFID;
    QAction *DeleteItem;
    QAction *capacity;
    QAction *nodeType;
    QAction *dirEnable;
    QMenu *ContextMenu;

    QUndoStack *undoStack;
    QPointF Press_Pos;
    QPointF Release_Pos;
    QPointF Offset_Pos;
    QTimer *timer_Initstatus;
    QPushButton *Btn_test;

    bool LineMode2;
    bool LineMode3;
    bool LineMode4;
    int count_twoPoints;
    bool flag_first;
    bool flag_save;
    bool isgetLineLink;
    QList<RoutePoint> list_routepoint;  //路径端点列表
    QList<Route> list_route;  //路径段列表
    QList<StationPoint> list_stationpoint;  //工位点列表
    QList<RealPoint> list_realpoint;
    QList<FloydInfo>list_floydinfo;    //存储有向图信息

    int count_routepoint;
    int count_route;
    int count_stationpoint;
    int count_window = 0;

    //保存指定完的第一个图元
    int firstItem_num;

    int SelectCount;
    QRectF SelectItemRect;

    Dialog_attribute *dialog_attribute;
    Dialog_search *dialog_search;
    Dialog_loop *dialog_loop;


    //鼠标点击两点画直线
    QPointF startPos;
    QPointF endPos;

    //画笔画刷线宽颜色设置
    QPen RoutePen;
    QPen ArrowPen;
    QPen RelationPen;
    QBrush PointBrush;
    QBrush StationBrush;
    QPen RemarkPen;

    //视图显示标志位
    bool flag_routeDisplay;
    bool flag_pointDisplay;
    bool flag_stationDisplay;
    bool flag_relationDisplay;
    bool flag_pointRmkDisplay;
    bool flag_stationRmkDisplay;

    QVector<int> existence;

    bool flag_isDefaultPos;

    double reference_PosX;
    double reference_PosY;

    double scalingFactor;

    QLabel *LabelScaling;

    qreal new_realScaling;
//    QPointF origin_pressPos;
    QLineF ReferLineItem;

    QGridLayout *mainLayout;

    bool isOpenfile = false;
    QString openFilename;
    bool flag_extraSave = false;

};

#endif // MAINWINDOW_H
