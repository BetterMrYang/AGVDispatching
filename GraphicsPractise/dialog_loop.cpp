#include "dialog_loop.h"
#include "ui_dialog_loop.h"

Dialog_loop::Dialog_loop(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_loop)
{
    ui->setupUi(this);
    flag_loop = false;
}

Dialog_loop::~Dialog_loop()
{
    delete ui;
}

void Dialog_loop::on_Btn_ok_clicked()
{
    flag_loop = true;
    this->close();
}

void Dialog_loop::on_Btn_cancel_clicked()
{
    this->close();
}

bool Dialog_loop::getFlagLoop()
{
    return flag_loop;
}
