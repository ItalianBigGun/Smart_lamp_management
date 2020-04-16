#ifndef __WORKER_H__
#define __WORKER_H__

#include "Sql.h"
#include "test.pb.h"
#include "taskqueue.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <vector>

using Socket = boost::asio::ip::tcp::socket; 
using namespace std;

class Worker
{

public:
    Worker(int size, Taskqueue* m_q);
    ~Worker();
    int getWorkerSize();

    
    
    //数据库操作
    void initDataBase(
        const char* host = "127.0.0.1", 
        const char* user = "root", 
        const char* pwd = "123456", 
        const char* database = "test", 
        int port = 3306);
    void connectSql();
    void disconnectSql();

    //设备操作
    my::Node& searchDevice(my::Node& node);
    bool checkStatus(std::string id);
    bool deleteDevice(my::Node node);
    bool addDevice(my::Node node);


    bool controlDevice(my::Node node);
    bool resetDevSocket(std::string id);
    bool resetUserSocket(std::string id);
    Socket* getDevSocket(std::string id);
    Socket* getUserSocket(std::string id);

    //线程函数
    bool dowork();
    void threadManagement();
    void addThread();
    void distoryOneThread();
    void distoryAllThread();

private:
    void setqueue(Taskqueue *m_queue);


private:
    int t_size;     //最大线程数
    boost::thread_group tg;
    SQL mysql;
    Taskqueue *m_queue = nullptr;
    my::Node msgnode;
    my::Message *message;
    my::DevNode *devnode;
    int work_size = 0;

    std::vector<boost::thread*> t_ids;
};

//线程函数
bool Worker::dowork()
{
    my::Node m_node;
    my::Message *message;
    my::DevNode *devnode;
    std::string opt,id;
    Socket *socket;
    struct tasknode *task = nullptr;
    while(1){
        try{
        //boost::this_thread::interruption_point();
        //boost::this_thread::disable_interruption di;    //禁用中断
        //变量：my::Node,Socket,std::string
        //1.从队列中获取一个task
        std::cout<<"pop one"<<std::endl;
        task = m_queue->pop();
        shared_ptr<struct tasknode> task_ptr(task);
        std::cout<<"pop finaish"<<std::endl;
        //1.1.将task中的socket和数据分离
        socket = task->socket;
        //1.2.获取数据中的操作
        std::cout<<"the thread id of worker is:"<<boost::this_thread::get_id()
            <<"\nget data:"<<task->taskmsg.SerializeAsString()<<std::endl;
        opt = task->taskmsg.operation();
        //2.根据操作调用对应的函数
        if(opt=="search"){
            m_node = searchDevice(task->taskmsg);//printf("%d\n", __LINE__); 
        }else if(opt=="del"){
            if(deleteDevice(task->taskmsg)){
                m_node = searchDevice(task->taskmsg);
            }else{
                //
            }
        }else if(opt=="add"){
            if(addDevice(task->taskmsg)){
                m_node = searchDevice(task->taskmsg);
            }else{
                //
            }
        }else if(opt=="control"){
            if(checkStatus(id)){
                if(controlDevice(task->taskmsg)){
                    m_node = searchDevice(task->taskmsg);
                }else{
                    //
                }
            }else{
                //
            }
        }
        //3.调用完成
        //3.1.使用socket发送回传数据
        socket->send(boost::asio::buffer(m_node.SerializeAsString()));
        //boost::this_thread::restore_interruption ri(di);   //恢复中断
        //delete task;    
        }catch(std::exception &ex)
        {
            delete task;
            resetDevSocket(id);
            std::cout<<ex.what()<<std::endl;
        }
        
    }
}

//设置任务队列
void Worker::setqueue(Taskqueue* m_queue)
{
    this->m_queue = m_queue;
}

//初始化数据库
void Worker::initDataBase(
    const char* host, 
    const char* user, 
    const char* pwd, 
    const char* database, 
    int port)
{
    this->connectSql();
}

//连接数据库
void Worker::connectSql()
{
    this->mysql.connect_to_Mysql();
}

//断开数据库连接
void Worker::disconnectSql()
{
    this->mysql.disconnect_to_Mysql();
}

