#include "Sql.h"

SQL::SQL()
{
    user = new char[128];
    pswd = new char[128];
    host = new char[128];
    database = new char[128];
    initMysqlInfo();
}

SQL::~SQL()
{
    user = nullptr;
    delete user;
    pswd = nullptr;
    delete pswd;
    host = nullptr;
    delete host;
    database = nullptr;
    delete database;
}

void SQL::initMysqlInfo(
    const char* host, 
    const char* user, 
    const char* pwd, 
    const char* database, 
    int port)
{   //和自己数据库设置有关
    this->user = user;
    this->pswd = pwd;
    this->host = host;
    this->database = database;
    this->port = port;
}

void SQL::initDatabase()
{

}

bool SQL::connect_to_Mysql()
{
    //初始化数据结构
    mysql_init(this);
    //在连接数据库之前，设置额外的连接选项
    //可以设置的选项很多，这里设置字符集，否则无法处理中文
    if (0 == mysql_options(this, MYSQL_SET_CHARSET_NAME, "gbk"))
    {
        cout << "mysql_options() succeed" << endl;
    }
    else
    {
        cout << "mysql_options() failed" << endl;
    }

    if (this != nullptr && mysql_real_connect(this,host,user,pswd,database,port,nullptr,0))
    {
        cout << "Connect_to_Mysql Success" << endl;
        return true;
    }
    else
    {
        cout << "Connect_to_Mysql Failed "<<mysql_error(this)<<" from:" <<boost::this_thread::get_id()<< endl;
        return false;
    }
}

bool SQL::operate_Mysql_Modify(const char *Mysql_Sentence)
{
    int ret;
    //m_mutex.lock();
    ret = mysql_query(this, Mysql_Sentence);
    //m_mutex.unlock();

    if (ret == 0)
    {
        cout << "Operate_Mysql Modify Success" << endl;
        return true;
    }
    else
    {
        cout << "Operate_Mysql Modify Failed" << mysql_error(this) << "from:" <<boost::this_thread::get_id()<<endl;
        return false;
        //cout << mysql_error(&mysql) << endl;
    }
}

void SQL::operate_Mysql_Query(const char *Mysql_Sentence)
{
    //m_mutex.lock();
    if (0 == mysql_query(this, Mysql_Sentence))
    {
        cout << "Operate_Mysql Query Success" << endl;
        MYSQL_RES *result = nullptr;
        result = mysql_store_result(this);
        unsigned int rows = (unsigned int)mysql_num_rows(result);
        cout << "总记录条数： " << rows << endl;
        unsigned int fields = mysql_num_fields(result);
        cout << "每条记录总共 " << fields << " 个字段" << endl;
        MYSQL_FIELD *field = nullptr;
        for (unsigned int i = 0; i < fields; i++)
        {
            field = mysql_fetch_field_direct(result, i);
            cout << field->name << "\t\t";
        }
        cout << endl;
        /*
        MYSQL_ROW row = nullptr;
        row = mysql_fetch_row(result);
        while (nullptr != row)
        {
            for (unsigned int i = 0; i < fields; ++i)
            {
                if (row[i] != nullptr)
                    cout << row[i] << "\t\t";
                else
                    cout << "null" << "\t\t";
            }
            cout << endl;
            row = mysql_fetch_row(result);
        }
        */
        mysql_free_result(result);
    }
    else
    {
        cout << "Operate_Mysql Query Failed" <<mysql_error(this)<<"from:"<<boost::this_thread::get_id() <<endl;
        //cout << mysql_error(&mysql) << endl;
    }
    //m_mutex.unlock();
}

bool SQL::operate_select_only(const char *Mysql_Sentence)
{
    if (0 == mysql_query(this, Mysql_Sentence))
    {
        cout << "Operate_Mysql Query Success" << endl;
        MYSQL_RES *result = nullptr;
        result = mysql_store_result(this);
        unsigned int rows = (unsigned int)mysql_num_rows(result);
        mysql_free_result(result);
        if(rows==0){
            return false;
        }else{
            return true;
        }
    }
    else
    {
        cout << "Operate_Mysql Query Failed" <<mysql_error(this)<<"from:"<<boost::this_thread::get_id() <<endl;
        return false;
    }
    return true;
}

void SQL::disconnect_to_Mysql()
{
    mysql_close(this);
}

void SQL::pingtoconnect()
{
    if(!mysql_ping(this)){
        cout<<"ping occur failed"<<endl;
        mysql_close(this);
        connect_to_Mysql();
    }
}
