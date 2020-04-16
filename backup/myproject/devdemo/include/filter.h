#ifndef __FILTER_H__
#define __FILTER_H__

#include "Sql.h"
#include "test.pb.h"
#include <boost/asio.hpp>
#include <boost/date_time/gregorian/gregorian.hpp> 
#include<boost/date_time/posix_time/posix_time.hpp>
#include <string> 

using Socket = boost::asio::ip::tcp::socket;

class Filter
{
    public:
        Filter();
        ~Filter();

        bool init();
        void distroy();
        Socket* doFilter(Socket *var = nullptr);

        //视图操作
        bool createAccountView();
        bool reCreateView(std::string sqlstr="");
        bool dropView();

        //账号表操作
        bool addAccount(std::string id, std::string pwd);
        bool alterUserSocket(std::string id, Socket *socket);
        
        //终端表操作
        bool alterDevSocket(std::string id, Socket *socket);

        //
        //void dolog(std::string id, std::string message, std::string date);

    private:
        bool initDatabase();
    private:
        Socket* var = nullptr;
        SQL mysql;
        my::Node msgnode;
        my::Message *message;
        std::string sqlstr = "create view test_account_view as select * from test_account;";
};












#endif