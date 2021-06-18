#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket();
    qDebug()<<"test_lv1.0.0";
    this->setWindowTitle("test_lv1.0.0");
    QDesktopWidget *deskdop=QApplication::desktop();
     move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    leddev = new LedDev();

    DataBase database;
    m_datanode = database.query("select * from device;");
    database.close();
    mainform = new MainForm(this);
    mainform->hide();
    devwin = new devControl(this);
    devwin->hide();
    connect(socket,&QTcpSocket::connected,this,&MainWindow::connected);
    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::recvFrom);
    connect(socket,&QTcpSocket::disconnected,this,&MainWindow::disconnected);
    connect(this,&MainWindow::sendReady,this,&MainWindow::sendTo);
    connect(devwin,&devControl::sendReady,this,&MainWindow::sendTo);
    connect(devwin,&devControl::disconnectSignal,this,&MainWindow::disconnectfromhost);
}

MainWindow::~MainWindow()
{
    disconnect();
    delete socket;
    socket = nullptr;
    delete ui;
}

void MainWindow::connectToServer()
{
    socket->connectToHost(IP,PORT);
}

void MainWindow::disconnectfromhost()
{
    socket->disconnectFromHost();
    ui->bindBtn->setText("bind");
}

void MainWindow::disconnected()
{
    qDebug()<<"server disconnected";
    ui->bindBtn->setText("bind");
}

QTcpSocket *MainWindow::getSocket()
{
    return this->socket;
}

void MainWindow::deviceControl(my::Node node)
{
    std::string name, stutas;
    QString type;
    int n = node.m_message().dev_size();
    for(int i=0;i<n;i++){
        //1.fetch device message
        name = node.m_message().dev(i).name();
        stutas = node.m_message().dev(i).status();
        type = QString(node.m_message().dev(i).isdn().substr(0,node.m_message().dev(i).isdn().find('-')).c_str());
        qDebug()<<"type:"<<type;
        //2.operate device
        if(type=="LED"){
            if(QString(stutas.c_str()).toInt()){
               leddev->openLed(findDeviceType(QString(name.c_str())));
            }else{
               leddev->closeLed(findDeviceType(QString(name.c_str())));
            }
        }else{
            qDebug()<<"non device control message";
        }
    }

    qDebug()<<"device control";
}

//get data from server
void MainWindow::devStuSearch()
{

}

void MainWindow::closeAllDev()
{

}

void MainWindow::initProtobuf(my::Node &node)
{
    node.Clear();
}

//close the all device of terminal
void MainWindow::initTerminal()
{
    leddev->closeLed("");
}

void MainWindow::connected()
{
    qDebug()<<"connected";
}

void MainWindow::recvFrom()
{
    std::string buf;
    QString m_buf;
    std::string error;
    my::Node node;
    std::string operation;
    //

    //get protobuf
    m_buf = socket->readAll();
    node.ParseFromString(m_buf.toStdString());

    //yanzheng
    error = node.m_message().error();
    qDebug()<<"error:"<<QString(error.c_str());
    if(error==""){
        qDebug()<<m_buf;
    }else{
        //error tip
        return;
    }

    //decode protobuf
    operation = node.operation();
    qDebug()<<"operation:"<<operation.c_str();

    if(operation=="control"){
        deviceControl(node);
        qDebug()<<"get device size is:"<<node.m_message().dev_size();
        qDebug()<<QString(node.m_message().dev(0).name().c_str())
               <<QString(node.m_message().dev(0).status().c_str());
        //get control device result and set protobuf
        node.mutable_m_message()->set_error("sueecss");
        qDebug()<<"get device size is:"<<node.m_message().dev_size();
        this->socket->write(node.SerializeAsString().c_str());
        qDebug()<<"control and call back";
    }else if(operation=="search"){
        deviceControl(node);
    }else if(operation=="login"){
        this->hide();
        devwin->show();
        ui->bindBtn->setText("enter");
    }
    //device control operate
    //deviceControl(node);
}

void MainWindow::sendTo(my::Node node)
{
    std::string buf;
    QString m_buf;

    buf.clear();
    buf = node.SerializeAsString();
    //protobuf object make

    socket->write(buf.c_str());
}

/***************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ***************************/

void MainWindow::on_bindBtn_clicked()
{
    qDebug()<<"bind";
    if(this->socket->isValid()&&devwin!=nullptr){
        this->hide();
        devwin->show();
    }
    this->id = ui->idEdit->text();
    QString userId = ui->userEdit->text();
    connectToServer();
    initProtobuf(m_node);
    m_node.set_operation("login");
    m_node.mutable_m_message()->set_id(this->id.toStdString().c_str());
    m_node.mutable_m_message()->set_password(userId.toStdString().c_str());
    sendReady(m_node);
    qDebug()<<"bind";
}
