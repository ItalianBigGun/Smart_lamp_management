#include "loginwin.h"
#include "ui_loginwin.h"
#include <QDebug>

//login windows
LoginWin::LoginWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWin)
{
    //UI设置
    ui->setupUi(this);
    QDesktopWidget *deskdop=QApplication::desktop();
         move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    this->setWindowTitle("LoginWin");
    this->setProperty("classname","LoginWin");
    this->setFixedSize(this->size());

    //设置事件过滤
    ui->exitBtn->installEventFilter(this);
    ui->loginBtn->installEventFilter(this);
    ui->registerBtn->installEventFilter(this);

    qDebug()<<"creating";
    this->show();
}

LoginWin::~LoginWin()
{
    delete ui;
}

//事件过滤及发送信号给eventControl
bool LoginWin::eventFilter(QObject *watched, QEvent *event)
{
    my::Node node;
    my::Message *message;
    QString username,pwd;
    username = ui->userlineEdit->text();
    pwd = ui->pwdlineEdit->text();
    if(event->type() == QEvent::MouseButtonPress){
        if (watched == ui->exitBtn) {
            //设置数据
            node.set_operation("exit");
            //发送信号

            emit SendSignal(node);
        }else if(watched == ui->loginBtn){
            //设置数据
            node.set_operation("login");
            message = node.mutable_m_message();
            message->set_id(username.toStdString().c_str());
            message->set_password(pwd.toStdString().c_str());
            //发送信号
            emit SendSignal(node);
        }else if(watched == ui->registerBtn){
            node.set_operation("register");
            message = node.mutable_m_message();
            message->set_id(username.toStdString().c_str());
            message->set_password(pwd.toStdString().c_str());

            emit SendSignal(node);
        }else{
            node.set_operation("nullptr");
            emit SendSignal(node);
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
