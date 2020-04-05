#include "dialog_viewstatus.h"
#include "ui_dialog_viewstatus.h"

#include <QDebug>
Dialog_ViewStatus::Dialog_ViewStatus(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ViewStatus)
{
    ui->setupUi(this);
}

Dialog_ViewStatus::~Dialog_ViewStatus()
{
    delete ui;
}

void Dialog_ViewStatus::on_Btn_closeRoute_clicked()
{
    this->close();
}

void Dialog_ViewStatus::on_Btn_closeCross_clicked()
{
    this->close();
}

void Dialog_ViewStatus::on_Btn_closeStation_clicked()
{
    this->close();
}

void Dialog_ViewStatus::setSelectNo(QString type,int content)
{
    m_Type = type;
    if(type == "line")
    {
        ui->LineEdit_routeNo->setText(QString("%1").arg(content));
        ui->Tab_view->setCurrentIndex(0);
        ui->Tab_view->setTabEnabled(1,false);
        ui->Tab_view->setTabEnabled(2,false);
    }
    else if(type == "station")
    {
        ui->LineEdit_stationNo->setText(QString("%1").arg(content));
        ui->Tab_view->setCurrentIndex(2);
        ui->Tab_view->setTabEnabled(0,false);
        ui->Tab_view->setTabEnabled(1,false);
    }
    else if(type == "point")
    {
        ui->LineEdit_crossNo->setText(QString("%1").arg(content));
        ui->Tab_view->setCurrentIndex(1);
        ui->Tab_view->setTabEnabled(0,false);
        ui->Tab_view->setTabEnabled(2,false);
    }
    else
    {
        qDebug() << "该类型不存在！";
    }
}


/***************************************************填充信息*************************************************************/
/*注：//对于路径：str = agvAccount; 对于交叉点和工位点：str = status,agvNo只需取一个*/
void Dialog_ViewStatus::fillInfo(QString str,QString agvNo)
{
    if(m_Type == "line")
    {
        ui->LineEdit_agvAccount->setText(str);
//        QString content_agvNo;
//        //链表向qstring处理
//        for(int i = 0;i < agvNo.size();i++)
//        {
//            if(i != agvNo.size()-1)
//            {
//                content_agvNo.append(agvNo.at(i));
//                content_agvNo.append(",");
//            }
//            else
//            {
//                 content_agvNo.append(agvNo.at(i));
//            }

//        }
        ui->LineEdit_agvNo->setText(agvNo);
    }
    else if(m_Type == "station")
    {
        if(str.toInt() == 0)
        {
            ui->LineEdit_stationStatus->setText("空闲");
        }
        else
        {
            ui->LineEdit_stationStatus->setText("占用");
        }

        ui->LineEdit_occupyStation->setText(agvNo);
    }
    else if(m_Type == "point")
    {
        if(str.toInt() == 0)
        {
            ui->LineEdit_crossStatus->setText("空闲");
        }
        else
        {
             ui->LineEdit_crossStatus->setText("占用");
        }

        ui->LineEdit_occupyCross->setText(agvNo);
    }
    else
    {
      qDebug() << "该类型不存在！";
    }
}
