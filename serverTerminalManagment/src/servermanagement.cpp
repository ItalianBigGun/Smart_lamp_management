#include <iostream>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <stdio.h>
#include "Sql.h"

struct devnode{
    std::string devname;
    std::string isdn;
    std::string stutas;
};

struct usrnode{
    std::string usrname;
    std::string tername;
    struct devnode *devices;
    unsigned int devlen;
};

class ServerManagement{

public:
    ServerManagement(
        const char* host = "127.0.0.1", 
        const char* user = "root", 
        const char* pwd = "123456"){
        mysql.initMysqlInfo(host,user,pwd);
        if(mysql.connect_to_Mysql()){
            //std::cout<<""<<std::endl;
        }
    }
    ~ServerManagement(){
        mysql.disconnect_to_Mysql();
    }

    std::string *showallaccount(int &size);
    struct usrnode* showaccountdetail(std::string usrname);
    bool deleteaccount(std::string usrname);
    bool alteraccountpwd(std::string usrname, std::string pwd);
    void accountView();

    void terminalView();

    void deviceView();

    void isdnView();

    bool loginverify();
    bool alterpassword();
    bool addSuperAccount();
    void superView();

    void mainView();

private:
    SQL mysql;
    std::string usrname;
    std::string pwd;
};
/***********窗口功能*************/

