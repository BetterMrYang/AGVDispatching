#include "thread_dealserialport.h"
#include <QByteArray>
#include <QDebug>

/*****************************************************
通信协议：Head      Command      Data     Mode          Verify     Tail
        0xFF,0XFE   1-9        根据表   十进制/十六进制   奇偶校验    尾校验
******************************************************/


Thread_dealSerialPort::Thread_dealSerialPort(const QByteArray arr)
{
    QString readContent = arr; //收到的数据变为字符串接受
    receiceData = analyzeData(readContent);
    myDatabase = new MyDatabase();
    myDatabase->createConnection("son");
}

//解析数据，不同数据之间以特殊符号间隔，比如“，”,包含Command
QList<QString> Thread_dealSerialPort::analyzeData(QString content)
{
    QList<QString> tempStr = content.split(",");
    for(int j = 2;j < tabAnddata.value(tempStr.at(2).toInt())-5;j++) //数据为链表
    {
        dataContent.append(tempStr.at(j));
    }
    return tempStr;
}


bool Thread_dealSerialPort::verifyHead(QList<QString> head)
{
    if(head.at(0) == "0xFF" && head.at(1) == "0xFE")
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Thread_dealSerialPort::verifyTail(QList<QString> tail)
{
    int size = tail.size();
    if(tail.at(size-2) == "0xFD" && tail.at(size-1) == "0xFC")
    {
        return true;
    }
    else
    {
        return false;
    }
}

//奇偶校验,如果为奇数则为true，偶数则为false
int Thread_dealSerialPort::verifyParity(QList<QString> content)
{
    int count = 0;
    for(int i = 2;i < 2+tabAnddata.value(content.at(2).toInt())-4;i++)
    {
        unsigned int v = content[i].toInt();
        while(v)
        {
            count++;
            v = v&(v-1);
        }
        qDebug() << i << "count" << count;
    }
    if(count%2 == 1) //奇数个1
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Thread_dealSerialPort::run()
{
    qDebug() << "进入线程";

//        analyzeData(completeContent); //传递处理字符串
        if(!verifyHead(receiceData))
        {
            qDebug() << "数据头不通过";
            return;
        }
        else
        {
            if(!verifyTail(receiceData))
            {
                qDebug() << "数据尾不通过";
                return;
            }
            else
            {
                if(!verifyParity(receiceData))
                {
                   qDebug() << "奇偶性不通过";
                }
                else//处理数据
                {
                    qDebug() << "操作数据表";
                    QString temp_content;
                    int tableType = receiceData.at(2).toInt(); //表的类型
                    switch(tableType)
                    {
                       case 2://AGV任务确认
                              myDatabase->insert_MySQL("",dataContent); //提取有用信息进行操作
                              break;
                       case 4://AGV运行状态
                              myDatabase->insert_MySQL("agvRunStatus",dataContent);
                              break;
                       case 5://AGV进入请求
                              myDatabase->insert_MySQL("",dataContent); //提取有用信息进行操作
                              break;
                       case 8://其他设备运行状态
                              myDatabase->insert_MySQL("otherDeviceStatus",dataContent); //提取有用信息进行操作
                              break;
                       case 9://AGV位置反馈
                              for(int i = 0;i < dataContent.size();i++)
                              {
                                  qDebug()<< "dataConetent" << dataContent.at(i);
                              }
                              temp_content = QString("AGVNo = %1,PosX = %2,PosY = %3").arg(dataContent.at(1)).arg(dataContent.at(2)).arg(dataContent.at(3));
                              qDebug() << "9" << temp_content;
                              myDatabase->update_MySQL("agvPosition",temp_content,dataContent.at(1).toInt());
                              break;
                       default:
                              qDebug() << "不存该在类型表";

                    }

                }
            }

        }


}

////十六进制转10进制：例如FFFF->15*16^3+15*16^2+15*16+15=65535
//QString Thread_dealSerialPort::hexTransferToD(QByteArray& data)
//{
//    char revstr[16] = {0}; //根据十六进制字符串的长度，这里注意数组不要越界
//    long long num[16] = {0};
//    long long count = 1;
//    long long result = 0;

////    strcpy(revstr, data);

//    for(int i = data.size() - 1; i >= 0; i--)
//    {
//       if((revstr[i] >= '0') && (revstr[i] <= '9'))
//       {
//          num[i] = revstr[i] - 48;   //字符0的ASCII值为48
//       }
//       else if((revstr[i] >= 'a') && (revstr[i] <= 'f'))
//       {
//          num[i] = revstr[i] - 'a' + 10;
//       }
//       else if((revstr[i] >= 'A') && (revstr[i] <= 'F'))
//       {
//          num[i] = revstr[i] - 'A' + 10;
//       }
//       else
//       {
//          num[i] = 0;
//       }

//       result += num[i] * count;
//       count *= 16; //十六进制(如果是八进制就在这里乘以8)
//    }
//    QString str_result;
//    if(result!=0) //如果0x00，则输出空字符串
//    {
//        str_result = QString("%1").arg(result);

//    }
//    return str_result;
//}

//QString Thread_dealSerialPort::ByteArrayToQString(QByteArray receivedata)
//{
//    quint8 u8data,u8data_2;
//    QByteArray receivedata_midprocess;
//    QString str;
//    int j=0;

//    int hexlen=receivedata.size();
//    receivedata_midprocess.resize(hexlen*2);
//    for(int i=0;i<hexlen;i++)
//    {
//        u8data_2=receivedata[i];
//        receivedata_midprocess[j*2]=u8data_2/16;
//        receivedata_midprocess[j*2+1]=u8data_2%16;
//        j++;
//    }
//    for(int i=0;i<hexlen*2;i++)
//    {
//        u8data=receivedata_midprocess[i];
//        str.append(quin8ToChar(u8data));
//    }
//    str.remove(str.size()-1,1);
//    return str;
//}

//char Thread_dealSerialPort::quin8ToChar(quint8 b)
//{
//    if(b>=0x00 && b<=0x09)
//    {
//       return b+0x30;
//    }
//    else if(b>=10&&b<=0x15)
//    {
//       return b-0x0A+'A';
//    }
//    else return '@'; //十六进制ascii为40
//}


