#include "database.h"
#include <QDebug>

DataBase::DataBase(QString name)
    :databasename(name)
{
    bool ret = initDataBase();
    if(ret){
        qDebug()<<"initial database succeed!";
    }else{
        qDebug()<<"initial database failed!";
    }
}

DataBase::~DataBase()
{
}

bool DataBase::initDataBase(QString name)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    qDebug()<<"1";
    if(name==nullptr&&databasename!=nullptr){
        db.setDatabaseName("database0.db");
    }else{
        db.setDatabaseName("database0.db");
    }
    qDebug()<<"2";
    if(!db.open())
    {
        qDebug()<<"open error!"<<endl;
        return false;
    }
    //more code
    //initial database
    sqlstr = "create table if not exists device(  \
             type char(16) primary key,\
             name char(64)  \
             )";
    bool ret = this->create(sqlstr);
    if(!ret){
        return ret;
    }


    QSqlQuery m_query;

    if(m_query.exec(
                "select * from device where type='LED1' and type='LED2';"
                )&&m_query.next()){
        qDebug()<<"data have exists!";
    }else{
        //insert data to database
        sqlstr = "insert into device(type,name) \
                    values('LED1','LED1'),('LED2','LED2'),\
                    ('LED3','LED3'),('LED4','LED4')   \
                    ";
        ret = this->execute(sqlstr);
        if(!ret){
            qDebug()<<"insert data failed!";
            model = new QSqlTableModel();
            return ret;
        }else{
            qDebug()<<"insert data succeed!";
        }
    }
    model = new QSqlTableModel();
    return true;
}

bool DataBase::create(QString sqlstr)
{
    QSqlQuery m_query;
    return m_query.exec(sqlstr);
}

bool DataBase::execute(QString sqlstr)
{
    QSqlQuery m_query;
    return m_query.exec(sqlstr);
}

bool DataBase::close()
{
    db.close();
    return true;
}

QVector<datanode>* DataBase::query(QString sqlstr)
{
    QSqlQuery m_query;
    if(m_query.exec(sqlstr)){
        QVector<datanode> *datas = new QVector<datanode>;
        while(m_query.next()){
            datanode tmp;
            tmp.devicetype = m_query.value("type").toString();
            tmp.devicename = m_query.value("name").toString();
            datas->push_back(tmp);
        }
        return datas;
    }else{
        return nullptr;
    }
}
