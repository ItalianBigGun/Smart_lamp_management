#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlTableModel>
typedef struct Node{
    QString devicename;
    QString devicetype;
}datanode,*datanodeptr;

class DataBase
{
public:
    DataBase(QString name=nullptr);
    ~DataBase();

    bool initDataBase(QString name = nullptr);

    bool create(QString sqlstr);
    bool execute(QString sqlstr);
    bool close();
    QVector<datanode> *query(QString sqlstr);

    QSqlTableModel *getModel(){
        return this->model;
    }

private:
     QString sqlstr = nullptr;
     QString databasename;
     QVector<datanode> *datanodes;
     QSqlTableModel *model;
     QSqlQuery m_query;
     QSqlDatabase db;
};

#endif // DATABASE_H
