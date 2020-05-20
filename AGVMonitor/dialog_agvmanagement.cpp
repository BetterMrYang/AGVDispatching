#include "dialog_agvmanagement.h"
#include "ui_dialog_agvmanagement.h"
#include <QDebug>
#include <QMessageBox>
Dialog_agvManagement::Dialog_agvManagement(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_agvManagement)
{
    ui->setupUi(this);
    ui->Tab_appear->setCurrentIndex(0);
    connect(ui->CBX_appearType,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(slot_appearTypeChange()));
}

Dialog_agvManagement::~Dialog_agvManagement()
{
    delete ui;
}

void Dialog_agvManagement::on_Btn_add_clicked()
{
    if(ui->LineEdit_agvNo->text().isEmpty())
    {
        QMessageBox::warning(this,"警告：","AGV编号不为空");
        return;
    }
    if(appearType == position && (ui->LineEdit_appearX->text().isEmpty() || ui->LineEdit_appearY->text().isEmpty()))
    {
        QMessageBox::warning(this,"警告：","指定坐标不能为空");
        return;
    }
    else if(appearType == RFID && ui->LineEdit_appearRFID->text().isEmpty())
    {
        QMessageBox::warning(this,"警告：","RFID不能为空");
        return;
    }
    if(ui->CBX_agvType->currentText() == "举升式")
    {
        agvType = lift;
    }
    else if(ui->CBX_agvType->currentText() == "牵引式")
    {
        agvType = pull;
    }
    else
    {
        agvType = latent;
    }
    flag_addAGV = true;
    this->close();
}

void Dialog_agvManagement::slot_appearTypeChange()
{
    if(ui->CBX_appearType->currentIndex() == 0)
    {
        ui->Tab_appear->setCurrentIndex(0);
        ui->Tab_appear->setTabEnabled(0,true);
        ui->Tab_appear->setTabEnabled(1,false);
        ui->LineEdit_appearX->setFocus();
        appearType = position;
    }
    else
    {
        ui->Tab_appear->setCurrentIndex(1);
        ui->Tab_appear->setTabEnabled(0,false);
        ui->Tab_appear->setTabEnabled(1,true);
        ui->LineEdit_appearRFID->setFocus();
        appearType = RFID;
    }
}

void Dialog_agvManagement::on_Btn_del_clicked()
{
    QString lsv_AGVNo = ui->LineEdit_agvNo->text();
    if(lsv_AGVNo.isEmpty())
    {
        QMessageBox::warning(this,tr("警告："),tr("AGV编号不能为空"));
    }
    emit deleteAGVNo(lsv_AGVNo.toInt());
    this->close();

}


void Dialog_agvManagement::on_Btn_close_clicked()
{
    this->close();
}

/************************************************************对外接口*******************************************************************/
AGVTYPE Dialog_agvManagement::getAGVType()
{
    return agvType;
}

APPEARTYPE Dialog_agvManagement::getAppearType()
{
    return appearType;
}

bool Dialog_agvManagement::getFlag_addAGV()
{
    return flag_addAGV;
}

