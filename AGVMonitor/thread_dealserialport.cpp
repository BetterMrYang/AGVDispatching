#include "thread_dealserialport.h"
#include <QByteArray>
#include <QDebug>

/*****************************************************
通信协议：Head      Command      Data     Mode          Verify     Tail
        0xFF,0XFE   1-9        根据表   十进制/十六进制   奇偶校验    尾校验
******************************************************/
extern QMutex mutex_BUFFER;

Thread_dealSerialPort::Thread_dealSerialPort(QString& buf)
    : BUFFER(buf) //列表初始化缓冲区和数据库
{
   qDebug() << "buf" << buf;
}


/*
 * 数据库创建往前提，销毁往后放，减少连接数据库次数
*/
void Thread_dealSerialPort::run()
{
   qDebug()<<"######"<<QThread::currentThreadId();


    //    mutex.lock();
    //    QByteArray arr = myCom.readAll(); //一次只能读到4个字节，故需根据长度分多次接受(算上逗号共32个字母),用QString拼接后再处理
    //    completeContent.append(arr);
        QString completeContent = BUFFER;
    //    BUFFER.clear(); /*****************************************考虑枷锁,防止一边读一边写*************************************/
        qDebug() << "接收到" <<completeContent;

        //判断是否接受了数帧数据
        QList<QString> splitContent = completeContent.split(",");
        QList<QString> bitAccount = dealStickPackage(splitContent); //处理过粘包的数据

        qDebug()  << "bitAccount:" << bitAccount;
        //此时的bitAccount没有粘包现象

        QVector<QList<QString>> dataFrames = packFrames(bitAccount); //将各个数据帧打包进vector

        //处理每一帧
        for(auto oneFrame : dataFrames)
        {
            qDebug() << "帧：" << oneFrame;
            //    if(bitAccount.last().isEmpty())
            //    {
            //        bitAccount.removeLast();
            //    }
                if(oneFrame.size() == tabAnddata.value(oneFrame[2].toInt()))
                {
                    QList<QString> dataContent = analyseData(oneFrame);
            //        completeContent.clear(); //清空接受区
                    if(!verifyHead(oneFrame))
                    {
                        qDebug() << "数据头不通过";
                        return;
                    }
                    else
                    {
                        if(!verifyTail(oneFrame))
                        {
                            qDebug() << "数据尾不通过";
                            return;
                        }
                        else
                        {
                            if(!verifyParity(oneFrame))
                            {
                               qDebug() << "奇偶性不通过";
                            }
                            else//处理数据
                            {
                                qDebug() << "操作数据表";
                                QString temp_content;
                                int tableType = oneFrame.at(2).toInt(); //表的类型


//                                qDebug() << "已有的数据库连接" << QSqlDatabase::connectionNames();
                                myDatabase = new MyDatabase;
                                myDatabase->createConnection("thread_serialport");


                                switch(tableType)//注：存入数据库需要判断是否已经存在该条记录
                                {
                                   case 2://AGV任务确认
                                     /******************************方法1：存入数据库*********************************/
            //                              myDatabase->insert_MySQL("",dataContent); //提取有用信息进行操作
                                    /*****************************方法2：存入Map***********************************/
                                          Map_affirmTask.insert(dataContent.at(1).toInt(),dataContent);
                                          break;
                                   case 4://AGV运行状态
                                    /******************************方法1：存入数据库*********************************/
                                          if(Set_agv.contains(dataContent.at(1).toInt()))
                                          {
    //                                          QString content = ListToString(dataContent);
                                              myDatabase->update_record("agvRunStatus",dataContent,dataContent.at(1).toInt());
                                          }
                                          else
                                          {
                                              myDatabase->insert_MySQL("agvRunStatus",dataContent);
                                              Set_agv.insert(dataContent.at(1).toInt());
                                          }
                                     /*****************************方法2：存入Map***********************************/
                                          Map_agvRunStatus.insert(dataContent.at(1).toInt(),dataContent); //自动替换已有的键对应的值
                                          dealComplete(dataContent);

                                          break;
                                   case 5://AGV进入请求
                                    /******************************方法1：存入数据库*********************************/
            //                              myDatabase->insert_MySQL("",dataContent); //提取有用信息进行操作
                                    /*****************************方法2：存入Map***********************************/
                                          Map_agvEnterRequest.insert(dataContent.at(1).toInt(),dataContent);
                                          break;
                                   case 8://其他设备运行状态
                                    /******************************方法1：存入数据库*********************************/
            //                              myDatabase->insert_MySQL("otherDeviceStatus",dataContent); //提取有用信息进行操作
                                    /*****************************方法2：存入Map***********************************/
                                          Map_otherDeviceStatus.insert(dataContent.at(1).toInt(),dataContent); //自动替换已有的键对应的值
                                          break;
                                   case 9://AGV位置反馈
//                                          for(int i = 0;i < dataContent.size();i++)
//                                          {
//                                              qDebug()<< "dataConetent" << dataContent.at(i);
//                                          }
                                          temp_content = QString("AGVNo = %1,PosX = %2,PosY = %3,TurningStatus = %4").arg(dataContent.at(1))
                                                         .arg(dataContent.at(2)).arg(dataContent.at(3)).arg(dataContent.last());
//                                          qDebug() << "9" << temp_content;
                                          myDatabase->update_MySQL("agvPosition",temp_content,dataContent.at(1).toInt());
//                                          myDatabase->update_record("agvPosition",dataContent,dataContent.at(1).toInt());
                                          break;
                                   default:
                                          qDebug() << "不存该在类型表";

                                }
//                                myDatabase->closeConnection("thread_serialport");
                                delete  myDatabase;
                                QSqlDatabase::removeDatabase("thread_serialport");

                            }
                        }

                    }
                }
                else
                {
                    qDebug() << "无效帧";
                }
        }

        //当所读取数据不是整数帧时，需要保留BUFFER数据
        if(bitAccount.last() != "0xFC")
        {
            int index3 = -1; //最后帧的尾巴下标
            for(int z = 0;z < dataFrames.size();z++)
            {
                index3 = completeContent.indexOf("0xFC",index3+1);
            }

            //更新BUFFER
            QMutexLocker Locker(&mutex_BUFFER);
            BUFFER = BUFFER.mid(index3+1);
        }
        else
        {
            QMutexLocker Locker(&mutex_BUFFER);
            BUFFER.clear();
        }
        qDebug() << "BUFFER" << BUFFER;

        quit();
    //    mutex.unlock();
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


QList<QString> Thread_dealSerialPort::analyseData(QList<QString> content)
{
    QList<QString> result;
//    dataContent.clear();
    for(int j = 2;j < tabAnddata.value(content.at(2).toInt())-5+2;j++) //数据为链表
    {
        result.append(content.at(j));
        qDebug() << "解析到数据：" << content.at(j);
    }
    return  result;

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

//奇偶校验,如果为奇数则为true，偶数则为false,算上校验位
int Thread_dealSerialPort::verifyParity(QList<QString> content) //content:属于内容链表 例如content={"9","1111","120","120","0","1"}
{
    int count = 0;
    for(int i = 2;i < 2+tabAnddata.value(content.at(2).toInt())-4;i++) //即分别对9,1111,120,120,0,1 中的1计数，最后得到总的1的个数
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

//链表转字符串
QString Thread_dealSerialPort::ListToString(QList<QString> info)
{
     QString content_agvNo;
     //链表向qstring处理
     for(int i = 0;i < info.size();i++)
     {
         if(i != info.size()-1)
         {
             content_agvNo.append(info.at(i));
             content_agvNo.append(",");
         }
         else
         {
              content_agvNo.append(info.at(i));
         }

     }
     return content_agvNo;
}



/*******************************************************处理粘包***********************************************************/
//目标：0xFC0xFF分解为0xFC,0xFF
QList<QString> Thread_dealSerialPort::dealStickPackage(QList<QString>& splitContent)
{
    int count = 0;
    int index1 = -1;
    QList<QString> result = splitContent;
    for(auto data : splitContent)
    {
        if(data.size() == 8) //粘包————0xFC0xFF
        {

            index1 = splitContent.indexOf(data,index1+1);
            qDebug() << "粘包：" << data << "出现处：" << index1;
            result.replace(index1+count,data.mid(0,4)); //0xFC0XFF修改为0xFC
            result.insert(index1+1+count,data.mid(4)); //添加0xFF
            count++;
        }
    }
    return result;

}


/*******************************************************打包帧***********************************************************/
//目标：将所有帧按一帧分离
QVector<QList<QString>> Thread_dealSerialPort::packFrames(QList<QString>& bitAccount)
{
    QVector<QList<QString>> result;
    int index2 = -1;
    if(bitAccount.contains("0xFF") && bitAccount.contains("0xFC"))
    {
        int frameAccount = bitAccount.count("0xFC"); //帧数
        QVector<int> indexTail;
        for(int i = 0;i < frameAccount;i++) //保存所有0xFC的下标
        {
            index2 = bitAccount.indexOf("0xFC",index2+1);
            indexTail.append(index2);
        }

        int start = 0;
        for(int j = 0;j < frameAccount;j++) //获取每帧数据
        {
            QList<QString> tmpFrame;
            for(int k = start;k <= indexTail.at(j);k++)
            {
                tmpFrame.append(bitAccount.at(k));
            }
            result.append(tmpFrame);
            start = indexTail.at(j)+1; //更新起点

        }
    }
    return result;
}

/********************************************************条件限制************************************************************/

void Thread_dealSerialPort::dealComplete(const QList<QString>& data)
{
    if(data[12].toInt() != 4) //任务未完成，不做处理
    {
        return;
    }

    if(myDatabase->select_recordNum("taskComplete") > 10) //记录达到上限，先删除最旧的后添加最新的记录
    {
        myDatabase->delete_limitRecords("taskComplete",1);
    }
     //写入已完成数据表,信息需整合
    QList<QString> content = myDatabase->select_MySQL("taskExecute",data[10].toInt());
    myDatabase->insert_MySQL("taskComplete",content);
}



