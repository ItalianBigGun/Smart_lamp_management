//code of client
#include <iostream>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <string>
#include "test.pb.h"
using namespace std;
using boost::asio::ip::tcp;

void read_handler(const boost::system::error_code& ec,std::string msg)
{
    if(ec)
        return;
    std::cout<<"get from service:"<<msg<<std::endl;
    
}
 
int main(int argc, char* argv[])
{
	my::Node *mypro = new my::Node();
    try
    {
        //(1)通过tcp::socket类定义一个tcp client对象socket
        boost::asio::io_service io;
        tcp::socket socket(io);
        
        //(2)通过connect函数连接服务器，打开socket连接。
        tcp::endpoint end_point(boost::asio::ip::address::from_string("47.107.184.34"), 9527);
        socket.connect(end_point);
 
        for (;;)
        {
            std::string buf;
            char getbuffer[256];
            memset(getbuffer,0,256);
            boost::system::error_code error;
 
            std::cout<<"发送数据:"<<std::endl;
            buf = "helloworld";

            //封装proto数据包
            mypro->set_operation("login");
            buf = mypro->SerializeAsString();
            //(3)通过send函数来发数据
            size_t len = socket.send(boost::asio::buffer(buf));

           // lm::helloworld getpro;
            //socket.async_read_some(
           // socket.receive(boost::asio::buffer(getbuffer));
            //boost::bind(&read_handler,error,getbuffer));
           // std::string getstring(getbuffer);

            //getpro.ParseFromString(getstring);

           // std::cout<<"get from service:"<<getpro.id()<<getpro.str()<<std::endl;
 
            if (error)
            {
                throw boost::system::system_error(error);    //some other error
            }
 
            //cout.write(buf.data(), len);
        }
        io.run();
    }
    catch (std::exception& e)
    {
        cout<<e.what()<<endl;
    }
}
