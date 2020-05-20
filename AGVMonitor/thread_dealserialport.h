#ifndef THREAD_DEALSERIALPORT_H
#define THREAD_DEALSERIALPORT_H
#include  "mydatabase.h"
#include "include.h"
#include <QThread>
#include <qmap.h>
#include <QSet>

class Thread_dealSerialPort : public QThread
{
public:
    Thread_dealSerialPort(QString& BUFFER);

private:
    void run();

//    QString readContent;//处理内容
    QList<QString> receiceData;
    QList<QString> dataContent;

    QMap<int,int> tabAnddata  = {{1,18},{2,11},{3,8},{4,25},{5,14},{6,14},{7,8},{8,12},{9,10}}; //数据位+头+尾+校验,收到的为表2，4，5，8，9

    //缓冲区
    QString& BUFFER;

    QSet<int> Set_agv;

    //数据库
    MyDatabase* myDatabase;


    /*! @brief 描整理Map，生成其他表需要的信息 */
    QMap<int,QList<QString>> Map_affirmTask;        /*!< 任务信息 */
    QMap<int,QList<QString>> Map_agvRunStatus;      /*!< agv运行状态 */
    QMap<int,QList<QString>> Map_agvEnterRequest;   /*!< agv进入请求 */
    QMap<int,QList<QString>> Map_otherDeviceStatus; /*!< 其他设备状态 */




protected:
    bool verifyHead(QList<QString>);
    bool verifyTail(QList<QString>);
    int verifyParity(QList<QString>);
    QList<QString> analyzeData(QString);
//    QString hexTransferToD( QByteArray&);//十六进制转十进制,并以字符串形式输出
//    QString ByteArrayToQString(QByteArray);
//    char quin8ToChar(quint8);

    QList<QString> analyseData(QList<QString>);

    QString ListToString(QList<QString>);

    void dealComplete(const QList<QString>&);
    QList<QString> dealStickPackage(QList<QString>&);
    QVector<QList<QString>> packFrames(QList<QString>&);


};

#endif // THREAD_DEALSERIALPORT_H
