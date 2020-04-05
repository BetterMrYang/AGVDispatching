#ifndef DIALOG_ATTRIBUTE_H
#define DIALOG_ATTRIBUTE_H

#include <QDialog>

namespace Ui {
class Dialog_attribute;
}

class Dialog_attribute : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_attribute(QWidget *parent = nullptr);
    ~Dialog_attribute();
    Ui::Dialog_attribute *ui;

    bool getFlagValid();
    bool getFlagStation();


private slots:


    void on_Btn_OK_clicked();

    void on_Btn_Cancel_clicked();

private:
    bool flag_valid;
    bool flag_station;

};

#endif // DIALOG_ATTRIBUTE_H