//搜索设备
my::Node& Worker::searchDevice(my::Node& node)
{
    std::string sqlstr = "select * from test_device;";
    MYSQL_RES *result = nullptr;
    
    //1.获取结果集
    if (0 == mysql_query(&mysql, sqlstr.c_str()))
    {
        cout << "Operate_Mysql Query Success" << endl;
        result = mysql_store_result(&mysql);
    }else{
        cout << "Operate_Mysql Query Failed" << mysql_errno(&mysql)<<":"<<mysql_error(&mysql) << "from:" <<boost::this_thread::get_id()<<endl;
        //result = nullptr;
    }
    //if(result==nullptr)
        //return nullptr;
    //2.分析结果集
    int datasize = mysql_num_fields(result);
    
    //3.组包数据
    my::Message *message;
    my::DevNode *devnode;
    node.set_operation("search");
    message = node.mutable_m_message();
    MYSQL_ROW row = nullptr;
    row = mysql_fetch_row(result);
    while (nullptr != row){
        devnode = message->add_dev();
        devnode->set_name(row[2]);
        devnode->set_status(row[3]);
        devnode->set_isen(row[4]);
        row = mysql_fetch_row(result);
    }
    mysql_free_result(result);
    //4.返回
    return node;
}

//检测设备状态
bool Worker::checkStatus(std::string id)
{
    
}

//删除设备
bool Worker::deleteDevice(my::Node node)
{

}

//添加设备
bool Worker::addDevice(my::Node node)
{

}

//控制设备
bool Worker::controlDevice(my::Node node)
{

}

//用于动态增删线程
void Worker::threadManagement()
{
    
    while(1){
        boost::this_thread::interruption_point();
        if(0){      //
            addThread();
        }
        if(0){      //
            distoryOneThread();
        }
    }
}

//增加一个线程
void Worker::addThread()
{
    //创建线程
    boost::thread *tmp = this->tg.create_thread(
        //boost::bind(&Worker::dowork)
        [&](){
            this->dowork();
        }
        );
    //保存线程
    this->t_ids.push_back(tmp);
    //更新计数
    work_size = this->t_ids.size();
}

//删除一个线程
void Worker::distoryOneThread()
{
    boost::thread *tmp = nullptr;
    //取出一个线程
    if(this->t_ids.size()==0){
        return ;
    }
    tg.remove_thread(this->t_ids[this->t_ids.size()-1]);
    tmp = this->t_ids[this->t_ids.size()-1];
    this->t_ids.pop_back();
    //中断线程
    tmp->interrupt();

    //等待线程结束并释放空间
    tmp->join();
    delete tmp;

    //更新计数
    work_size = this->t_ids.size()-1;
}

//删除所有线程
void Worker::distoryAllThread()
{
    while(this->t_ids.size()){
        distoryOneThread();
    }
    std::cout<<"distory all thread"<<std::endl;
}

Worker::Worker(int size, Taskqueue* m_q)
    :t_size(size),m_queue(m_q)
{
    this->initDataBase();
    boost::thread *t;
    //初始化一个线程
    t = this->tg.create_thread(
        //boost::bind(&Worker::dowork)
        [&](){
            this->dowork();
        }
        );
    t_ids.push_back(t);
    t = this->tg.create_thread(
    //boost::bind(&Worker::threadManagement)
    [&](){
        this->threadManagement();
    }
    );
    t_ids.push_back(t);
}

bool Worker::resetDevSocket(std::string id)
{
    std::string sqlstr = (boost::format("update test_terminal set status=0 where id=\"%s\";") % id).str();
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

bool Worker::resetUserSocket(std::string id)
{
    std::string sqlstr = (boost::format("update test_account set status=0 where id=\"%s\";") % id).str();
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

Socket* Worker::getDevSocket(std::string id)
{
    Socket *socket = nullptr;
    std::string sqlstr = (boost::format("select status from test_terminal where id=\"%s\";") % id).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES* result = mysql.operate_Query_Result(sqlstr.c_str());
    MYSQL_ROW row = nullptr;
    if(mysql_num_rows(result)){
        row = mysql_fetch_row(result);
        if(row!=nullptr)
            socket = (Socket*)atol(row[0]);
        mysql_free_result(result);
    }
    
    if(socket!=0&&socket!=nullptr){
        return socket;
    }else{
        return nullptr;
    }
}

Socket* Worker::getUserSocket(std::string id)
{
    Socket *socket = nullptr;
    std::string sqlstr = (boost::format("select status from test_account where id=\"%s\";") % id).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES* result = mysql.operate_Query_Result(sqlstr.c_str());
    MYSQL_ROW row = nullptr;
    if(mysql_num_rows(result)){
        row = mysql_fetch_row(result);
        if(row!=nullptr)
            socket = (Socket*)atol(row[0]);
        mysql_free_result(result);
    }
    
    if(socket!=0&&socket!=nullptr){
        return socket;
    }else{
        return nullptr;
    }
}
Worker::~Worker()
{
    distoryAllThread();
    disconnectSql();
}

int Worker::getWorkerSize()
{
    return tg.size();
}


#endif