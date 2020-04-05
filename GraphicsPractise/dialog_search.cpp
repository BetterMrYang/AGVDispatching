#include "dialog_search.h"
#include "ui_dialog_search.h"
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
Dialog_search::Dialog_search(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_search)
{
    ui->setupUi(this);
    search_index = -1;

    ui->comboBox_attribute->addItem("路径编号");
    timer = new QTimer;
    timer->start(300);
    connect(timer,SIGNAL(timeout()),this,SLOT(InitCombox_attribute()));
}

Dialog_search::~Dialog_search()
{
    delete ui;

}

void Dialog_search::InitCombox_attribute()
{
    if(ui->comboBox_obj->currentIndex() == old_index)
    {
        return;
    }
    else
    {
        if(ui->comboBox_obj->currentIndex() == 0)
        {
            ui->comboBox_attribute->clear();
            ui->comboBox_attribute->addItem("路径编号");
            old_index = 0;

        }
        else if(ui->comboBox_obj->currentIndex() == 1)
        {
            ui->comboBox_attribute->clear();
            ui->comboBox_attribute->addItem("端点编号");
            ui->comboBox_attribute->addItem("端点RFID");
            old_index = 1;
        }
        else if(ui->comboBox_obj->currentIndex() == 2)
        {
            ui->comboBox_attribute->clear();
            ui->comboBox_attribute->addItem("工位点编号");
            ui->comboBox_attribute->addItem("工位点RFID");
            old_index = 2;
        }
    }

}

void Dialog_search::on_pushButton_clicked() //确定
{
    if(checkSearch())
    {
        flag_search = true;
        if(ui->comboBox_obj->currentIndex() == 0) //查路径段的编号
        {
            search_index = 0;
        }
        else if(ui->comboBox_obj->currentIndex() == 1 && ui->comboBox_attribute->currentIndex() == 0) //查路径端点编号
        {
            search_index = 1;
        }
        else if(ui->comboBox_obj->currentIndex() == 1 && ui->comboBox_attribute->currentIndex() == 1) //查路径端点RFID
        {
            search_index = 2;
        }
        else if(ui->comboBox_obj->currentIndex() == 2 && ui->comboBox_attribute->currentIndex() == 0) //查工位点编号
        {
            search_index = 3;
        }
        else if(ui->comboBox_obj->currentIndex() == 2 && ui->comboBox_attribute->currentIndex() == 1) //查工位点RFID
        {
            search_index = 4;
        }

        qDebug()<< "search_index" << search_index;
        search_content = ui->lineEdit->text();
        this->close();
    }
    else
    {
        this->close();
    }

}

void Dialog_search::on_pushButton_2_clicked() //取消
{
    this->close();
}

bool Dialog_search::checkSearch()
{
    if(ui->comboBox_attribute->currentIndex() == 0)
    {
        if(ui->lineEdit->text().toInt() < 0 || !isNumber(ui->lineEdit->text()))
        {
            QMessageBox::warning(this,tr("错误！"),tr("输入格式不正确！"));
            return false;
        }
        else
        {
            return true;
        }
    }
    if((ui->comboBox_obj->currentIndex() == 1 ||  ui->comboBox_obj->currentIndex() == 2) && (ui->comboBox_attribute->currentIndex() == 1))
    {
        if(isRFID(ui->lineEdit->text()))
        {
            return true;
        }
        else
        {
            QMessageBox::warning(this,tr("错误！"),tr("输入RFID格式不正确！"));
            return false;
        }
    }
}

bool Dialog_search::isNumber(const QString& content)
{
    QByteArray array = content.toLatin1();
    const char *s = array.data();
    bool flag_isdigit = true;
    while (*s)
    {
        if(*s >= '0' && *s <= '9')
        {
        }
        else
        {
            flag_isdigit = false;
            break;
        }
        s++;
    }
    return  flag_isdigit;
}

bool Dialog_search::isRFID(const QString& content)
{
    QByteArray array = content.toLatin1();
    const char *s = array.data();
    bool flag_isRFID = true;
    while (*s)
    {
        if((*s >= '0' && *s <= '9') || (*s >= 'A' && *s <= 'E'))
        {
        }
        else
        {
            flag_isRFID = false;
            break;
        }
        s++;
    }
    return  flag_isRFID;
}

int Dialog_search::getSearchIndex()
{
    return search_index;
}

QString Dialog_search::getSearchContent()
{
    return search_content;
}
