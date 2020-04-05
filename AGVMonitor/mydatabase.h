#ifndef MYDATABASE_H
#define MYDATABASE_H
#include <QSql>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlDatabase>

class MyDatabase : public QSqlDatabase
{
public:
    MyDatabase();
    QSqlDatabase dbconn;

    void createDefaultConnection();
    void createConnection(QString name);
    void closeConnection();
    void insert_MySQL(QString,QList<QString>);//数据表，内容
    void delete_MySQL(QString,int);//数据表，主键
    void update_MySQL(QString,QString,int); //数据表,内容,主键
    QList<QString> select_MySQL(QString,int);//数据表，主键
    int select_recordNum(QString);//某个表记录数
    void delete_limitRecords(QString,int);


private:
   int tabNameTotableNo(QString);





protected:

    //设置数据库参数
    QString hostName;
    QString dbName;
    QString userName;
    QString password;
    QString connectName;

};

#endif // MYDATABASE_H
