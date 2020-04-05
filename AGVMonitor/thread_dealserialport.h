#ifndef THREAD_DEALSERIALPORT_H
#define THREAD_DEALSERIALPORT_H
#include  "mydatabase.h"
#include <QThread>
#include <qmap.h>


class Thread_dealSerialPort : public QThread
{
public:
    Thread_dealSerialPort(const QByteArray);

private:
    void run();
//    QString readContent;//处理内容
    QList<QString> receiceData;
    QList<QString> dataContent;

    QMap<int,int> tabAnddata  = {{1,13},{2,6},{3,3},{4,18},{5,9},{6,9},{7,3},{8,7},{9,4}};

    //数据库
    MyDatabase* myDatabase;


protected:
    bool verifyHead(QList<QString>);
    bool verifyTail(QList<QString>);
    int verifyParity(QList<QString>);
    QList<QString> analyzeData(QString);
//    QString hexTransferToD( QByteArray&);//十六进制转十进制,并以字符串形式输出
//    QString ByteArrayToQString(QByteArray);
//    char quin8ToChar(quint8);

};

#endif // THREAD_DEALSERIALPORT_H
