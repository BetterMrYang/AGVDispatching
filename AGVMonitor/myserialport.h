#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H
//#include "thread_dealserialport.h"
#include "mydatabase.h"
#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCloseEvent>
#include <QSet>
namespace Ui {
class MySerialPort;
}

enum DataFormat {Hex,ascii};

class MySerialPort : public QDialog
{
    Q_OBJECT

public:
    explicit MySerialPort(QWidget *parent = nullptr);
    ~MySerialPort();
//    bool writeData(QByteArray);
    void writeData(QList<QString>);
    MyDatabase* myDatabase;

    //整理Map，生成其他表需要的信息
    QMap<int,QList<QString>> Map_affirmTask;
    QMap<int,QList<QString>> Map_agvRunStatus;
    QMap<int,QList<QString>> Map_agvEnterRequest;
    QMap<int,QList<QString>> Map_otherDeviceStatus;

    void integrateAGVError(int AGVNo);
    void integrateDeviceError(int DeviceNo);


private:
    Ui::MySerialPort *ui;
    void searchInfo();

    QSerialPort myCom;
    QSerialPortInfo comInfo;
    QTimer* timer;

    QSet<int> Set_agv;




    QString completeContent;
    QList<QString> receiceData;
    QList<QString> dataContent;
    QMap<int,int> tabAnddata  = {{1,18},{2,11},{3,8},{4,23},{5,14},{6,14},{7,8},{8,12},{9,9}}; //数据位+头+尾+校验,收到的为表2，4，5，8，9
//    int analyzeAccount = 0;

//    Thread_dealSerialPort* dealSerialPort;
    unsigned char ByteArrayToHexArray(unsigned char);
    bool checkDataFormat(DataFormat,QByteArray);
    void closeEvent(QCloseEvent *);

    bool verifyHead(QList<QString>);
    bool verifyTail(QList<QString>);
    int verifyParity(QList<QString>);
    void analyzeData(QString); //将所有内容加入ReceiveData，并提取数据内容，不包括校验位

    QString ListToString(QList<QString>);

    void dealComplete(const QList<QString>&);




private slots:
    void readyReadSlot();
    void openCom();
    void changeCom(const QString&);
    void on_Btn_open_clicked();
    void on_Btn_close_clicked();
    void slot_timerout();
};

#endif // MYSERIALPORT_H
