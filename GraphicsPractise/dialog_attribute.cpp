#include "dialog_attribute.h"
#include "ui_dialog_attribute.h"

Dialog_attribute::Dialog_attribute(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_attribute)
{
    ui->setupUi(this);
    setWindowTitle("查看属性");
    ui->routeAngle->setReadOnly(false);
    ui->routeCapacity->setReadOnly(true);
    ui->routeContain->setReadOnly(true);
    ui->routeDir->setReadOnly(true);
    ui->routeEnd->setReadOnly(true);
    ui->routeLength->setReadOnly(false);
    ui->routeNum->setReadOnly(true);
    ui->routeStart->setReadOnly(true);
    ui->routeType->setReadOnly(true);

    ui->pointClass->setReadOnly(true);
    ui->pointIn->setReadOnly(true);
    ui->pointNum->setReadOnly(true);
    ui->pointOut->setReadOnly(true);
    ui->pointRFID->setReadOnly(true);
    ui->pointType->setReadOnly(true);
    ui->pointXpos->setReadOnly(true);
    ui->pointYpos->setReadOnly(true);

    ui->stationAttach->setReadOnly(true);
    ui->stationClass->setReadOnly(true);
    ui->stationLength->setReadOnly(false);
    ui->stationNum->setReadOnly(true);
    ui->stationRFID->setReadOnly(true);
    ui->stationType->setReadOnly(true);

    flag_valid = false;
    flag_station = false;
}

Dialog_attribute::~Dialog_attribute()
{
    delete ui;
}

void Dialog_attribute::on_Btn_OK_clicked()
{
    if(ui->routeLength->isModified() || ui->routeAngle->isModified()) //路径属性修改
    {
        flag_valid = true;        
        this->close();
    }

    if(ui->stationLength->isModified()) //工位点属性修改
    {
        flag_station = true;
        this->close();
    }
}

void Dialog_attribute::on_Btn_Cancel_clicked()
{
    this->close();
}

bool Dialog_attribute::getFlagValid()
{
    return flag_valid;
}

bool Dialog_attribute::getFlagStation()
{
    return flag_valid;
}
