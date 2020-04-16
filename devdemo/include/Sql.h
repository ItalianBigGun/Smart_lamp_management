#ifndef _SQL_H_
#define _SQL_H_
#include <iostream>
#include<mysql/mysql.h>
#include <boost/thread.hpp>
using namespace std;

class SQL:public MYSQL
{
public:
    SQL();
    ~SQL();
    void initMysqlInfo(
        const char* host = "127.0.0.1", 
        const char* user = "root", 
        const char* pwd = "123456", 
        const char* database = "test", 
        int port = 3306);
    void initDatabase();
    bool connect_to_Mysql();
    bool operate_Mysql_Modify(const char *Mysql_Sentence);
    void operate_Mysql_Query(const char *Mysql_Sentence);

    int operate_select_only(const char *Mysql_Sentence);
    MYSQL_RES* operate_Query_Result(const char *Mysql_Sentence);

    void disconnect_to_Mysql();

    void pingtoconnect();

private:
    const char *user;       //username
    const char *pswd;       //password
    const char *host;       //host address
    const char *database;   //database
    unsigned int port;      //server port
    boost::mutex m_mutex;       

};

#endif
