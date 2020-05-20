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

    AGVTYPE getAGVType();
    APPEARTYPE getAppearType();
    bool getFlag_addAGV();

    

private slots:
    void on_Btn_add_clicked();
    void slot_appearTypeChange();
    
    void on_Btn_close_clicked();

    void on_Btn_del_clicked();

private:
    bool flag_addAGV = false;
    AGVTYPE agvType;
    APPEARTYPE appearType = position;

signals:
    void deleteAGVNo(int);



};

#endif // DIALOG_AGVMANAGEMENT_H
