#ifndef DIALOG_LOOP_H
#define DIALOG_LOOP_H

#include <QDialog>

namespace Ui {
class Dialog_loop;
}

class Dialog_loop : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_loop(QWidget *parent = nullptr);
    ~Dialog_loop();
    Ui::Dialog_loop *ui;


    bool getFlagLoop();

private slots:
    void on_Btn_ok_clicked();

    void on_Btn_cancel_clicked();

private:
    bool flag_loop;

};

#endif // DIALOG_LOOP_H