/***********账号窗口功能*************/
std::string* ServerManagement::showallaccount(int &size){
    MYSQL_RES* result;
    int datasize;
    MYSQL_ROW row;
    std::string sqlstr;
    std::string *accounts;
    result = mysql.operate_Query_Result("select id from test_account;");
    if(result!=nullptr){
            datasize = mysql_num_rows(result);
            size = datasize;
            accounts = new std::string[datasize];
            row = mysql_fetch_row(result);
            while (nullptr != row){
                if(datasize--){
                    accounts[datasize].clear();
                    accounts[datasize].append(row[0]);
                }
                row = mysql_fetch_row(result);
            }
            mysql_free_result(result);
            return accounts;
        }else{
            mysql_free_result(result);
            return nullptr;
        }
}
struct usrnode* ServerManagement::showaccountdetail(std::string usrname){
    MYSQL_RES* result;
    int datasize;
    MYSQL_ROW row;
    std::string sqlstr;
    struct usrnode *usrmsg = new struct usrnode;
    if(usrmsg==nullptr){
        return nullptr;
    }
    try{
        sqlstr = (boost::format(
        "select test_account.id,test_terminal.id as t_id, \
            test_device.name,test_device.ISDN,test_device.status \
                from test_account,test_terminal,test_device \
                    where test_account.id =\"%s\";") % usrname.c_str()).str();
        result = mysql.operate_Query_Result(sqlstr.c_str());
        if(result!=nullptr){
            datasize = mysql_num_rows(result);
            if(datasize==0){
                std::cout<<"用户名为空!"<<std::endl;
                mysql_free_result(result);
                return nullptr;
            }else{
                usrmsg->devlen = datasize;  //用户和终端只能由一个,单设备可以有一至多个
            }
            row = mysql_fetch_row(result);
            if(row!=nullptr){   //保存用户和设备名
                usrmsg->usrname.clear();
                usrmsg->usrname.append(row[0]);
                usrmsg->tername.clear();
                usrmsg->tername.append(row[1]);
                //初始化设备节点
                usrmsg->devices = new devnode[usrmsg->devlen];
            }
            while (nullptr != row){
                if(datasize--){
                    usrmsg->devices[datasize].devname.clear();
                    usrmsg->devices[datasize].isdn.clear();
                    usrmsg->devices[datasize].stutas.clear();
                    usrmsg->devices[datasize].devname.append(row[2]);
                    usrmsg->devices[datasize].isdn.append(row[3]);
                    usrmsg->devices[datasize].stutas.append(row[4]);
                }
                row = mysql_fetch_row(result);
            }
            mysql_free_result(result);
            return usrmsg;
        }else{
            mysql_free_result(result);
            return nullptr;
        }
    }catch(std::exception &ex){
        std::cout<<ex.what()<<std::endl;
        return nullptr;
    }
    
}
bool ServerManagement::deleteaccount(std::string usrname){
    std::string sqlstr;
    sqlstr = (boost::format(
        "delete from test_account where id =\"%s\";") % usrname.c_str()).str();
    bool ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
    return ret;
}
bool ServerManagement::alteraccountpwd(std::string usrname, std::string pwd){
    std::string sqlstr;
    sqlstr = (boost::format(
        "update test_account set password=\"%s\" where id =\"%s\";"
        ) % pwd.c_str() % usrname.c_str()).str();
    bool ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
    return ret;
}
void ServerManagement::accountView(){
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"accountView"<<std::endl;
        printf("1.查看用户信息\n2.删除用户\n3.修改用户密码\n4.查看所有用户\n(e&E.exit)\n");
        printf("请输入选项:\n");
        c = getchar();
        while((getchar())!='\n');     //获取回车符
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){     
            //
            std::string usrname;
            std::cout<<"请输入用户名:"<<std::endl;
            std::cin>>usrname;
            getchar();
            struct usrnode *usrmsg = showaccountdetail(usrname);
            if(usrmsg!=nullptr){
                std::cout<<"用户名:"<<usrmsg->usrname<<"终端名:"<<usrmsg->tername<<std::endl;
                std::cout<<"设备信息:"<<std::endl;
                for(int i=0;i<usrmsg->devlen;++i){
                    std::cout<<"设备名:"<<usrmsg->devices[i].devname<<"ISDN:"<<usrmsg->devices[i].isdn<<"状态:"<<usrmsg->devices[i].stutas<<std::endl;
                }
                delete[] usrmsg->devices;
            }else{
            }
            getchar();
        }else if(c=='2'){
            std::string usrname;
            std::cout<<"请输入用户名:"<<std::endl;
            std::cin>>usrname;
            getchar();
            int ret = deleteaccount(usrname);
            if(ret){
                std::cout<<"删除成功!"<<std::endl;
            }else{
                std::cout<<"删除失败!"<<std::endl;
            }
            getchar();
        }else if(c=='3'){
            std::string usrname,pwd;
            std::cout<<"请输入用户名:"<<std::endl;
            std::cin>>usrname;
            std::cout<<"请输入新密码:"<<std::endl;
            std::cin>>pwd;
            getchar();
            bool ret = alteraccountpwd(usrname,pwd);
            if(ret){
                std::cout<<"修改成功!"<<std::endl;
            }else{
                std::cout<<"修改失败!"<<std::endl;
            }
            getchar();
        }else if(c=='4'){
            int size;
            std::string *accounts = showallaccount(size);
            if(accounts!=nullptr){
                std::cout<<"所有用户:"<<std::endl;
                for(int i=0;i<size;++i){
                    std::cout<<i+1<<":"<<accounts[i]<<std::endl;
                }
            }
            getchar();
        }
    }
}
/***********终端窗口功能*************/
void ServerManagement::terminalView(){
    
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"terminalView"<<std::endl;
        printf("1.查看所有终端\n2.删除终端\n3.demo\n4.demo\n(e&E.exit)\n");
        printf("请输入选项:\n");
        c = getchar();
        while((getchar())!='\n');     //获取回车符
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){     
            MYSQL_RES* result;
            int datasize;
            MYSQL_ROW row;
            result = mysql.operate_Query_Result("select id from test_terminal;");
            if(result!=nullptr){
                datasize = mysql_num_rows(result);
                if(datasize==0){
                    std::cout<<"终端为空!"<<std::endl;
                    mysql_free_result(result);
                }else{
                    std::cout<<"终端名:"<<std::endl;
                    row = mysql_fetch_row(result);
                    while (nullptr != row){
                        if(datasize--){
                            std::cout<<row[0]<<std::endl;
                        }
                        row = mysql_fetch_row(result);
                    }
                }
                mysql_free_result(result);
            }
            getchar();
        }else if(c=='2'){
            std::string tername;
            std::cout<<"请输入终端名:"<<std::endl;
            std::cin>>tername;
            int ret = mysql.operate_Mysql_Modify((boost::format(
            "delete from test_terminal where id =\"%s\";"
            ) % tername.c_str()).str().c_str());
            if(ret){
                std::cout<<"删除成功!"<<std::endl;
            }else{
                std::cout<<"删除失败!"<<std::endl;
            }
            getchar();
        }else if(c=='3'){
            ;
        }else if(c=='4'){

        }
        
    }
}
/***********设备窗口功能*************/
void ServerManagement::deviceView(){
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"deviceView"<<std::endl;
        printf("1.demo\n2.demo\n3.demo\n4.demo\n(e&E.exit)\n");
        printf("请输入选项:\n");
        c = getchar();
        while((getchar())!='\n');     //获取回车符
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){     
            ;
        }else if(c=='2'){
            ;
        }else if(c=='3'){
            ;
        }else if(c=='4'){

        }
        //while((c=getchar())!='\n');     //获取回车符
    }
}

