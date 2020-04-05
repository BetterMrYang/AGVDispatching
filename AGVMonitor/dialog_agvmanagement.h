#ifndef DIALOG_AGVMANAGEMENT_H
#define DIALOG_AGVMANAGEMENT_H

#include <QDialog>

namespace Ui {
class Dialog_agvManagement;
}

enum AGVTYPE{lift,pull,latent};
enum APPEARTYPE{position,RFID};

class Dialog_agvManagement : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_agvManagement(QWidget *parent = nullptr);
    ~Dialog_agvManagement();
    Ui::Dialog_agvManagement *ui;
    bool flag_addAGV = false;
    AGVTYPE agvType;
    APPEARTYPE appearType = position;
    

private slots:
    void on_Btn_add_clicked();
    void slot_appearTypeChange();
    
    void on_Btn_add_3_clicked();

private:


};

#endif // DIALOG_AGVMANAGEMENT_H
