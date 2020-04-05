#ifndef DIALOG_FIRSTPOS_H
#define DIALOG_FIRSTPOS_H

#include <QDialog>

namespace Ui {
class Dialog_firstPos;
}

class Dialog_firstPos : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_firstPos(QWidget *parent = nullptr);
    ~Dialog_firstPos();
    Ui::Dialog_firstPos *ui;

    bool getFlagFirstPos();

    double getFirstPos(QString);




private slots:
    void on_Btn_clear_clicked();

    void on_Btn_ok_clicked();

    void on_Btn_default_clicked();

private:
    bool flag_firstPos;
    double first_posX;
    double first_posY;

};

#endif // DIALOG_FIRSTPOS_H
