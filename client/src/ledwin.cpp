#include "ledwin.h"
#include "ui_ledwin.h"
#include <QDebug>


//灯控制窗口
LedWin::LedWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LedWin)
{
    ui->setupUi(this);
    QDesktopWidget *deskdop=QApplication::desktop();
         move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    //UI初始化
    this->setWindowTitle("LedWin");
    this->setProperty("classname","LedWin");
    //this->setFixedSize(this->size());
    //this->showFullScreen();
    ui->backBtn->setImage(":/return.png");
    ui->searchBtn->setImage(":/search.png");
    ui->addBtn->setImage(":/addlight.png");
    ui->deleteBtn->setImage(":/deletelight.png");
    //事件初始化
    ui->searchBtn->installEventFilter(this);
    ui->addBtn->installEventFilter(this);
    ui->deleteBtn->installEventFilter(this);
    ui->backBtn->installEventFilter(this);
}

LedWin::~LedWin()
{
    delete ui;
}

bool LedWin::eventFilter(QObject *watched, QEvent *event)
{
    node.Clear();
    message = new my::Message;
    if(event->type() == QEvent::MouseButtonPress){
        if (watched == ui->searchBtn) {
            qDebug()<<"emit search";
            //设置数据
            node.set_operation("search");
            node.set_allocated_m_message(message);

        }else {
            if(watched == ui->addBtn){
                if(ui->nameEdit->text()==""||ui->ISDNEdit->text()=="")
                {
                    qDebug()<<"please input vaild value";
                    return true;
                }
                node.set_operation("add");
                message = node.mutable_m_message();
                my::DevNode *dev = message->add_dev();
                dev->set_isdn(ui->ISDNEdit->text().toStdString().c_str());
                qDebug()<<"hello:"<<QString(dev->isdn().c_str());
                dev->set_name(ui->nameEdit->text().toStdString().c_str());
                dev->set_status(ui->ISDNEdit->text().toStdString().c_str());
            }else if(watched == ui->deleteBtn){
                node.set_operation("del");
                /*
                if(ui->nameEdit->text()=="")
                {
                    return true;
                }
                node.set_operation("del");
                message = node.mutable_m_message();
                my::DevNode *dev = message->add_dev();
                dev->set_isdn(ui->ISDNEdit->text().toStdString().c_str());
                qDebug()<<"hello:"<<QString(dev->isdn().c_str());
                dev->set_name(ui->nameEdit->text().toStdString().c_str());
                dev->set_status("0");
                */
            }else if(watched == ui->backBtn){
                node.Clear();
                node.set_operation("back");
            }

        }
        //发送信号
        emit SendSignal(node);
        qDebug()<<"ledwin send signal";
        //修改delete按钮样式
        if(watched == ui->deleteBtn){
            if(ui->deleteBtn->getText()=="●"){
                ui->deleteBtn->setText("");
            }else{
                ui->deleteBtn->setText("●");
            }
        }else{
           ui->deleteBtn->setText("");
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

QWidget *LedWin::getLevWidget()
{
    ui->ledWidget->children().size();
    //qDebug()<<;
    return ui->ledWidget;
}

void LedWin::clearLedName()
{
    this->lednames.clear();
}

void LedWin::addLedName(QString name)
{
    this->lednames.append(name);
}

//test...


