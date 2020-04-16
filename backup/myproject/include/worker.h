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
    int port;
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
        id = task->taskmsg.m_message().id();
        //2.根据操作调用对应的函数
        if(opt=="search"){
            m_node = searchDevice(task->taskmsg);//printf("%d\n", __LINE__); 
        }else if(opt=="del"){
            std::cout<<"deling isdn is:"<<task->taskmsg.m_message().dev(0).isdn()<<std::endl;
            if(deleteDevice(task->taskmsg)){
                m_node = searchDevice(task->taskmsg);
            }else{
                m_node.mutable_m_message()->set_error("delete");//
            }
        }else if(opt=="add"){
            if(addDevice(task->taskmsg)){
                m_node = searchDevice(task->taskmsg);
            }else{
                m_node.mutable_m_message()->set_error("add");//
            }
        }else if(opt=="control"){       //control操作的数据，分用户和设备
            port = socket->local_endpoint().port();
            std::cout<<"control device size is:"<<task->taskmsg.m_message().dev_size()<<std::endl;
            if(port==9527){     //用户
                if(checkStatus(id)){
                    if(controlDevice(task->taskmsg)){
                        //if(alterDevice(task->taskmsg)){
                            //m_node = searchDevice(task->taskmsg);
                        //}else{

                        //}
                        continue;
                    }else{
                        m_node.mutable_m_message()->set_error("control");//
                    }
                    }else{
                        m_node.mutable_m_message()->set_error("Status");//
                    }
            }else if(port==9526){   //设备
                std::cout<<"device update itself"<<std::endl;
                int ret = alterDevice(task->taskmsg);
                //用户控制设备时返回数据
                if(!task->taskmsg.m_message().error().empty()){
                    if(task->taskmsg.m_message().error().size()!=0){
                        std::string id = task->taskmsg.m_message().id();
                        m_node = searchDevice(task->taskmsg);
                        std::cout<<"device size is:"<<task->taskmsg.m_message().dev_size()<<std::endl;
                        m_node.mutable_m_message()->set_error("");
                    }else{
                        m_node.mutable_m_message()->set_error("control");
                    }
                    //获取用户socket
                    //发送数据给用户
                    std::cout<<id<<std::endl;
                    getUserSocket(id)->async_send(
                    boost::asio::buffer(m_node.SerializeAsString()),
                    [&](boost::system::error_code ec, std::size_t lenght){
                        if(ec){
                            return ;
                        }
                        std::cout<<"send back to client"<<std::endl;;
                    }); 
                    continue;
                }else{
                    if(ret){
                    m_node.mutable_m_message()->set_error("");
                    }else{
                        m_node.mutable_m_message()->set_error("alter");
                    }
                }
            }
        }
        //3.调用完成
        //3.1.使用socket发送回传数据
        socket->async_send(
            boost::asio::buffer(m_node.SerializeAsString()),
            [&](boost::system::error_code ec, std::size_t lenght){
                ;
            });   
        }catch(std::exception &ex)
        {
            delete task;
            resetUserSocket(id);
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
    std::string id;
    id = node.m_message().id();
    std::string sqlstr = (boost::format(
        "select * from test_device where t_id in   \
        (select id from test_terminal where a_id=\"%s\");"
        ) % id).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES *result = nullptr;
    
    //1.获取结果集
    result = mysql.operate_Query_Result(sqlstr.c_str());
    //if(result==nullptr)
        //return nullptr;
    //2.分析结果集
    int datasize = mysql_num_rows(result);
    
    //3.组包数据
    my::Message *message;
    my::DevNode *devnode;
    node.clear_m_message();
    node.set_operation("search");
    message = node.mutable_m_message();
    MYSQL_ROW row = nullptr;
    row = mysql_fetch_row(result);
    while (nullptr != row){
        devnode = message->add_dev();
        devnode->set_name(row[1]);
        devnode->set_status(row[3]);
        devnode->set_isdn(row[2]);
        row = mysql_fetch_row(result);
    }
    mysql_free_result(result);
    //4.返回
    return node;
}

//检测设备状态
bool Worker::checkStatus(std::string id)
{
    std::string sqlstr;
    sqlstr = (boost::format(
        "select status from test_terminal where a_id=\"%s\";"
        ) % id).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES *result = nullptr;
    
    //1.获取结果集
    result = mysql.operate_Query_Result(sqlstr.c_str());
    //2.分析结果集
    int datasize = mysql_num_rows(result);
    MYSQL_ROW row = nullptr;
    row = mysql_fetch_row(result);
    if(datasize==1){
        int status = atol(row[0]);
        std::cout<<"status:"<<status<<std::endl;
        mysql_free_result(result);
        return status;
    }else{
        mysql_free_result(result);
        return false;
    }
}

//删除设备
bool Worker::deleteDevice(my::Node node)
{
    std::string id;
    id = node.m_message().id();
    std::string sqlstr;
    int flag = 0;
    //1.遍历对象中的设备数据，并逐个做删除操作
    for(int i=0,j=node.m_message().dev_size();i<j;++i){
        std::string ISDN, name;
        ISDN = node.m_message().dev(i).isdn();
        name = node.m_message().dev(i).name();
        sqlstr = (boost::format(
        "delete from test_device where t_id in   \
        (select id from test_terminal where a_id=\"%s\") and \
        ISDN = \"%s\" and name = \"%s\";"
        ) % id % ISDN % name).str();
        std::cout<<sqlstr<<std::endl;
        int ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
        flag += ret;
    }

    //2.判断是否全部操作成功
    if(flag==node.m_message().dev_size()){
        return true;
    }else{
        return false;
    } 
}

