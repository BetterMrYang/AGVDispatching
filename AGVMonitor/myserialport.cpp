#include "myserialport.h"
#include "ui_myserialport.h"
#include <QDebug>
#include <QTimer>
#include <synchapi.h>
#include <QMessageBox>
QMutex mutex_BUFFER;

MySerialPort::MySerialPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MySerialPort)
{
    ui->setupUi(this);
    searchInfo();

    comTimer = new QTimer(this);
    timer = new QTimer(this);
    timer->start(400);
    initConnect();
//    myDatabase = new MyDatabase();
//    myDatabase->createConnection("thread_serialport");
    this->show();

}

MySerialPort::~MySerialPort()
{
    delete ui;
}

void MySerialPort::initConnect()
{
    connect(ui->CBX_baud,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changeCom(const QString&)));
    connect(ui->CBX_comNo,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changeCom(const QString&)));
    QObject::connect(&myCom, SIGNAL(readyRead()), this, SLOT(slot_comDelay()));
    QObject::connect(comTimer, SIGNAL(timeout()), this, SLOT(readyReadSlot()));
    connect(timer,SIGNAL(timeout()),this,SLOT(slot_timerout()));
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

void MySerialPort::slot_comDelay()
{
    QMutexLocker Locker(&mutex_BUFFER);
    comTimer->start(200); //不够再500ms
    BUFFER.append(myCom.readAll());
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
    else
    {
        QMessageBox::critical(this,tr("错误："),tr("无法打开该串口"));
    }
}



void MySerialPort::readyReadSlot()
{
//    mutex.lock();
    comTimer->stop();
    dealSerialPort = new Thread_dealSerialPort(BUFFER); //线程处理数据

    dealSerialPort->start();

    dealSerialPort->wait();
    delete  dealSerialPort;

    qDebug() << "返回BUFFER" << BUFFER;

//    mutex.unlock();
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
//void MySerialPort::analyzeData(QString content)
//{
//    receiceData.clear();
//    receiceData = content.split(",");
//    dataContent.clear();
//    for(int j = 2;j < tabAnddata.value(receiceData.at(2).toInt())-5+2;j++) //数据为链表
//    {
////        qDebug() << tabAnddata.value(receiceData.at(2).toInt())-5 << "receiceData内容" << receiceData.at(j);
//        dataContent.append(receiceData.at(j));
//        qDebug() << "datacontent" << receiceData.at(j);
//    }
//    for(int i = 0;i < receiceData.size();i++)
//    {
//        qDebug() << "receiceData" << receiceData.at(i);
//    }
//}

QList<QString> MySerialPort::analyseData(QList<QString> content)
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
    QList<QString> test = content; //将content补充完整后奇偶校验
    int commandType = content.first().toInt();
    QString lsv_data;

    test.prepend("0xFE");
    test.prepend("0xFF");
    test.append("0"); //先补一个校验位
    test.append("0xFD");
    test.append("0xFC");
    int liv_paritybit = 0;//得知数据位奇偶性,补充成寄校验
//    qDebug() << "补充前" << verifyParity(content);
    if(!verifyParity(test))
    {
        liv_paritybit = 1;
    }

    qDebug() << "补充后" << liv_paritybit;
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
             lsv_data = QString("0xFF,0xFE,%1,%2,%3,%4,%5,%6,0xFD,0xFC").arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(content.at(3)).arg(content.at(4)).arg(liv_paritybit);
             break;
        case 4:
             lsv_data = QString("0xFF,0xFE,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,0xFD,0xFC")
                  .arg(content.at(0)).arg(content.at(1)).arg(content.at(2)).arg(content.at(3)).arg(content.at(4)).arg(content.at(5))
                  .arg(content.at(6)).arg(content.at(7)).arg(content.at(8)).arg(content.at(9)).arg(content.at(10)).arg(content.at(11))
                  .arg(content.at(12)).arg(content.at(13)).arg(content.at(14)).arg(content.at(15)).arg(content.at(16)).arg(content.at(17))
                  .arg(content.at(18)).arg(content.at(19)).arg(liv_paritybit);
             break;

    }
    myCom.write(lsv_data.toLatin1());
    qDebug() << "写数据：" << lsv_data;

}

/********************************************************整合信息*************************************************************/
/*
表19：AGV编号、AGV类型、电量、运行状态、故障状态、任务状态、装卸异常、充电异常（from 'agvRunStatus'）
*/
//void MySerialPort::integrateAGVError(int AGVNo)
//{
//    Map_agvRunStatus.insert(4,{"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"});
//    QList<QString> errorInfo;
//    QVector<int> index = {1,2,3,5,9,12,15,17}; //必须是顺序容器
//    QList<QString> agvRunStatusInfo = Map_agvRunStatus.find(AGVNo).value();

//    for(int j:index)
//    {
//        errorInfo.append(agvRunStatusInfo.at(j));
//        qDebug() << "AGV编号、AGV类型、电量" <<agvRunStatusInfo.at(j);
//    }

//    myDatabase->insert_MySQL("agvError",errorInfo);

//}

/*
表20：设备编号、设备类型、装卸动作号、装卸异常、充电动作号、充电异常（from 'otherDeviceStatus'）
*/
//void MySerialPort::integrateDeviceError(int DeviceNo)
//{
//    Map_otherDeviceStatus.insert(8,{"0","1","2","3","4","5","6"});
//    QList<QString> errorInfo;
//    QList<QString> deviceStatusInfo = Map_otherDeviceStatus.find(DeviceNo).value();

//    for(int i = 1;i < 7 ;i++)
//    {
//        errorInfo.append(deviceStatusInfo.at(i));
//        qDebug() << "AGV编号、AGV类型、电量" <<deviceStatusInfo.at(i);
//    }

////    myDatabase->insert_MySQL("otherDeviceError",errorInfo);

//}


/********************************************************条件限制************************************************************/

//void MySerialPort::dealComplete(const QList<QString>& data)
//{
//    if(data[12].toInt() != 4) //任务未完成，不做处理
//    {
//        return;
//    }

//    if(myDatabase->select_recordNum("taskComplete") > 10) //记录达到上限，先删除最旧的后添加最新的记录
//    {
//        myDatabase->delete_limitRecords("taskComplete",1);
//    }
//     //写入已完成数据表,信息需整合
//    QList<QString> content = myDatabase->select_MySQL("taskExecute",data[10].toInt());
//    myDatabase->insert_MySQL("taskComplete",content);
//}

/*******************************************************处理粘包***********************************************************/
//目标：0xFC0xFF分解为0xFC,0xFF
QList<QString> MySerialPort::dealStickPackage(QList<QString>& splitContent)
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
QVector<QList<QString>> MySerialPort::packFrames(QList<QString>& bitAccount)
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


