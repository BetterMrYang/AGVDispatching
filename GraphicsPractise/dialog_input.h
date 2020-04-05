#ifndef DIALOG_INPUT_H
#define DIALOG_INPUT_H

#include <QDialog>
#include <QCloseEvent>
namespace Ui {
class Dialog_input;
}

class Dialog_input : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_input(QWidget *parent = nullptr);
    ~Dialog_input();
    Ui::Dialog_input *ui;



    bool getFlagCancel();
    bool getFlagOk();
    double getInputInfo(QString);

private slots:
    void on_Btn_Ok_clicked();

    void on_Btn_Cancel_clicked();

private:
    void closeEvent(QCloseEvent *);

    bool flag_cancel;  //取消标志，不触发计数
    bool flag_ok;

    double value_startx;
    double value_starty;
    double value_angle;
    double value_length;


};

#endif // DIALOG_INPUT_H