/***********isdn窗口功能*************/
void ServerManagement::isdnView(){
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"isdnView"<<std::endl;
        printf("1.查看所有ISDN\n2.添加ISDN\n3.删除ISDN\n4.demo\n(e&E.exit)\n");
        printf("请输入选项:\n");
        c = getchar();
        while((getchar())!='\n');     //获取回车符
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){     
            MYSQL_RES* result;
            int datasize;
            MYSQL_ROW row;
            result = mysql.operate_Query_Result("select name from test_isdn;");
        if(result!=nullptr){
            datasize = mysql_num_rows(result);
            if(datasize==0){
                std::cout<<"ISDN为空!"<<std::endl;
                mysql_free_result(result);
            }else{
                std::cout<<"ISDN:"<<std::endl;
                row = mysql_fetch_row(result);
                while (nullptr != row){
                    if(datasize--){
                        std::cout<<row[0]<<":"<<row[1]<<std::endl;
                    }
                    row = mysql_fetch_row(result);
                }
            }
            mysql_free_result(result);
            }
            getchar();
        }else if(c=='2'){
            std::string isdn;
            std::cout<<"请输入ISDN:"<<std::endl;
            std::cin>>isdn;
            int ret = mysql.operate_Mysql_Modify((boost::format(
            "insert into test_isdn(name) value(\"%s\");"
            ) % isdn.c_str()).str().c_str());
            if(ret){
                std::cout<<"添加成功!"<<std::endl;
            }else{
                std::cout<<"添加失败!"<<std::endl;
            }
            getchar();
        }else if(c=='3'){
            std::string isdn;
            std::cout<<"请输入ISDN:"<<std::endl;
            std::cin>>isdn;
            int ret = mysql.operate_Mysql_Modify((boost::format(
            "delete from test_isdn where name=\"%s\";"
            ) % isdn.c_str()).str().c_str());
            if(ret){
                std::cout<<"删除成功!"<<std::endl;
            }else{
                std::cout<<"删除失败!"<<std::endl;
            }
            getchar();
        }else if(c=='4'){

        }
        //while((c=getchar())!='\n');     //获取回车符
    }
}

/***********超管窗口功能*************/
void ServerManagement::superView(){
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"superView"<<std::endl;
        printf("1.添加超级管理员账号\n2.删除超级管理员账号\n3.修改密码\n4.demo\n(e&E.exit)\n");
        printf("请输入选项:\n");
        c = getchar();
        while((getchar())!='\n');     //获取回车符
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){     
            addSuperAccount();
        }else if(c=='2'){
            ;
        }else if(c=='3'){
            alterpassword();
        }else if(c=='4'){

        }
        //while((c=getchar())!='\n');     //获取回车符
    }
}
/***********主窗口功能*************/
void ServerManagement::mainView(){
    char c = {0};
    while(1){
        system("clear");
        std::cout<<"mainView"<<std::endl;
        printf("1.accountView\n2.terminalView\n3.deviceView\n4.isdnView\n5.superView\n(e&E.exit)\n");
        printf("请输入选项:\n");
        std::cin>>c;
        if(c=='e'||c=='E'){
            break;
        }
        if(c=='1'){         //账号窗口
            accountView();
        }else if(c=='2'){   //终端窗口
            terminalView();
        }else if(c=='3'){   //设备窗口
            deviceView();
        }else if(c=='4'){   //ISDN窗口
            isdnView();
        }else if(c=='5'){   //超管账号窗口
            superView();    
        }else{
            std::cout<<"输入有误"<<std::endl;
        }
        //while((c=getchar())!='\n');     //获取回车符
    }
}

