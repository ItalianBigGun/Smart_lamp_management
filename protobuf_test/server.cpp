#include <iostream>
#include "test.pb.h"
#include <boost/asio.hpp>
 
using namespace boost::asio;
 
int main(int argc, char* argv[])
{
    //lm::helloworld *mypro = new lm::helloworld();
    // 所有asio类都需要io_service对象
    io_service iosev;
    ip::tcp::acceptor acceptor(iosev, 
        ip::tcp::endpoint(ip::tcp::v4(), 9527));
    for(;;)
    {
        char getBuffer[128];
        memset(getBuffer,0,128);
        // socket对象
        ip::tcp::socket socket(iosev);
        // 等待直到客户端连接进来
        acceptor.accept(socket);
        // 显示连接进来的客户端
        std::cout << socket.remote_endpoint().address() << std::endl; 
        boost::system::error_code ec;

        my::Node buf;
        //接收
        socket.read_some(buffer(getBuffer));
	std::cout<<"getbuffer is:"<<getBuffer<<std::endl;
        buf.ParseFromString(std::string(getBuffer));
        std::cout<<"receive from client:"<<buf.operation()<<buf.m_message().id()<<buf.m_message().password()<<std::endl;
	/*
        buf.set_str("abcabc");

        std::string sendstr = buf.SerializeAsString();

        //发送
        socket.write_some(buffer(sendstr), ec);
 	*/
        // 如果出错，打印出错信息
        if(ec)
        {
            std::cout << 
                boost::system::system_error(ec).what() << std::endl;
            break;
        }
        // 与当前客户交互完成后循环继续等待下一客户连接
    }
    return 0;
}
