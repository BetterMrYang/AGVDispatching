#include "myserialport.h"
#include "ui_myserialport.h"
#include <QDebug>
#include <QTimer>
MySerialPort::MySerialPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MySerialPort)
{
    ui->setupUi(this);
    searchInfo();
    connect(ui->CBX_baud,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changeCom(const QString&)));
    connect(ui->CBX_comNo,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changeCom(const QString&)));
    QObject::connect(&myCom, SIGNAL(readyRead()), this, SLOT(readyReadSlot()));
    timer = new QTimer(this);
    timer->start(400);
    connect(timer,SIGNAL(timeout()),this,SLOT(slot_timerout()));
    this->show();

}

MySerialPort::~MySerialPort()
{
    delete ui;
}

void MySerialPort::slot_timerout()
{
    if(!myCom.isOpen())
    {
        ui->Btn_close->setEnabled(false);
        ui->Btn_open->setEnabled(true);
        ui->CBX_comNo->setEnabled(true);
        ui->CBX_baud->setEnabled(true);
    }
    else
    {
        ui->Btn_open->setEnabled(false);
        ui->Btn_close->setEnabled(true); //若还有数据要读，可能发生丢数据？
        ui->CBX_comNo->setEnabled(false);
        ui->CBX_baud->setEnabled(false);
    }
    if(ui->CBX_comNo->currentText().isEmpty()) //确保有新串口进入时更新
    {
        searchInfo();
    }
}

void MySerialPort::searchInfo()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort tempSerialPort;
        tempSerialPort.setPortName(info.portName());
        if(tempSerialPort.open(QIODevice::ReadWrite))
        {
            ui->CBX_comNo->addItem(info.portName());
        }
        tempSerialPort.close();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Serial Number: " << info.serialNumber();
        qDebug() << "System Location: " << info.systemLocation();
    }
}

void MySerialPort::openCom()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if(info.portName() == ui->CBX_comNo->currentText())
        {
           comInfo = info;
           break;
        }
    }
    myCom.setPort(comInfo);
    if(myCom.open(QIODevice::ReadWrite))
    {
        qDebug() << "myReader.open(QIODevice::ReadWrite)";
        myCom.setBaudRate(ui->CBX_baud->currentText().toInt());
        myCom.setParity(QSerialPort::NoParity);
        myCom.setDataBits(QSerialPort::Data8);
        myCom.setStopBits(QSerialPort::OneStop);
        myCom.setFlowControl(QSerialPort::NoFlowControl);
    }
}



