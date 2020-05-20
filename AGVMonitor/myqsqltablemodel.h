#ifndef MYQSQLTABLEMODEL_H
#define MYQSQLTABLEMODEL_H
#include <QSqlTableModel>


//该类是为了让数据表居中显示
class MyQSqlTableModel : public QSqlTableModel
{
public:
    MyQSqlTableModel(QObject *,QSqlDatabase);

     QVariant data(const QModelIndex &index, int role) const;
};

#endif // MYQSQLTABLEMODEL_H
