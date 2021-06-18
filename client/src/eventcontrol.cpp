#include "eventcontrol.h"
#include "ui_eventcontrol.h"
#include <QDebug>


EventControl::EventControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EventControl)
{
    ui->setupUi(this);
    this->setWindowTitle("eventControl");
    this->setProperty("classname","EventControl");

    mbox = new QMessageBox(
                "warn",
                QString(""),
                QMessageBox::NoIcon,
                QMessageBox::NoButton,
                QMessageBox::NoButton,
                QMessageBox::NoButton
                );
    //登陆窗口创建及相关信号槽函数绑定
    loginwin = new LoginWin(this);
    connect(loginwin,SIGNAL(SendSignal(my::Node)),this,SLOT(GetSignal(my::Node)));
    connect(this,SIGNAL(SendSignal(my::Node)),this,SLOT(GetSignal(my::Node)));
}

EventControl::~EventControl()
{
    qDebug()<<"exit";
    delete ui;
}

bool EventControl::analyze(my::Node msgnode)
{
    QString error = QString(msgnode.m_message().error().c_str());
    qDebug()<<"error:"<<error;
    if(error==""){
        return true;
    }else{
        return false;
    }
    //return true;
}

void EventControl::showWin()
{
    qDebug()<<"showWin";
    QObject *obj = static_cast<QObject*>(sender());
    qDebug()<<obj<<"=="<<loginwin<<"?";
}



//做子窗口事件触发相关处理
void EventControl::GetSignal(my::Node node)
{
    QString classname = (sender()->property("classname")).toString();
    qDebug()<<"classname:"<<classname;
    my::Message *message;
    QThread::msleep(200);
    std::string opt = node.operation();
    qDebug()<<"getData opt is:"<<QString(opt.c_str());
    //qDebug()<<"protobuf size is:"<<node.SerializeAsString().size();

    if(opt=="login"){           //登陆操作
        if(socket!=nullptr){
            delete  socket;
            socket = nullptr;
        }
        this->username = QString(node.m_message().id().c_str());
        this->pwd = QString(node.m_message().password().c_str());
        //创建套接字
        this->socket = new QTcpSocket();
        connect(socket,&QTcpSocket::readyRead,this,&EventControl::recvFrom);
        this->socket->connectToHost(IP,9527);
        //发送
        socket->write(node.SerializeAsString().c_str());

    }else if(opt=="register"){  //
        if(socket!=nullptr){
            delete  socket;
            socket = nullptr;
        }
        this->socket = new QTcpSocket();
        connect(this->socket,&QTcpSocket::readyRead,this,&EventControl::recvFrom);
        this->socket->connectToHost(IP,9527);
        //发送
        this->socket->write(node.SerializeAsString().c_str());
    }else if(opt=="exit"){      //
        if(this->socket!=nullptr){
            disconnect(this->socket,&QTcpSocket::readyRead,this,&EventControl::recvFrom);
            delete this->socket;
            this->socket = nullptr;
        }
        this->close();
    }else if(opt=="search"){    //
        deleteflag = false;
        if(this->username!=nullptr && this->pwd != nullptr){
            message = node.mutable_m_message();
            message->set_id(this->username.toStdString().c_str());
            message->set_password(this->pwd.toStdString().c_str());
            //发送
            socket->write(node.SerializeAsString().c_str());
        }
    }else if(opt=="del"){       //
        if(this->deleteflag){
            this->deleteflag = false;
        }else{
            this->deleteflag = true;
        }

        /*
        if(this->username!=nullptr && this->pwd != nullptr){    //
            message = node.mutable_m_message();
            message->set_id(this->username.toStdString().c_str());
            message->set_password(this->pwd.toStdString().c_str());
            //发送
            socket->write(node.SerializeAsString().c_str());
        }
        */
    }else if(opt=="add"){       //
        deleteflag = false;
        if(this->username!=nullptr && this->pwd != nullptr){    //
            message = node.mutable_m_message();
            message->set_id(this->username.toStdString().c_str());
            message->set_password(this->pwd.toStdString().c_str());
            qDebug()<<"the ISDN of sent data is:"<<QString(node.m_message().dev(0).isdn().c_str());
            //发送
            socket->write(node.SerializeAsString().c_str());
        }
    }else if(opt=="ledBtn"){
        WinControl::showNext<MainWin,LedWin>(mainwin,ledwin);
        //ledwin->show();
    }else if(opt=="control"){
        if(deleteflag){
            node.set_operation("del");
        }
        message = node.mutable_m_message();
        message->set_id(this->username.toStdString().c_str());
        //message->set_password(this->pwd.toStdString().c_str());
        qDebug()<<"status value is:"<<QString(node.mutable_m_message()->dev(0).status().c_str());
        socket->write(node.SerializeAsString().c_str());
    }else if(opt=="back"){
        deleteflag = false;
        qDebug()<<"back";
        if(classname=="MainWin"){
            WinControl::showPrev<MainWin,LoginWin>(this->mainwin,nullptr);
        }else if(classname=="LoginWin"){
            WinControl::showPrev<LoginWin,MainWin>(this->loginwin,nullptr);
        }else if(classname=="LedWin"){
            WinControl::showPrev<LedWin,MainWin>(this->ledwin,nullptr);
        }
    }
}

