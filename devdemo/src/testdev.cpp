#include "socketqueue.h"
#include "worker.h"
#include "test.pb.h"
#include "taskqueue.h"
#include "filter.h"
#include <vector>
#include <string>
#include <boost/any.hpp>
using namespace std;

boost::asio::io_service io_service; 
boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 9526); 
boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
using Socket = boost::asio::ip::tcp::socket;
Socketqueue socketq(2);
Taskqueue taskq(10);
Filter filter;
Worker m_worker(2,&taskq);      //处理任务的对象

void callback_timeout(const boost::system::error_code&){
    std::cout<<"超时"<<std::endl;
}

//生产者函数：用于接收连接进来的socket，验证socket，成功则入队
//外部资源为socket队列
void func_producter()
{
    //mysql_thread_init();
    
    //不断获取连接
    while(1)
    {
        int ret;
        //创建socket
        boost::asio::ip::tcp::socket *socket = new boost::asio::ip::tcp::socket(io_service);
    
        //监听socket
        acceptor.accept(*socket);
        std::cout<<"accept a socket"<<std::endl;
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
        
        //std::cout<<"push finish!queue size is:"<<socketq.size()<<endl;
        /*
        socket = socketq.pop();
        cout<<"queue size is:"<<socketq.size()<<endl;
        socket->send(boost::asio::buffer("hello"));
        */
    }
    //mysql_thread_end();
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
    while(1)
    {
        //取socket
        socket = socketq.pop();
        if(socket==nullptr)
            continue;
        //std::cout<<"pop finish!queue size is:"<<socketq.size()<<endl;

        shared_ptr<Socket> socketPtr(socket);
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
                std::cout<<"get data:"<<(buf)<<"from:"<<socket->remote_endpoint().address()<<endl;
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
                    std::cout<<task->taskmsg.SerializeAsString().size()<<std::endl;

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
                /*{int i = 0,j=10;
                std::string tmp = std::string(buf);
                i = tmp.length();
                task->taskmsg.ParseFromString(tmp);
                j = task->taskmsg.SerializeAsString().size();
                std::cout<<"i:"<<i<<"j:"<<j<<std::endl;
                if((i%j)!=0){
                    
                    std::cout<<"数据异常!"<<std::endl;
                    continue;
                }else{
                    std::cout<<"数据正常!"<<std::endl;
                    if(i>j){
                        std::cout<<"出现重发数据"<<std::endl;
                    }
                } }*/
                

                //存入sock
                //task->socket = socket;
                //存入到任务队列
                //taskq.push(task);
                //std::cout<<"task push finish!queue size is:"<<taskq.size()<<endl;
            }
        }
        catch(std::exception &ex)
        {
            std::cout<<ex.what()<<std::endl;
        }
    }
    
}

int main()
{
    cout << "初始化数据库" << endl;//初始化数据库
    if (0 == mysql_library_init(0, NULL, NULL))
        cout << "mysql_library_init() succeed" << endl;
    else
        cout << "mysql_library_init() failed" << endl;

    boost::thread_group tg;

    tg.create_thread(boost::bind(&func_comstmer));
    tg.create_thread(boost::bind(&func_comstmer));
    //tg.create_thread(boost::bind(&func_worker));
    tg.create_thread(boost::bind(&func_producter));
    char c;
    while((c=getchar())){
        if(c=='e'||c=='E'){
            //socketq.push(nullptr);
            //tg.interrupt_all();

            std::cout<<m_worker.getWorkerSize()<<std::endl;
            m_worker.distoryOneThread();
            std::cout<<m_worker.getWorkerSize()<<std::endl;
            //break;
        }
    }
        
    
    getchar();


    tg.join_all();

    mysql_library_end();
    std::cout<<"happy end"<<std::endl;
    return 0;
}


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
*/