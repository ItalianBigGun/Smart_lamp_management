#include "socketqueue.h"
#include "worker.h"
#include "test.pb.h"
#include "taskqueue.h"
#include "filter.h"
#include <vector>
#include <string>
#include <boost/any.hpp>
#include "Sql.h"
#include <boost/shared_ptr.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <memory>
using namespace boost;
using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> sock_ptr;
using namespace std;

using Socket = boost::asio::ip::tcp::socket;
using socket_ptr = boost::shared_ptr<boost::asio::ip::tcp::socket>;
//Socketqueue socketq(30);
//Taskqueue taskq(10);
//Filter filter;
//Worker m_worker(2,&taskq);      //处理任务的对象
SQL mysql;

//会话者类
class session : public std::enable_shared_from_this<session>
{
public:
    session(io_context &io_c,Taskqueue &t)
        :m_taskq(t),m_io(io_c)
    {

    }
    ~session(){
        //清空数据库中的socket值
        /*
        std::string sqlstr;
        int port = m_sock->local_endpoint().port();
        std::cout<<"port:"<<port<<std::endl;
        if(port==9527){
            sqlstr = (boost::format("update test_account set status=0 where status=%d;") % m_sock).str();
        }else if(port==9526){
            sqlstr = (boost::format("update test_terminal set status=0 where status=%d;") % m_sock).str();
        }
        std::cout<<sqlstr<<std::endl;
        mysql.operate_Mysql_Modify(sqlstr.c_str());
        */
        //释放socket的内存空间
        delete m_sock;
        m_sock = nullptr;
        std::cout<<"a session ended"<<std::endl;
    }
    
    virtual void do_write(){    //插入任务
        try{
            std::cout<<"session write"<<std::endl;
            //获取数据
        if(!m_sock->available()){
            struct tasknode *task = new struct tasknode;
            int i=0,j=0;
            std::string tmp;
            tmp.clear();
            tmp = std::string(read_data);
            task->taskmsg.ParseFromString(tmp);
            j = task->taskmsg.SerializeAsString().size()-1;
            delete task;
            do{
                task = new struct tasknode;
                //复制数据
                task->taskmsg.ParseFromString(tmp);
                //获取复制的数据长度-1
                j = task->taskmsg.SerializeAsString().size()-1;
                //重定位源字符串
                tmp = tmp.substr(j,tmp.size());

                //打印数据
                i = task->taskmsg.SerializeAsString().size();
                std::cout<<"taskmsg size is:"<<i<<std::endl;

                //入队
                if(i!=0){
                    //存入sock
                    task->socket = m_sock;
                    //存入到任务队列
                    m_taskq.push(task);
                    std::cout<<"task push finish!queue size is:"<<m_taskq.size()<<endl;
                }
                //余下数据长度是否和当前数据长度匹配
                if(tmp.size()<j)
                    break;
            }while(1);

            memset(read_data,0,128);
        }
        }catch(std::exception ex){
            std::cout<<ex.what()<<std::endl;
        }
        do_read();
    }
    virtual void do_read(){     //获取数据
        auto self(shared_from_this());
        //获取数据
        std::cout<<"session reading"<<std::endl;
        m_sock->async_read_some(
            boost::asio::buffer(read_data),
            [self](boost::system::error_code ec,std::size_t length){
                if(ec){
                    std::cout<<"a session ocurr a ec"<<std::endl;
                    //self->~session();
                            std::string sqlstr;
                int port = self->m_sock->local_endpoint().port();
                std::cout<<"port:"<<port<<std::endl;
                if(port==9527){
                    sqlstr = (boost::format("update test_account set status=0 where status=%d;") % self->m_sock).str();
                }else if(port==9526){
                    sqlstr = (boost::format("update test_terminal set status=0 where status=%d;") % self->m_sock).str();
                }
                std::cout<<sqlstr<<std::endl;
                mysql.operate_Mysql_Modify(sqlstr.c_str());
                }else{
                    self->do_write();
                }
            }
            );
    }
    virtual void start(Socket *socket){
        try{
            std::cout<<"a session be created"<<std::endl;
            //获取socket
            m_sock = socket;
            //
            do_read();

            m_io.run();
            
        }catch(std::exception &ex){
            std::cout<<"session:"<<ex.what()<<std::endl;
        } 
    }
    
private:
    Taskqueue &m_taskq;           //外部
    char read_data[128];
    char write_data[128];
    Socket *m_sock = nullptr;
    io_context &m_io;           //外部
};

