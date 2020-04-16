#include <iostream>
#include "test.pb.h"
#include <boost/asio.hpp>
 
using namespace boost::asio;
 
int main(int argc, char* argv[])
{
    //lm::helloworld *mypro = new lm::helloworld();
    // ����asio�඼��Ҫio_service����
    io_service iosev;
    ip::tcp::acceptor acceptor(iosev, 
        ip::tcp::endpoint(ip::tcp::v4(), 9527));
    for(;;)
    {
        char getBuffer[128];
        memset(getBuffer,0,128);
        // socket����
        ip::tcp::socket socket(iosev);
        // �ȴ�ֱ���ͻ������ӽ���
        acceptor.accept(socket);
        // ��ʾ���ӽ����Ŀͻ���
        std::cout << socket.remote_endpoint().address() << std::endl; 
        boost::system::error_code ec;

        my::Node buf;
        //����
        socket.read_some(buffer(getBuffer));
	std::cout<<"getbuffer is:"<<getBuffer<<std::endl;
        buf.ParseFromString(std::string(getBuffer));
        std::cout<<"receive from client:"<<buf.operation()<<buf.m_message().id()<<buf.m_message().password()<<std::endl;
	/*
        buf.set_str("abcabc");

        std::string sendstr = buf.SerializeAsString();

        //����
        socket.write_some(buffer(sendstr), ec);
 	*/
        // ���������ӡ������Ϣ
        if(ec)
        {
            std::cout << 
                boost::system::system_error(ec).what() << std::endl;
            break;
        }
        // �뵱ǰ�ͻ�������ɺ�ѭ�������ȴ���һ�ͻ�����
    }
    return 0;
}
