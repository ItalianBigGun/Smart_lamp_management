#include "mainwin.h"
#include "ui_mainwin.h"


//主窗口
MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    QDesktopWidget *deskdop=QApplication::desktop();
         move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);

    //
    this->setWindowTitle("MainWin");
    this->setProperty("classname","MainWin");
    this->setFixedSize(this->size());
    ui->backBtn->setImage(":/return.png");

    //
    ui->ledBtn->installEventFilter(this);
    ui->backBtn->installEventFilter(this);
}
MainWin::~MainWin()
{
    delete ui;
}

//事件过滤及发送信号给eventControl
bool MainWin::eventFilter(QObject *watched, QEvent *event)
{
    message = new my::Message;
    if(event->type() == QEvent::MouseButtonPress){
        if (watched == ui->ledBtn) {        //
            //设置数据
            node.set_operation("ledBtn");
            //发送信号
            emit SendSignal(node);
        }else if(watched == ui->backBtn){   //
            //设置数据
            node.set_operation("back");
            //发送信号
            emit SendSignal(node);
        }else if(watched == nullptr){       //
            ;
        }else if(0){
            ;
        }

    }
    return QMainWindow::eventFilter(watched, event);
}