//服务者类
class server
{
public:
    io_context m_io;
    ip::tcp::acceptor m_acceptor;
    char buf[128];

public:
    server(int port) : m_acceptor(m_io, ip::tcp::endpoint(ip::tcp::v4(), port))
    { accept(); }

    virtual void run(){ m_io.run();}

    virtual void stop(){ m_io.stop();}

    virtual void accept() {std::cout<<"server origin accept function"<<std::endl;};
    virtual void accept_handler(const boost::system::error_code& ec, sock_ptr sock)=0;
    virtual void write_handler(const boost::system::error_code&ec) = 0;
};

class server_c:public server
{
private:
    int m_port;
    Filter filter;
    bool flag = true;
    Taskqueue taskq;
    Socketqueue socketq;
    Worker worker;
    boost::thread *m_thread;
    //boost::asio::strand<boost::asio::io_context::executor_type> *strand_;
public:
    server_c(int port)
        :m_port(port),server(port),
        taskq(10),socketq(30),
        worker(2,&taskq){
            //socketq = new Socketqueue(30);
            //开启一个给每个sock创建一个session的线程
            m_thread = new boost::thread(
                [&](){
                    std::cout<<"c session creator is running"<<std::endl;
                    while(flag){
                        
                        Socket *sock = socketq.pop();
                        
                        if(sock==nullptr){
                            continue;
                        }

                        //获取到一个socket后，创建一个session对象接收用户请求
                        boost::thread t_([&](){
                            std::make_shared<session>(m_io,taskq)->start(sock);
                            std::cout<<"a session thread end"<<std::endl;
                        });
                    }
                    std::cout<<"c session creator is end"<<std::endl;
                }
            );     
        }
    ~server_c(){
        std::cout<<"server_c distroy"<<std::endl;
        
        //delete socketq;
        //worker退出
        //worker.~Worker();
        };
    //server_c(server_c& s):server(s){std::cout<<"copy constructor"<<std::endl;};
    virtual void accept()
    {
        //sock_ptr sock(new ip::tcp::socket(m_io));
        //创建socket
        //实现异步函数同步效果
        boost::asio::spawn(m_io,
        [&](boost::asio::yield_context yield)
        {
            char data[128];
            boost::system::error_code ec;
            while(flag){
                ip::tcp::socket *sock = new ip::tcp::socket(m_io);
                //ip::tcp::socket sock_(m_io);
                memset(data,0,128);
                
                this->m_acceptor.async_accept(*sock, yield[ec]);
                
                std::cout<<"accept a c sock"<<std::endl;
                if (ec)
                { 
                    std::cout<<"ocurr a ec"<<std::endl;
                    return; 
                }
                try{
                    //1.获取数据
                    my::Node msgnode;
                    //1.1异步接收数据
                    
                    std::size_t n = sock->async_read_some(boost::asio::buffer(data), yield);
                    
                    msgnode.Clear();
                    msgnode.ParseFromString(std::string(data));
                    //1.2接收到数据传入doFilter中
                    //监听成功,验证登陆
                    int ret = filter.doFilter(msgnode,sock);
                    std::cout<<"filter finish!"<<std::endl;
                    if(ret == 0){
                        ret = socketq.push(sock);
                        if(ret!=0){
                            cout<<"插入失败!"<<endl;
                            boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                            delete sock;
                            sock = nullptr;
                        }
                        else{
                            boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                            cout<<"插入socket成功!"<<endl;
                        }  
                    }else{
                        boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                        delete sock;
                        sock = nullptr;
                        cout<<"c socket is nullptr because yanzheng failed"<<endl;
                    }
                }catch(std::exception &ex){
                    std::cout<<ex.what()<<std::endl;
                }
            }
            
        });
        
        this->run();
        
    }
    virtual void stop(){
        flag = false; 
        m_io.stop();
        this->worker.stop();
        this->taskq.distroy();
        this->socketq.distroy();

        //m_thread->interrupt();
        //m_thread->join();
    }
    virtual void accept_handler(const boost::system::error_code& ec, sock_ptr sock){};
    virtual void write_handler(const boost::system::error_code&ec){};

};

