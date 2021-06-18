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
    Worker(const int size, Taskqueue* m_q);
    ~Worker();
    int getWorkerSize();
    bool stop();

    
    
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
    bool alterDevice(my::Node node);
    bool searchISDN(std::string name);

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
    unsigned int StrtoUInt32(const char *str);      //for 32 bit


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
    bool use_flag = true;

    std::vector<boost::thread*> t_ids;
};

#endif