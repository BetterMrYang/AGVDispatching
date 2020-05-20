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
    void closeConnection(QString);
    void insert_MySQL(QString,QList<QString>);//数据表，内容
    void delete_MySQL(QString,int);//数据表，主键
    void update_MySQL(QString,QString,int); //数据表,内容,主键————更新部分字段
    void update_record(QString,QList<QString>&,int); //数据表,内容,主键————更新一条记录
    QList<QString> select_MySQL(QString,int);//数据表，主键
    int select_recordNum(QString);//某个表记录数
    void delete_limitRecords(QString,int);


private:
   int tabNameTotableNo(QString);





protected:

    //设置数据库参数
   const QString hostName = "127.0.0.1";   // 主机名
   const QString dbName = "agvdispatching";   // 数据库名称
   const QString userName = "Nuaa";   // 用户名
   const QString password = "17504";   // 密码
    QString connectName;

};

#endif // MYDATABASE_H