void MySerialPort::readyReadSlot()
{
    QByteArray arr = myCom.readAll(); //一次只能读到4个字节，故需根据长度分多次接受(算上逗号共32个字母),用QString拼接后再处理
    completeContent.append(arr);
    qDebug() << "接收到" <<completeContent;

    QList<QString> bitAccount = completeContent.split(",");
    if(bitAccount.last().isEmpty())
    {
        bitAccount.removeLast();
    }
    if(bitAccount.size() == tabAnddata.value(bitAccount[2].toInt()))
    {
        qDebug() << "处理" <<completeContent;
        analyzeData(completeContent);
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
                                  QString content = ListToString(dataContent);
                                  myDatabase->update_MySQL("agvRunStatus",content,dataContent.at(1).toInt());
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

}


//bool MySerialPort::writeData(QByteArray content)
//{
////    if(checkDataFormat(Hex,content))
////    {
//        myCom.write(content);
//        qDebug() << "发送内容"<< content;
//        return true;
////    }
////    else
////    {
////        qDebug() << "发送内容失败";
////        return false;
////    }

//}

unsigned char MySerialPort::ByteArrayToHexArray(unsigned char ch)
{
    if(ch > '0' && ch < '9')
    {
        return ch;
    }
    else if(ch > 'A' && ch < 'F')
    {
        return ch-'A'+10;
    }
    else if(ch > 'a' && ch < 'f')
    {
        return ch-'a'+10;
    }
    else
    {
        return -1;
    }
}

bool MySerialPort::checkDataFormat(DataFormat format,QByteArray content)
{
    if(format == Hex)
    {
        bool flagHex = true;
        const char *s = content.data();
        while (*s)
        {
            if((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'E'))
            {

            }
            else
            {
                flagHex = false;
                qDebug() << "输入不合法";
                return flagHex;
            }

            s++;
        }
        return flagHex;
    }
    else if(format == ascii)
    {
        bool flagAscii = true;
        const char *s = content.data();
        while (*s)
        {
            if(!isascii(*s))
            {
                flagAscii = false;
                qDebug() << "输入不合法";
                return flagAscii;
            }

            s++;
        }
        return flagAscii;

    }
    else
    {
        qDebug() << "输入不合法";
        return false;
    }
}

void MySerialPort::changeCom(const QString &agr1)
{
    if(!myCom.isOpen())
    {
        return;
    }
    qDebug() << agr1 << "波特率改变";
    myCom.close();
    openCom();
}

void MySerialPort::on_Btn_open_clicked()
{
    qDebug() << "打开串口";
    openCom();
}

void MySerialPort::on_Btn_close_clicked()
{
    qDebug() << "关闭串口";
    while(myCom.readAll().size() != 0) //确保数据读完
    {
        continue;
    }
    myCom.close();
}

void MySerialPort::closeEvent(QCloseEvent *)
{
    this->hide();
}

/****************************************************************处理数据************************************************************************************/
//解析数据，不同数据之间以特殊符号间隔，比如“，”,一次最多接受8个字节，故区分接收次数
void MySerialPort::analyzeData(QString content)
{
    receiceData.clear();
    receiceData = content.split(",");
    dataContent.clear();
    for(int j = 2;j < tabAnddata.value(receiceData.at(2).toInt())-5+2;j++) //数据为链表
    {
//        qDebug() << tabAnddata.value(receiceData.at(2).toInt())-5 << "receiceData内容" << receiceData.at(j);
        dataContent.append(receiceData.at(j));
        qDebug() << "datacontent" << receiceData.at(j);
    }
    for(int i = 0;i < receiceData.size();i++)
    {
        qDebug() << "receiceData" << receiceData.at(i);
    }
}



bool MySerialPort::verifyHead(QList<QString> head)
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

bool MySerialPort::verifyTail(QList<QString> tail)
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
int MySerialPort::verifyParity(QList<QString> content)
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

//链表转字符串
QString MySerialPort::ListToString(QList<QString> info)
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

/********************************************************发送数据*************************************************************/
//表3、表7为主动查询，格式一样
void MySerialPort::writeData(QList<QString> content)
{
    int commandType = content.first().toInt();
    QString lsv_data;
    int liv_paritybit = 0;//得知数据位奇偶性,补充成寄校验
//    qDebug() << "补充前" << verifyParity(content);
    if(!verifyParity(content))
    {
        liv_paritybit = 1;
    }

//    qDebug() << "补充后" << liv_paritybit;
    switch(commandType)
    {
        case 3: case 7://0xFF,0xFE,指令，agv/设备编号，类型，奇偶校验，0xFD，0xFC
             lsv_data = QString("0xFF,0xFE,%1,%2,%3，%4,0xFD,0xFC").arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(liv_paritybit);
             break;
        case 1:
             lsv_data = QString("0xFF,0xFE,%1,%2,%3,%4,%5,%6,%7,%7,%8,%9,%10,%11,%12,%13,%14,0xFD,0xFC").arg(content.at(0)).arg(content.at(1))
                       .arg(content.at(2)).arg(content.at(3)).arg(content.at(4)).arg(content.at(5)).arg(content.at(6)).arg(content.at(7))
                       .arg(content.at(8)).arg(content.at(9)).arg(content.at(10)).arg(content.at(11)).arg(content.at(12)).arg(liv_paritybit);
             break;
        case 6:
             lsv_data = QString("0xFF,0xFE,%1,%2,%3,%4,%5,%6,%7,%7,%8,%9,%10,0xFD,0xFC").arg(content.at(0)).arg(content.at(1)).arg(content.at(2))
                       .arg(content.at(3)).arg(content.at(4)).arg(content.at(5)).arg(content.at(6)).arg(content.at(7)).arg(content.at(8)).arg(liv_paritybit);
             break;
        case 9:
             lsv_data = QString("0xFF,0xFE,%1,%2,%3,%4,%5,0xFD,0xFC").arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(content.at(3)).arg(liv_paritybit);
             break;

    }
    myCom.write(lsv_data.toLatin1());
    qDebug() << "写数据：" << lsv_data;

}

/********************************************************整合信息*************************************************************/
/*
表19：AGV编号、AGV类型、电量、运行状态、故障状态、任务状态、装卸异常、充电异常（from 'agvRunStatus'）
*/
void MySerialPort::integrateAGVError(int AGVNo)
{
    Map_agvRunStatus.insert(4,{"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"});
    QList<QString> errorInfo;
    QVector<int> index = {1,2,3,5,9,12,15,17}; //必须是顺序容器
    QList<QString> agvRunStatusInfo = Map_agvRunStatus.find(AGVNo).value();

    for(int j:index)
    {
        errorInfo.append(agvRunStatusInfo.at(j));
        qDebug() << "AGV编号、AGV类型、电量" <<agvRunStatusInfo.at(j);
    }

    myDatabase->insert_MySQL("agvError",errorInfo);

}

/*
表20：设备编号、设备类型、装卸动作号、装卸异常、充电动作号、充电异常（from 'otherDeviceStatus'）
*/
void MySerialPort::integrateDeviceError(int DeviceNo)
{
    Map_otherDeviceStatus.insert(8,{"0","1","2","3","4","5","6"});
    QList<QString> errorInfo;
    QList<QString> deviceStatusInfo = Map_otherDeviceStatus.find(DeviceNo).value();

    for(int i = 1;i < 7 ;i++)
    {
        errorInfo.append(deviceStatusInfo.at(i));
        qDebug() << "AGV编号、AGV类型、电量" <<deviceStatusInfo.at(i);
    }

//    myDatabase->insert_MySQL("otherDeviceError",errorInfo);

}


/********************************************************条件限制************************************************************/

void MySerialPort::dealComplete(const QList<QString>& data)
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
