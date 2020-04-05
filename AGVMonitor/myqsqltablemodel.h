#ifndef MYQSQLTABLEMODEL_H
#define MYQSQLTABLEMODEL_H
#include <QSqlTableModel>

class MyQSqlTableModel : public QSqlTableModel
{
public:
    MyQSqlTableModel(QObject *,QSqlDatabase);

     QVariant data(const QModelIndex &index, int role) const;
};

#endif // MYQSQLTABLEMODEL_H
