#include "Sql.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>
int flag = 1;
void func(const char* sqlstr)
{
    SQL Mysql;

    while(flag);
    if (Mysql.connect_to_Mysql())
    {
	cout <<"thread:"<<boost::this_thread::get_id()<<":" <<sqlstr<< endl;
	cout<<"strncmp:"<<strncmp(sqlstr,"s",1)<<endl;
        if(strncmp(sqlstr,"s",1)==0||strncmp(sqlstr,"S",1)==0)
	{
		cout<<"select"<<endl;
            Mysql.operate_Mysql_Query(sqlstr);
	}
        else
	{
		cout<<"nonselect"<<endl;
            Mysql.operate_Mysql_Modify(sqlstr);
	}
    }
    else
    {
        cout << "Error...Failed to connect to Mysql ! from:" << boost::this_thread::get_id()<< endl;
        return ;
    }
    Mysql.disconnect_to_Mysql();
}

int main()
{
    SQL Mysql;

    boost::thread_group thread_g;

    cout << "初始化数据库" << endl;//初始化数据库
    if (0 == mysql_library_init(0, NULL, NULL))
        cout << "mysql_library_init() succeed" << endl;
    else
        cout << "mysql_library_init() failed" << endl;

    cout << "连接数据库" << endl;
    if (Mysql.connect_to_Mysql())
    {
        cout << "新建表" << endl;  //新建表
        Mysql.operate_Mysql_Modify("create table if not exists test_table(id int(4), name varchar(20) character set gb2312 collate gb2312_chinese_ci)");

        //创建线程
        //插入数据
        thread_g.create_thread(boost::bind(&func,"insert into test_table values(1, 'aaa'), (2, 'bbb'), (3, 'ccc')"));
        thread_g.create_thread(boost::bind(&func,"insert into test_table(id) values(11), (22), (33)"));
        //查询数据
        thread_g.create_thread(boost::bind(&func,"select * from test_table where id = 22"));
        thread_g.create_thread(boost::bind(&func,"select * from test_table"));
    }
    else
    {
        cout << "Error...Failed to connect to Mysql ! from:" << boost::this_thread::get_id()<< endl;
    }

    /*
    cout << "连接数据库" << endl;
    if (Mysql.Connect_to_Mysql(Mysql.mysql)&&_Mysql.Connect_to_Mysql(_Mysql.mysql)&&__Mysql.Connect_to_Mysql(__Mysql.mysql)
                                                                                &&___Mysql.Connect_to_Mysql(___Mysql.mysql))
    {
        cout << "新建表" << endl;  //新建表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "create table if not exists\
                                                test_table(id int(4), name varchar(20) character set gb2312 collate gb2312_chinese_ci)");

        //添加数据子线程创建
        thread_g.create_thread([&](){
            while(flag);
                        
            cout << "添加记录:insert into test_table\
                                                values(1, 'aaa'), (2, 'bbb'), (3, 'ccc')" << endl;  //添加记录

            Mysql.Operate_Mysql_Modify(Mysql.mysql, "insert into test_table\
                                                values(1, 'aaa'), (2, 'bbb'), (3, 'ccc')");
            
        });
        
        thread_g.create_thread([&](){
            while(flag);
            
            cout << "添加记录:insert into test_table(id)\
                                                values(11), (22), (33)" << endl;  //添加记录
 
            _Mysql.Operate_Mysql_Modify(_Mysql.mysql, "insert into test_table(id)\
                                                values(11), (22), (33)");
                                                
        });
        

        //查找记录子线程创建
        thread_g.create_thread([&](){
            while(flag);
            
            cout << "查找记录:select * from test_table\
                                                where id = 22" << endl;  //查找记录
            __Mysql.Operate_Mysql_Query(__Mysql.mysql, "select * from test_table\
                                                where id = 22");
                                                
        });
        
        
        thread_g.create_thread([&](){
            while(flag);
            
            cout << "查找记录:select * from test_table" << endl;  //查找记录
            
            ___Mysql.Operate_Mysql_Query(___Mysql.mysql, "select * from test_table");
            
        });
        */
        

        while(flag)
        {
            std::cout<<"input flag:";
            std::cin>>flag;
        }

        thread_g.join_all();
        //cout << "删除表" << endl;  //删除表
        //Mysql.Operate_Mysql_Modify(Mysql.mysql, "drop table re_test_table");//删除表

        cout << "断开连接" << endl;  //断开连接
        Mysql.disconnect_to_Mysql();

        /*
        cout << "新建表" << endl;  //新建表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "create table if not exists\
                                                test_table(id int(4), name varchar(20) character set gb2312 collate gb2312_chinese_ci)");
        cout << "添加记录" << endl;  //添加记录
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "insert into test_table\
                                                values(1, 'aaa'), (2, 'bbb'), (3, 'ccc')");

        Mysql.Operate_Mysql_Modify(Mysql.mysql, "insert into test_table(id)\
                                                values(11), (22), (33)");
        cout << "查找记录" << endl;  //查找记录
        Mysql.Operate_Mysql_Query(Mysql.mysql, "select * from test_table\
                                                where id = 22");

        Mysql.Operate_Mysql_Query(Mysql.mysql, "select * from test_table");
        cout << "修改记录" << endl;  //修改记录
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "update test_table\
                                                set id = 22\
                                                where name = 'aaa'");
        cout << "修改表明" << endl;  //修改表明
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "alter table test_table rename re_test_table");
        cout << "删除记录" << endl;  //删除记录
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "delete from re_test_table\
                                                where  id = 2");//删除表中的特定条件的记录

        cout << "清空表" << endl;  //清空表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "truncate table re_test_table");//删除表中的所有数据记录，清空表
        //  Mysql.Operate_Mysql_Modify(Mysql.mysql, "delete from test_table");//删除表中的所有数据记录，清空表

        cout << "删除表" << endl;  //删除表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "drop table re_test_table");//删除表

        cout << "断开连接" << endl;  //断开连接
        Mysql.Disconnect_to_Mysql(&Mysql.mysql);

        

        thread_g.join_all();
        cout << "修改记录" << endl;  //修改记录
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "update test_table\
                                                set id = 22\
                                                where name = 'aaa'");
        cout << "修改表明" << endl;  //修改表明
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "alter table test_table rename re_test_table");
        cout << "删除记录" << endl;  //删除记录
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "delete from re_test_table\
                                                where  id = 2");//删除表中的特定条件的记录

        cout << "清空表" << endl;  //清空表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "truncate table re_test_table");//删除表中的所有数据记录，清空表
        //  Mysql.Operate_Mysql_Modify(Mysql.mysql, "delete from test_table");//删除表中的所有数据记录，清空表

        cout << "删除表" << endl;  //删除表
        Mysql.Operate_Mysql_Modify(Mysql.mysql, "drop table re_test_table");//删除表

        cout << "断开连接" << endl;  //断开连接
        Mysql.Disconnect_to_Mysql(&Mysql.mysql);
        cout << "断开连接" << endl;  //断开连接
        _Mysql.Disconnect_to_Mysql(&_Mysql.mysql);
        cout << "断开连接" << endl;  //断开连接
        __Mysql.Disconnect_to_Mysql(&_Mysql.mysql);
        cout << "断开连接" << endl;  //断开连接
        ___Mysql.Disconnect_to_Mysql(&_Mysql.mysql);
    }
    else
    {
        cout << "Error...Failed to connect to Mysql !" << endl;
    }
    */
    mysql_library_end();
    char c;
    while((c = getchar())!='e');
    return 0;
}
