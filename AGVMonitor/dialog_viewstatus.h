#ifndef DIALOG_VIEWSTATUS_H
#define DIALOG_VIEWSTATUS_H

#include <QDialog>

namespace Ui {
class Dialog_ViewStatus;
}

class Dialog_ViewStatus : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ViewStatus(QWidget *parent = nullptr);
    ~Dialog_ViewStatus();
    //内容框根据主窗口填充，此处创建接口
    void setSelectNo(QString,int);

    //填充内容
    void fillInfo(QString str,QString agvNo); //对于路径：str = agvAccount; 对于交叉点和工位点：str = status,agvNo只需取一个


private slots:
    void on_Btn_closeRoute_clicked();

    void on_Btn_closeCross_clicked();

    void on_Btn_closeStation_clicked();

private:
    Ui::Dialog_ViewStatus *ui;
    QString m_Type; 
};

#endif // DIALOG_VIEWSTATUS_H
