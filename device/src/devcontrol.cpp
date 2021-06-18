#include "devcontrol.h"
#include "ui_devcontrol.h"
#include <QDebug>
#include <QDesktopWidget>
devControl::devControl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::devControl)
{
    ui->setupUi(this);
    QDesktopWidget *deskdop=QApplication::desktop();
     move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    ui->widget_3->setImage(":/onlight.png");
    ui->widget_4->setImage(":/onlight.png");
    ui->widget_5->setImage(":/onlight.png");
    ui->widget_6->setImage(":/onlight.png");
    ui->backBtn->setImage(":/return.png");
    ui->setBtn->setImage(":/setlight.png");
    ui->offBtn->setImage(":/offline.png");
    ui->controlBtn->setImage(":/off.png");

    ui->backBtn->installEventFilter(this);
    ui->setBtn->installEventFilter(this);
    ui->offBtn->installEventFilter(this);
    ui->controlBtn->installEventFilter(this);
}

devControl::~devControl()
{
    delete ui;
}

void devControl::clearDeviceName()
{
    this->devicenames.clear();
}

void devControl::addDeviceName(QString name)
{
    this->devicenames.append(name);
}

bool devControl::eventFilter(QObject *watched, QEvent *e)
{
    if(e->type()==QEvent::MouseButtonPress){
        qDebug()<<"mouse press";
        if(watched == ui->setBtn){
            if(setform==nullptr)
                setform = new SetForm(this);
            setform->show();
        }else if(watched == ui->offBtn){
            emit disconnectSignal();
        }else if(watched == ui->backBtn){
            this->hide();
            this->parentWidget()->show();
        }else if(watched == ui->controlBtn){
            ;
        }else{
            ;
        }
    }

    return false;
}

/*

void devControl::on_controlBtn_clicked()
{
    my::Node node;
    node.Clear();
    my::Message *m;
    my::DevNode *dev;

    //control device

    //if completed, set data to protobuf object

    //send protobuf to serivce
    emit sendReady(node);
}

void devControl::on_setBtn_clicked()
{
    //the show condition of setform is know your admin
    if(setform==nullptr)
        setform = new SetForm(this);
    setform->show();
    qDebug()<<setform->parentWidget()->pos().x()<<setform->parentWidget()->pos().y();
    setform->move(
                setform->parentWidget()->pos().x()*1.5,
                setform->parentWidget()->pos().y()*1.5);
}

void devControl::on_backBtn_clicked()
{
    this->hide();
    this->parentWidget()->show();
}

void devControl::on_offlineBtn_clicked()
{
    emit disconnectSignal();
}
*/
