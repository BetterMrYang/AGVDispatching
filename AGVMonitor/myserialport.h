#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H
#include "thread_dealserialport.h"
#include "mydatabase.h"
#include "include.h"
#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCloseEvent>
#include <QSet>
#include <QMutex>

namespace Ui {
class MySerialPort;
}

enum DataFormat {Hex,ascii};

/*!
 * @class MySerialPort
 * @brief 描述串口功能的类
 * @date 2020-5-9
 */
class MySerialPort : public QDialog
{
    Q_OBJECT

public:
    explicit MySerialPort(QWidget *parent = nullptr);
    ~MySerialPort();
//    bool writeData(QByteArray);
    void writeData(QList<QString>);
//    MyDatabase *myDatabase;

    /*! @brief 描整理Map，生成其他表需要的信息 */
    QMap<int,QList<QString>> Map_affirmTask;        /*!< 任务信息 */
    QMap<int,QList<QString>> Map_agvRunStatus;      /*!< agv运行状态 */
    QMap<int,QList<QString>> Map_agvEnterRequest;   /*!< agv进入请求 */
    QMap<int,QList<QString>> Map_otherDeviceStatus; /*!< 其他设备状态 */

//    void integrateAGVError(int AGVNo);
//    void integrateDeviceError(int DeviceNo);


private:
    Ui::MySerialPort *ui;
    void searchInfo();

    QSerialPort myCom;
    QSerialPortInfo comInfo;
    QTimer* timer;
    QTimer* comTimer;

    QSet<int> Set_agv;

    QString BUFFER;

//    QMutex mutex;




//    QString completeContent;
//    QList<QString> receiceData;
//    QList<QString> dataContent;
    QMap<int,int> tabAnddata  = {{1,18},{2,11},{3,8},{4,25},{5,14},{6,14},{7,8},{8,12},{9,9}}; //数据位+头+尾+校验,收到的为表2，4，5，8，9
//    int analyzeAccount = 0;

    Thread_dealSerialPort* dealSerialPort;

    void initConnect();

    unsigned char ByteArrayToHexArray(unsigned char);
    bool checkDataFormat(DataFormat,QByteArray);
    void closeEvent(QCloseEvent *);

    bool verifyHead(QList<QString>);
    bool verifyTail(QList<QString>);
    int verifyParity(QList<QString>);
    void analyzeData(QString); //将所有内容加入ReceiveData，并提取数据内容，不包括校验位
    QList<QString> analyseData(QList<QString>);

    QString ListToString(QList<QString>);

//    void dealComplete(const QList<QString>&);
    QList<QString> dealStickPackage(QList<QString>&);
    QVector<QList<QString>> packFrames(QList<QString>&);




private slots:
    void readyReadSlot();
    void openCom();
    void changeCom(const QString&);
    void on_Btn_open_clicked();
    void on_Btn_close_clicked();
    void slot_timerout();
    void slot_comDelay();
};

#endif // MYSERIALPORT_H
