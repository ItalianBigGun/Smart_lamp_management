#include "filter.h"
#include <boost/format.hpp>

Filter::Filter()
{
    bool ret;
    ret = init();
    
}

Filter::~Filter()
{
    distroy();
}

//初始化数据库
bool Filter::init()
{
    int ret;
    return initDatabase();
}

void Filter::distroy()
{
    mysql.disconnect_to_Mysql();
    std::cout<<"disconnect to mysql"<<std::endl;
}

//获取socket传来的数据，并验证数据库里面的数据
int Filter::doFilter(my::Node& msgnode, Socket* var)
{
    try{
        int port = var->local_endpoint().port();
        char msg[128] = {0};
        std::string sql;
        my::Message *message;
        if(port==9527){     //如果是9527，说明是用户
            //调用protobuf接口解析
            std::string id, pwd,operation;
            operation = msgnode.operation();
            id = msgnode.m_message().id();
            pwd = msgnode.m_message().password();
            std::cout<<operation<<id.size()<<pwd.size()<<std::endl;
            if(id.size()==0||pwd.size()==0){
                std::cout<<"data is null"<<std::endl;
                message = new my::Message;
                *message = msgnode.m_message();
                message->set_error("failed");
                msgnode.set_allocated_m_message(message);
                return 1;
            }

            //3.如果是登陆，在视图中查询数据;否则，查找数据之后，进行插入账号信息，并更新视图。
            int ret;
            if(operation=="login"){     
                sql = (boost::format("select * from test_account where id=\"%s\" and password=\"%s\";") % id % pwd).str();
                std::cout<<sql<<std::endl;
                ret = mysql.operate_select_only(sql.c_str());
                if(ret){
                    ret = alterUserSocket(id, var);
                    std::cout<<"alter a socket value is:"<<var<<std::endl;
                    //std::cout<<"alter a *socket value is:"<<*var<<std::endl;
                }
                if(ret==-1){
                    initDatabase();
                    ret = mysql.operate_select_only(sql.c_str());
                }
            }else if(operation=="register"){
                //3.1.搜索有没有相同账号
                sql = (boost::format("select * from test_account where id=\"%s\";") % id).str();
                std::cout<<sql<<std::endl;
                ret = mysql.operate_select_only(sql.c_str());
                if(ret==-1){
                    initDatabase();
                    ret = mysql.operate_select_only(sql.c_str());
                }
                //3.2.对返回结果进行判断
                if(ret==false){     //没有相同账号
                //3.2.1.插入对应的账号表
                ret = addAccount(id, pwd);
                //3.2.2.更新账号视图
                ret = reCreateView();
                }else{              //有相同账号
                    ret = false;
                }
            }
            

            //4.操作成功返回socket,否则返回nullptr;
            if(ret){
                message = new my::Message;
                *message = msgnode.m_message();
                message->set_error("");
                msgnode.set_allocated_m_message(message);
                var->send(boost::asio::buffer(msgnode.SerializeAsString()));
                //dolog(msgnode.m_message().id(),msgnode.m_message().SerializeAsString(),msgnode.operation());
                return 0;
            }else{
                message = new my::Message;
                *message = msgnode.m_message();
                message->set_error("failed");
                msgnode.set_allocated_m_message(message);
                var->send(boost::asio::buffer(msgnode.SerializeAsString()));
                return 1;
            }
        }else if(port==9526){   //
            std::string userId,id;
            //
            userId = msgnode.m_message().password();
            id = msgnode.m_message().id();
            sql = (boost::format(
            "select * from test_terminal where id=\"%s\"   \
            and a_id=\"%s\";")% id % userId).str();
            int ret;
            std::cout<<sql<<std::endl;
            ret = mysql.operate_select_only(sql.c_str());
            if(ret){    //有终端数据就保存socket
                ret = alterDevSocket(id,var);
                if(ret){
                    message = new my::Message;
                    *message = msgnode.m_message();
                    message->set_error("");
                    msgnode.set_allocated_m_message(message);

                    return 0;
                }else{
                    message = new my::Message;
                    *message = msgnode.m_message();
                    message->set_error("failed");
                    msgnode.set_allocated_m_message(message);
 
                    return 1;
                }
            }else{      //没有终端数据就添加终端
            ret = addTerminal(id,userId);
            if(ret){
                    message = new my::Message;
                    *message = msgnode.m_message();
                    message->set_error("");
                    msgnode.set_allocated_m_message(message);

                return 0;
            }else{
                    message = new my::Message;
                    *message = msgnode.m_message();
                    message->set_error("failed");
                    msgnode.set_allocated_m_message(message);

                return 1;
            }
            }
        }
    }catch(std::exception &ex)
    {
        return 1;
    }
       
}

bool Filter::initDatabase()
{
    if(mysql.connect_to_Mysql()){
        if(createAccountView()){
        //if(1){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}

bool Filter::createAccountView()
{
    return mysql.operate_Mysql_Modify("create view test_account_view as select * from test_account;");
}

bool Filter::reCreateView(std::string sqlstr)
{
    bool ret;
    ret = this->dropView();
    if(ret){
        std::cout<<"删除成功！"<<std::endl;
        ret = createAccountView();
        if(ret){
            std::cout<<"重建成功！"<<std::endl;
        }
    }
    return ret;
}

bool Filter::dropView()
{
    return mysql.operate_Mysql_Modify("drop view test_account_view;");
}

bool Filter::addAccount(std::string id, std::string pwd)
{
    std::string sqlstr = (boost::format("insert into test_account value(\"%s\",\"%s\",0);") % id % pwd).str();
    std::cout<<sqlstr<<std::endl;
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

bool Filter::addTerminal(std::string id, std::string userId)
{
    std::string sqlstr = (boost::format("insert into test_terminal value(\"%s\",\"%s\",0);") % id % userId).str();
    std::cout<<sqlstr<<std::endl;
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

bool Filter::alterDevSocket(std::string id, Socket *socket)
{
    std::string sqlstr = (boost::format("update test_terminal set status=%d where id = \"%s\";") %socket%id).str();
    std::cout<<sqlstr<<std::endl;
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

bool Filter::alterUserSocket(std::string id, Socket *socket)
{
    std::string sqlstr = (boost::format("update test_account set status = %d where id = \"%s\";") % socket % id).str();
    std::cout<<sqlstr<<std::endl;
    return mysql.operate_Mysql_Modify(sqlstr.c_str());
}

/*
void Filter::dolog(std::string id, std::string message, std::string opt)
{ 
    std::string date = boost::gregorian::to_iso_string(boost::gregorian::day_clock::universal_day());
    std::cout<<date<<endl;
    std::string sqlstr = (boost::format("insert into test_log value(\"%s\",\"%s\",\"%s\",\"%s\");") % id % opt % message % date).str();
}
*/