class server_d:public server
{
private:
    int m_port;
    Filter filter;
    bool flag = true;
    Taskqueue taskq;
    Socketqueue socketq;
    Worker worker;
    boost::thread *m_thread;
    //boost::asio::strand<boost::asio::io_context::executor_type> *strand_;
public:
    server_d(int port)
        :m_port(port),server(port),
        taskq(10),socketq(30),
        worker(2,&taskq){
            //socketq = new Socketqueue(30);
            //开启一个给每个sock创建一个session的线程
            m_thread = new boost::thread(
                [&](){
                    std::cout<<"d session creator is running"<<std::endl;
                    while(flag){
                        
                        Socket *sock = socketq.pop();
                        
                        if(sock==nullptr){
                            continue;
                        }
                        boost::thread t_([&](){
                            
                            std::make_shared<session>(m_io,taskq)->start(sock);
                        });
                    }
                    std::cout<<"d session creator is end"<<std::endl;
                }
            );     
        }
    ~server_d(){
        std::cout<<"server_d distroy"<<std::endl;
        //delete socketq;
        //worker退出
        //worker.~Worker();
        };
    //server_d(server_d& s):server(s){std::cout<<"copy constructor"<<std::endl;};
    virtual void accept()
    {
        //sock_ptr sock(new ip::tcp::socket(m_io));
        //创建socket
        //实现异步函数同步效果
        boost::asio::spawn(m_io,
        [&](boost::asio::yield_context yield)
        {
            char data[128];
            boost::system::error_code ec;
            while(flag){
                ip::tcp::socket *sock = new ip::tcp::socket(m_io);
                //ip::tcp::socket sock_(m_io);
                memset(data,0,128);

                
                this->m_acceptor.async_accept(*sock, yield[ec]);
                
                std::cout<<"accept a d sock"<<std::endl;
                if (ec)
                { 
                    std::cout<<"ocurr a ec"<<std::endl;
                    return; 
                }
                try{
                    //1.获取数据
                    my::Node msgnode;
                    //1.1异步接收数据
                    std::size_t n = sock->async_read_some(boost::asio::buffer(data), yield);
                    
                    msgnode.Clear();
                    msgnode.ParseFromString(std::string(data));
                    //1.2接收到数据传入doFilter中
                    //监听成功,验证登陆
                    int ret = filter.doFilter(msgnode,sock);
                    std::cout<<"filter finish!"<<std::endl;
                    if(ret == 0){
                        ret = socketq.push(sock);
                        if(ret!=0){
                            cout<<"插入失败!"<<endl;
                            boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                            delete sock;
                            sock = nullptr;
                        }
                        else{
                            boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                            cout<<"插入socket成功!"<<endl;
                        }  
                    }else{
                        boost::asio::async_write(*sock, boost::asio::buffer(msgnode.SerializeAsString()), yield);
                        delete sock;
                        sock = nullptr;
                        cout<<"d socket is nullptr because yanzheng failed"<<endl;
                    }
                }catch(std::exception &ex){
                    std::cout<<ex.what()<<std::endl;
                }
            }
            
        });
        
        this->run();
        
    }
    virtual void stop(){
        flag = false; 
        m_io.stop();
        //this->worker.stop();
        //this->taskq.distroy();
        this->worker.stop();
        this->taskq.distroy();
        this->socketq.distroy();

        //m_thread->interrupt();
        //m_thread->join();
    }
    virtual void accept_handler(const boost::system::error_code& ec, sock_ptr sock){};
    virtual void write_handler(const boost::system::error_code&ec){};

};
/////////////////////////////////
int main()
{
    std::cout<<"lv1.0"<<std::endl;
    std::cout << "初始化数据库" << std::endl;//初始化数据库
    if (0 == mysql_library_init(0, NULL, NULL))
        cout << "mysql_library_init() succeed" << endl;
    else
        cout << "mysql_library_init() failed" << endl;
    mysql.connect_to_Mysql();

    try
    {
        server_c *sc_ptr = new server_c(9527);
        server_d *sd_ptr = new server_d(9526);

        boost::thread t1(boost::bind(&server_c::accept,sc_ptr));
        boost::thread t2(boost::bind(&server_d::accept,sd_ptr));
        char c;
        while((c=getchar())){
            if(c=='e'||c=='E'){
                sc_ptr->stop();
                sd_ptr->stop();
                std::cout<<"stop finish"<<std::endl;
                t1.join();
                t2.join();
                std::cout<<"join finish"<<std::endl;
                delete sc_ptr;
                delete sd_ptr;
                //std::cout<<"delete finish"<<std::endl;
                break;
            }
        }
    std::cout<<"1"<<std::endl;
    }
    catch (std::exception &e)
    {
        cout<<"server:"<<e.what()<<endl;
    }
    
        
    getchar();

    mysql_library_end();
    std::cout<<"happy end"<<std::endl;
    return 0;
}