/***********功能*************/  
bool ServerManagement::loginverify(){
    char c = {0};
    char usrname[64] = {0};
    char pwd[64] = {0};
    std::string sqlstr;
    while(1){
        system("clear");
        std::cout<<"请输入账号:"<<std::endl;
        std::cin>>usrname;
        std::cout<<"请输入密码:"<<std::endl;
        std::cin>>pwd;
        std::cout<<"输入为:"<<usrname<<pwd<<std::endl;
        if(usrname[0]=='e'||usrname[0]=='E'){
            return false;
        }
        getchar();
        getchar();
        //验证
        if(usrname[0]!='\n'||usrname[0]!='\0'){
            int ret;
            sqlstr = (boost::format(
            "select * from test_admin where id=\"%s\" and password=\"%s\";"
            ) % usrname % pwd).str();

            ret = this->mysql.operate_select_only(sqlstr.c_str());
            if(ret){
                this->usrname.clear();
                this->usrname.append(usrname);
                std::cout<<"sueecss!"<<std::endl;
                getchar();
                return true;
            }else{
                std::cout<<"failed!"<<std::endl;
                getchar();
            }
        }else{
            continue;
        } 
    }
}

bool ServerManagement::alterpassword(){
    std::string pwd,_pwd;
    std::cout<<"请输入新的密码:"<<std::endl;
    std::cin>>pwd;
    std::cout<<"再次输入新密码:"<<std::endl;
    std::cin>>_pwd;
    getchar();
    if(pwd==_pwd){
        int ret;
        std::string sqlstr;
        sqlstr = (boost::format(
            "update test_admin set password=\"%s\" where id=\"%s\";"
            ) % pwd % this->usrname).str();

        ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
        if(ret){
            this->pwd.clear();
            this->pwd.append(pwd);
            std::cout<<"修改成功!"<<std::endl;
            getchar();
        }
    }else{
        std::cout<<"输入有误!"<<std::endl;
        getchar();
    }
}

bool ServerManagement::addSuperAccount(){
    std::string usrname, pwd;
    std::cout<<"请输入账号:"<<std::endl;
    std::cin>>usrname;
    std::cout<<"请输入密码:"<<std::endl;
    std::cin>>pwd;
    getchar();
    if(usrname.size()){
        int ret;
        std::string sqlstr;
        sqlstr = (boost::format(
            "insert into test_admin value(\"%s\",\"%s\");"
            ) % usrname.c_str() % pwd.c_str()).str();
        ret = mysql.operate_Mysql_Modify(sqlstr.c_str());
        if(ret){
            getchar();
        }else{
            getchar();
        }
    }else{

    } 
}



int main()
{
    printf("\033[0;31;40m\n");
    system("clear");
    int ret;
    std::string host,usr,pwd;
    std::cout<<"请输入数据库的主机、用户和密码:"<<std::endl;
    std::cout<<"主机:"<<std::endl;std::cin>>host;
    std::cout<<"用户:"<<std::endl;std::cin>>usr;
    std::cout<<"密码:"<<std::endl;std::cin>>pwd;
    getchar();
    ServerManagement s_mm(host.c_str(),usr.c_str(),pwd.c_str());
    getchar();
    
    while(!s_mm.loginverify());      //登陆

    s_mm.mainView();

    printf("\033[0m\n");
    system("clear");
    return 0;
}