//recv data from service
void EventControl::recvFrom()
{
    static int i = 0;
    i++;
    qDebug()<<"recv time is"<<i;
    //1.recv data
    my::Node node;
    if(this->socket!=nullptr){
        node.ParseFromString(QString(this->socket->readAll()).toStdString().c_str());
    }else {
        //node.mutable_m_message()->set_error();
    }
    //2.analyze data
    bool ret;
    ret = this->analyze(node);
    if(ret==false){
        //prevent procedure exit
        QApplication::setQuitOnLastWindowClosed(false);
        mbox->setText(QString("%1 failed!").arg(node.m_message().error().c_str()));
        mbox->show();
        mbox->exec();
        if(mbox->clickedButton())
            mbox->hide();
        QApplication::setQuitOnLastWindowClosed(true);
        return ;
    }else{
        qDebug()<<"analyze sueecc";
    }

    //3.change win
    QString opt = QString(node.operation().c_str());
    //login operation is to change loginwin to mainwin
    if(opt=="login"){

        if(mainwin==nullptr){
            mainwin = new MainWin(loginwin);
            //WinControl::showByParent<MainWin>(mainwin);
            mainwin->show();
            if(ledwin==nullptr){
                ledwin = new LedWin(mainwin);
                grid = new QGridLayout(ledwin->getLevWidget());
                //WinControl::showByParent<LedWin>(ledwin);
                //ledwin->show();
                //ledwin->showFullScreen();
                connect(ledwin,SIGNAL(SendSignal(my::Node)),this,SLOT(GetSignal(my::Node)));
                connect(mainwin,SIGNAL(SendSignal(my::Node)),this,SLOT(GetSignal(my::Node)));
            }
        }
        if(mainwin!=nullptr){
            WinControl::showNext<LoginWin,MainWin>(loginwin,mainwin);
        }
    }else if(opt=="register"){      //register operation is delete the socket
        if(this->socket!=nullptr){
            disconnect(this->socket,&QTcpSocket::readyRead,this,&EventControl::recvFrom);
            delete this->socket;
            this->socket = nullptr;
        }
    }
    //search,add,del operation is to fresh UI
    else if(opt=="search"){
        qDebug()<<"do seach opt";
        int size = node.m_message().dev_size();
        qDebug()<<size;
        if(size==0){
            node.Clear();
            node.set_operation("search");
            node.mutable_m_message()->set_id(this->username.toStdString().c_str());
            emit SendSignal(node);
            return ;
        }
        for(int i=0;i<ledVector.size();++i){
            grid->removeWidget(ledVector.at(i));
            delete ledVector.at(i);
        }
        ledVector.clear();
        ledwin->clearLedName();
        qDebug()<<"led vector size is:"<<ledVector.size();

        for(int i = 0;i<size;++i){
            led = new LED();
            bool status = QString(node.m_message().dev(i).status().c_str()).toInt();
            QString name = QString(node.m_message().dev(i).name().c_str());
            led->setProperty("classname","LED");
            led->setImage(status?":/onlight.png":":/offlight.png");
            led->setText(name);
            ledwin->addLedName(name);
            led->setProperty("status",status);
            ledVector.append(led);
            grid->addWidget(led,0,i,0,1);
            connect(led,SIGNAL(SendSignal(my::Node)),this,SLOT(GetSignal(my::Node)));
            qDebug()<<"led vector size is:"<<ledVector.size();
        }
        //qDebug()<<QString(node.SerializeAsString().c_str());
    }else if (opt=="add") {
        ;   //close add window
    }else if (opt=="del") {
        ;   //close delete window
    }
}

template<typename T1, typename T2>
void WinControl::showPrev(T1 *arg1, T2 *arg2)
{
    if(arg1!=nullptr){
        arg1->hide();
        if(arg2==nullptr){
            arg1->parentWidget()->show();
        }else{
            arg2->show();
        }
    }
}

template<typename T1,typename T2>
void WinControl::showNext(T1 *arg1, T2 *arg2)
{
    if(arg1!=nullptr){
        arg1->hide();
        if(arg2!=nullptr){
            arg2->show();
        }
    }
}

WinControl::WinControl(QObject *parent)
{

}

WinControl::~WinControl()
{

}


void WinControl::insertWidget(QWidget *parent, LED *child)
{
    child = new LED(parent);
}

template<typename T>
void WinControl::showByParent(T *arg)
{
    if(arg!=nullptr){
        arg->setGeometry(
                    arg->parentWidget()->geometry().x(),
                    arg->parentWidget()->geometry().y(),
                    arg->geometry().width(),
                    arg->geometry().height()
                    );
    }
}
