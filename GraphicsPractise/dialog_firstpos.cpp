#include "dialog_firstpos.h"
#include "ui_dialog_firstpos.h"
#include <QMessageBox>
#include <QDebug>
Dialog_firstPos::Dialog_firstPos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_firstPos)
{
    ui->setupUi(this);
    flag_firstPos = false;
    ui->lineEdit_referenceX->setText("0.0");
    ui->lineEdit_referenceY->setText("0.0");
}

Dialog_firstPos::~Dialog_firstPos()
{
    delete ui;
}

void Dialog_firstPos::on_Btn_clear_clicked()
{
    ui->lineEdit_referenceX->clear();
    ui->lineEdit_referenceY->clear();
}

void Dialog_firstPos::on_Btn_ok_clicked()
{
    if(ui->lineEdit_referenceX->text().isEmpty() || ui->lineEdit_referenceY->text().isEmpty())
    {
        QMessageBox::warning(this,tr("错误："),tr("基准坐标不能为空！"));
        return;
    }
    else
    {
        flag_firstPos = true;
        first_posX = ui->lineEdit_referenceX->text().toDouble();
        first_posY = ui->lineEdit_referenceY->text().toDouble();
        this->close();
    }
}

void Dialog_firstPos::on_Btn_default_clicked()
{
    this->close();
}

bool Dialog_firstPos::getFlagFirstPos()
{
    return flag_firstPos;
}

double Dialog_firstPos::getFirstPos(QString type)
{
    if(type == "X")
    {
        return first_posX;
    }
    else if(type == "Y")
    {
        return first_posY;
    }
    else
    {
        qDebug() << "无法获取位置";
        return 0;
    }
}
