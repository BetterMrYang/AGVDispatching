#include "dialog_input.h"
#include "ui_dialog_input.h"
#include <QCloseEvent>
#include <QDebug>
Dialog_input::Dialog_input(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_input)
{
    ui->setupUi(this);
     value_startx = 0.0;
     value_starty = 0.0;
     value_angle = 0.0;
     value_length = 0.0;
     flag_cancel = false;
     flag_ok = false;
}

Dialog_input::~Dialog_input()
{
    delete ui;
}

void Dialog_input::on_Btn_Ok_clicked()
{
    flag_ok = true;
    value_startx = ui->lineEdit_StartX->text().toDouble();
    value_starty = ui->lineEdit_StartY->text().toDouble();
    value_angle = ui->SpinBox_Angle->value();
    value_length = ui->SpinBox_Length->value();
    this->close();
}

void Dialog_input::on_Btn_Cancel_clicked()
{
    flag_cancel=true;
    ui->SpinBox_Angle->clear();
    ui->SpinBox_Length->clear();
    ui->lineEdit_StartX->clear();
    ui->lineEdit_StartY->clear();
    this->close();
    deleteLater();
}

void Dialog_input::closeEvent(QCloseEvent *)
{
    if(flag_ok)
    {
       this->close();
        return;
    }
    flag_cancel=true;
    this->close();
}

bool Dialog_input::getFlagCancel()
{
    return flag_cancel;
}

bool Dialog_input::getFlagOk()
{
    return flag_ok;
}

double Dialog_input::getInputInfo(QString type)
{
    if(type == "X")
    {
        return value_startx;
    }
    else if(type == "Y")
    {
        return value_starty;
    }
    else if(type == "Angle")
    {
        return value_angle;
    }
    else if(type == "Length")
    {
        return value_length;
    }
    else
    {
        qDebug() << "找不到参数";
        return 0;
    }
}