//生产者函数：用于接收连接进来的socket，验证socket，成功则入队
//外部资源为socket队列

#if 0       //在之前的同步测试中使用到的函数

void func_server()
{
    //不断获取连接
    boost::this_thread::sleep(boost::posix_time::seconds(1));
    int ret;
    
    //创建socket
    Socket *socket;
    while(1){
        
        //监听socket
        try{
            socket = new Socket(io_s);
            acceptor.accept(*socket);
            std::cout<<"accept a socket"<<std::endl;
            //监听成功,验证登陆
            socket = filter.doFilter(socket);
            std::cout<<"filter finish!"<<std::endl;
            //验证成功,插入到队列中
            if(socket!=nullptr){
                int ret = socketq.push(socket);
                if(ret!=0)
                    cout<<"插入失败!"<<endl;
                else
                    cout<<"插入socket成功!"<<endl;
            }else{
                cout<<"socket is nullptr because yanzheng failed"<<endl;
                delete socket;
                continue;
            }
        }catch(std::exception &ex){
            std::cout<<ex.what()<<std::endl;
        }
    }
    
    //io_s.run();
}
//消费者函数：从队列中取出socket，接收数据，将任务入队
//外部资源为socket队列和task队列

void func_comstmer()
{
    Socket *socket=nullptr;
    struct tasknode *task;
    char buf[128];
    std::string tmp;
    int i,j;
    int port;
    while(1)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        //取socket
        socket = socketq.pop();
        if(socket==nullptr)
            continue;
        port = socket->local_endpoint().port();
        //std::cout<<"pop finish!queue size is:"<<socketq.size()<<endl;

        boost::shared_ptr<Socket> socketPtr(socket);
        try
        {
            //发出任务
            while(1)
            {
                //创建任务结点
                task = new struct tasknode;
                //建立缓冲区
                memset(buf,0,sizeof(buf));
                //读取数据
                //socket->read_some(boost::asio::buffer(buf));
                while((socket->read_some(boost::asio::buffer(buf)))&&socket->available())
                {
                    std::cout<<"buffer:"<<(buf)<<std::endl;
                }
                
                std::cout<<"get data:"<<(buf)<<"from:"<<port<<endl;
                //存入数据

                i=0,j=0;
                tmp.clear();
                tmp = std::string(buf);
                task->taskmsg.ParseFromString(tmp);
                j = task->taskmsg.SerializeAsString().size()-1;
                delete task;
                do{
                    task = new struct tasknode;
                    //复制数据
                    task->taskmsg.ParseFromString(tmp);
                    //获取复制的数据长度-1
                    j = task->taskmsg.SerializeAsString().size()-1;
                    //重定位源字符串
                    tmp = tmp.substr(j,tmp.size());

                    //打印数据
                    i = task->taskmsg.SerializeAsString().size();
                    std::cout<<"taskmsg size is:"<<i<<std::endl;

                    //入队
                    if(i!=0){
                        //存入sock
                        task->socket = socket;
                        //存入到任务队列
                        taskq.push(task);
                        std::cout<<"task push finish!queue size is:"<<taskq.size()<<endl;
                    }

                    //余下数据长度是否和当前数据长度匹配
                    if(tmp.size()<j)
                        break;
                    
                }while(1);
            }
        }
        catch(std::exception &ex)
        {
            std::string sqlstr;
            std::cout<<"port:"<<port<<std::endl;
            if(port==9527){
                sqlstr = (boost::format("update test_account set status=0 where status=%d;") % socket).str();
            }else if(port==9526){
                sqlstr = (boost::format("update test_terminal set status=0 where status=%d;") % socket).str();
            }
            std::cout<<sqlstr<<std::endl;
            std::cout<<ex.what()<<std::endl;
            mysql.operate_Mysql_Modify(sqlstr.c_str());
        }
    }
    
}

