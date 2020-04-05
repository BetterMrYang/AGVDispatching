#ifndef DIALOG_SEARCH_H
#define DIALOG_SEARCH_H

#include <QDialog>
#include <QTimer>
namespace Ui {
class Dialog_search;
}

class Dialog_search : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_search(QWidget *parent = nullptr);
    ~Dialog_search();
    bool flag_search;

    QTimer *timer;
    Ui::Dialog_search *ui;

    int getSearchIndex();
    QString getSearchContent();


private:

    bool checkSearch();
    bool isNumber(const QString&);
    bool isRFID(const QString&);

    int search_index;
    QString search_content;
    int old_index;


public slots:
    void InitCombox_attribute();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // DIALOG_SEARCH_H