//添加设备
bool Worker::addDevice(my::Node node)
{
    std::string id;
    id = node.m_message().id();
    std::string sqlstr;
    int flag = 0;
    //1.遍历对象中的设备数据，并逐个做插入操作
    for(int i=0,j=node.m_message().dev_size();i<j;++i){
        std::string ISDN, name;
        ISDN = node.m_message().dev(i).isdn();
        std::cout<<"ISDN:"<<ISDN<<std::endl;
        if(searchISDN(ISDN)){
            name = node.m_message().dev(i).name();
            sqlstr = (boost::format(
            "insert into test_device(t_id,name,ISDN,status)\
            value((select id from test_terminal where a_id=\"%s\"),\"%s\",\"%s\",0);"
            ) % id % name % ISDN).str();
            std::cout<<sqlstr<<std::endl;
            int ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
            flag += ret;
        }else{
            continue;
        }
    }

    //2.判断是否全部操作成功
    if(flag==node.m_message().dev_size()){
        return true;
    }else{
        return false;
    }
}

//控制设备
bool Worker::controlDevice(my::Node node)
{
    std::string sqlstr;
    std::string id, t_id;
    char buf[128]={0};
    Socket *t_socket;
    MYSQL_ROW row = nullptr;
    MYSQL_RES *result = nullptr;
    //在控制设备之前，我们已经做了检测设备是否在线
    //1.解析用户id，并从数据库中获取设备socket
    id = node.m_message().id();
    
    t_socket = getDevSocket(id);
    
    if(t_socket!=nullptr){
        
    }else if(t_socket==nullptr){
        
        return false;
    }
    
    mysql_free_result(result);
    //2.将protobuf对象发送给设备
    int flag = 1;
    t_socket->async_send(
        boost::asio::buffer(node.SerializeAsString()),
        [&](boost::system::error_code ec,std::size_t length){
            if(ec){
                flag = -1;
            }
            flag = 0;
            printf("%d:\n",__LINE__);
        }
        );

    while(flag);
    printf("%d:\n",__LINE__);
    if(flag==-1){
        return 0;
    }else{
    }

    return 1;

    /*
    
    //3.接收设备返回的数据
    flag = 1;
    t_socket->async_read_some(boost::asio::buffer(buf),
        [&](boost::system::error_code ec,std::size_t length){
            if(flag){
                flag = -1;
            }
            flag = 0;
            printf("%d:\n",__LINE__);
        });

    while(flag);
    printf("%d:\n",__LINE__);
    if(flag==-1){
        return 0;
    }else{
    }

    std::cout<<"get data from dev"<<std::endl;
    node.ParseFromString(buf);
    std::cout<<"get from device dev size:"<<node.m_message().dev_size()<<std::endl;
    //4.判断是否设置成功
    std::string error = node.m_message().error();
    std::cout<<"error:"<<error<<std::endl;
    return error.size();
    */
}

//用于更新设备数据
bool Worker::alterDevice(my::Node node)
{
    std::string id;
    id = node.m_message().id();
    std::string sqlstr;
    int flag = 0;
    std::cout<<"device size:"<<node.m_message().dev_size()<<std::endl;
    //1.遍历对象中的设备数据，并逐个做删除操作
    for(int i=0,j=node.m_message().dev_size();i<j;++i){
        std::string ISDN, name;
        int status;
        ISDN = node.m_message().dev(i).isdn();
        name = node.m_message().dev(i).name();
        status = atoi(node.m_message().dev(i).status().c_str());
        sqlstr = (boost::format(
        "update test_device set status=%d \
        where t_id = (select id from test_terminal where a_id = \"%s\") \
        and ISDN = \"%s\" and name=\"%s\";"
        ) %status % id % ISDN % name).str();
        std::cout<<sqlstr<<std::endl;
        int ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
        flag += ret;
    }

    //2.判断是否全部操作成功
    if(flag==node.m_message().dev_size()){
        return true;
    }else{
        return false;
    } 
}
bool Worker::searchISDN(std::string name)
{
    std::string sqlstr;
    sqlstr = (boost::format(
        "select * from test_ISDN where name=\"%s\";"
        ) % name).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES *result = nullptr;
    
    //1.获取结果集
    result = mysql.operate_Query_Result(sqlstr.c_str());
    //2.分析结果集
    int datasize = mysql_num_rows(result);
    mysql_free_result(result);
    return datasize;
}

//用于动态增删线程
void Worker::threadManagement()
{
    
    while(1){
        boost::this_thread::sleep(boost::posix_time::seconds(1));
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

Worker::Worker(const int size, Taskqueue* m_q)
{
    t_size = size;
    m_queue = m_q;
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
    std::cout<<sqlstr<<std::endl;
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

Socket* Worker::getDevSocket(std::string id)
{
    Socket *socket = nullptr;
    std::string sqlstr = (boost::format("select status from test_terminal where a_id=\"%s\";") % id).str();
    std::cout<<sqlstr<<std::endl;
    MYSQL_RES* result = mysql.operate_Query_Result(sqlstr.c_str());
    MYSQL_ROW row = nullptr;
    
    if(mysql_num_rows(result)){
        
        row = mysql_fetch_row(result);
        
        if(row!=nullptr){
            
            socket = (Socket*)atol(row[0]);
        }
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