void func_producter_for_dev()
{
        //mysql_thread_init();
    
    //不断获取连接
    while(1)
    {
        boost::this_thread::sleep(boost::posix_time::seconds(1));
        int ret;
        //创建socket
        boost::asio::ip::tcp::socket *socket = new boost::asio::ip::tcp::socket(io_s);
        try{
            //监听socket
            devacceptor.accept(*socket);
        }catch(std::exception &ex){
            std::cout<<"devacceptor close:"<<ex.what()<<std::endl;
        }
        
        std::cout<<"accept a devsocket"<<std::endl;
        //监听成功,验证登陆
        socket = filter.doFilter(socket);
        std::cout<<"filter finish!"<<std::endl;
        //验证成功,插入到队列中
        if(socket!=nullptr){
            ret = socketq.push(socket);
            if(ret!=0)
                cout<<"插入失败!"<<endl;
            else
                cout<<"插入socket成功!"<<endl;
        }else{
            cout<<"socket is nullptr because yanzheng failed"<<endl;
            delete socket;
            continue;
        }
    }
}

#endif


//
/*

struct cjson
{
    string operation;
    string message;
};

client ==> service
switch operation
    case "login":
        message = [no, password]
    case "register":
        message = [no, password]
    case "changepassword":
        message = [no, password]
    case "searchdev":
        message = [[devno, devname, devISON, devstatus],...]
    case "adddev":
        message = [devname, devISON]
    case "deletedev":
        message = [devname]
    case "controldev":
        message = [devname, devstatus]
    
service ==> client
switch operation
    case "login/register/changepassword/addev/deletedev":
        message = [errorcode]
    case "searchdev":
        message = [[devname, devstatus],...]





*/

/*

//销售员函数：用于从队列中取走任务，执行任务，发送数据
//外部资源为task队列
void func_worker()
{
    struct tasknode *task=nullptr;
    while(1)
    {
        //取task
        task = taskq.pop();
        if(task==nullptr)
            continue;
        std::cout<<"task pop finish!queue size is:"<<taskq.size()<<endl;

        //存入shared_ptr
        boost::shared_ptr<struct tasknode> taskPtr(task);

        //while(1)
        {
            //执行任务
            try
            {
                //建立缓冲区
                char buf[128];
                memset(buf,0,sizeof(buf));
                //boost::this_thread::sleep(boost::posix_time::seconds(2));
                //读取数据
                task->socket->send(boost::asio::buffer(task->taskmsg.SerializeAsString()));
            }
            catch(std::exception &ex)
            {
                std::cout<<ex.what()<<std::endl;
                break;
            }
        }
    }
}

class server
{
private:
    io_context &m_io;
    ip::tcp::acceptor m_acceptor;

public:
    server(io_context& io_s) : m_io(io_s),m_acceptor(io_s, ip::tcp::endpoint(ip::tcp::v4(), 9527))
    { accept(); }

    void run(){ io_s.run();}

    void accept()
    {
        sock_ptr sock(new ip::tcp::socket(io_s));
        m_acceptor.async_accept(*sock, boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
    }

    void accept_handler(const boost::system::error_code& ec, sock_ptr sock)
    {
        if (ec)
        { 
            return; 
        }
        cout<<"Client:";
        cout<<sock->remote_endpoint().address()<<endl;
        sock->async_write_some(buffer("hello asio"), boost::bind(&server::write_handler, this, boost::asio::placeholders::error));
        // 发送完毕后继续监听，否则io_service将认为没有事件处理而结束运行 
        accept();
}

    void write_handler(const boost::system::error_code&ec)
    {
        cout<<"send msg complete"<<endl;
    }
};

*/
