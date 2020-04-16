#ifndef __WORKER_H__
#define __WORKER_H__

#include "Sql.h"
#include "test.pb.h"
#include "taskqueue.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <vector>

using Socket = boost::asio::ip::tcp::socket; 
using namespace std;

template<class T>
class Worker
{

public:
    Worker(int size, T* m_q);
    ~Worker();

    
    
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
    my::Node& searchDevice(std::string id);
    bool checkStatus(std::string id);
    bool deleteDevice(my::Node node);
    bool addDevice(my::Node node);
    bool controlDevice(my::Node node);

    //线程函数
    bool dowork();
    void threadManagement();
    void addThread();
    void distoryOneThread();
    void distoryAllThread();

private:
    void setqueue(T *m_queue);


private:
    int t_size;     //最大线程数
    boost::thread_group tg;
    SQL mysql;
    T *m_queue = nullptr;
    my::Node msgnode;
    my::Message *message;
    my::DevNode *devnode;
    int work_size = 0;

    std::vector<boost::thread*> t_ids;
};

template<class T>
Worker<T>::Worker(int size, T* m_q)
    :t_size(size),m_queue(m_q)
{
    //初始化一个线程
    this->tg.create_thread(
        boost::bind(&Worker::dowork)
        );
    this->tg.create_thread(
    boost::bind(&Worker<T>::threadManagement)
    );
}

template<class T>
Worker<T>::~Worker()
{
    distoryAllThread();
    disconnectSql();
}

template<class T>
void Worker<T>::setqueue(T* m_queue)
{
    this->m_queue = m_queue;
}

template<class T>
void Worker<T>::initDataBase(
    const char* host, 
    const char* user, 
    const char* pwd, 
    const char* database, 
    int port)
{
    
}

template<class T>
void Worker<T>::connectSql()
{
    this->mysql.connect_to_Mysql();
}
template<class T>
void Worker<T>::disconnectSql()
{
    this->mysql.disconnect_to_Mysql();
}
template<class T>
my::Node& Worker<T>::searchDevice(std::string id)
{
    my::Node m_node;
    std::string sqlstr = "select * from test_device;";
    MYSQL_RES *result = nullptr;
    
    //1.获取结果集
    result = this->mysql.operate_Query_Result(sqlstr.c_str());
    //2.分析结果集
    int datasize = mysql_num_fields(result);
    //3.组包数据
    my::Message *message;
    my::DevNode *devnode;
    message = m_node.mutable_m_message();
    MYSQL_ROW row = nullptr;
    row = mysql_fetch_row(result);
    while (nullptr != row){
        devnode = message->add_dev();
        devnode->set_name(row[2]);
        devnode->set_status(row[3]);
        devnode->set_isen(row[4]);
        row = mysql_fetch_row(result);
    }
    //4.返回
    return m_node;
}

template<class T>
bool Worker<T>::checkStatus(std::string id)
{
    
}

template<class T>
bool Worker<T>::deleteDevice(my::Node node)
{

}

template<class T>
bool Worker<T>::addDevice(my::Node node)
{

}

template<class T>
bool Worker<T>::controlDevice(my::Node node)
{

}

template<class T>
bool Worker<T>::dowork()
{
    my::Node msgnode;
    my::Message *message;
    my::DevNode *devnode;
    std::string opt,id;
    Socket *socket;
    struct tasknode *task = nullptr;
    while(1){
        boost::this_thread::interruption_point();
        boost::this_thread::disable_interruption di;    //禁用中断
        //变量：my::Node,Socket,std::string
        //1.从队列中获取一个task
        task = m_queue->pop();
        //1.1.将task中的socket和数据分离
        socket = task->socket;
        //1.2.获取数据中的操作
        std::cout<<"the thread id of worker is:"<<boost::this_thread::get_id()
            <<"\nget data:"<<task->taskmsg.SerializeAsString()<<std::endl;
        opt = task->taskmsg.operation();
        id = task->taskmsg.m_message().id();
        //2.根据操作调用对应的函数
        if(opt=="search"){
            msgnode = searchDevice(id);
        }else if(opt=="del"){
            if(deleteDevice(task->taskmsg)){
                msgnode = searchDevice(id);
            }else{
                //
            }
        }else if(opt=="add"){
            if(addDevice(task->taskmsg)){
                msgnode = searchDevice(id);
            }else{
                //
            }
        }else if(opt=="control"){
            if(checkStatus(id)){
                if(controlDevice(task->taskmsg)){
                    msgnode = searchDevice(id);
                }else{
                    //
                }
            }else{
                //
            }
        }
        //3.调用完成
        //3.1.使用socket发送回传数据
        socket->send(boost::asio::buffer(msgnode.SerializeAsString()));
        boost::this_thread::restore_interruption ri(di);   //恢复中断
    }
}

//用于动态增删线程
template<class T>
void Worker<T>::threadManagement()
{
    while(1){
        if(0){      //
            addThread();
        }
        if(0){      //
            distoryOneThread();
        }
    }
}

//增加一个线程
template<class T>
void Worker<T>::addThread()
{
    //创建线程
    boost::thread *tmp = this->tg.create_thread(
        boost::bind(&Worker::dowork)
        );
    //保存线程
    this->t_ids.push_back(tmp);
    //更新计数
    work_size = this->t_ids.size();
}

//删除一个线程
template<class T>
void Worker<T>::distoryOneThread()
{
    boost::thread *tmp = nullptr;
    //取出一个线程
    tmp = this->t_ids.front();
    this->t_ids.pop_back();
    //中断线程
    tmp->interrupt();
    //等待线程结束并释放空间
    tmp->join();
    delete tmp;
    //更新计数
    work_size = this->t_ids.size();
}

//删除所有线程
template<class T>
void Worker<T>::distoryAllThread()
{
    while(this->t_ids.size()){
        distoryOneThread();
    }
    std::cout<<"distory all thread"<<std::endl;
}


#endif