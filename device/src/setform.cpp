#include "setform.h"
#include "ui_setform.h"
#include <QDebug>
#include <QDesktopWidget>
SetForm::SetForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetForm)
{
    ui->setupUi(this);
    QDesktopWidget *deskdop=QApplication::desktop();
     move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint|Qt::Tool|Qt::X11BypassWindowManagerHint);
    //this->setWindowOpacity(0.7);
    setWindowFlags(Qt::Window);
    this->m_base = new DataBase("myDevice.db");
    m_base->getModel()->setTable("device");
    ui->tableView->setModel(m_base->getModel());
    m_base->getModel()->setFilter("type='LED1'");
    m_base->getModel()->select();
}

SetForm::~SetForm()
{
    delete ui;
}


void SetForm::on_comboBox_activated(const QString &arg1)
{
    if(arg1=="LED"){
        qDebug()<<"LED";
        this->m_base->getModel()->setFilter("type like 'LED%'");
        this->m_base->getModel()->select();
    }else if(arg1=="TV"){
        ;
    }else if(arg1==""){

    }
}

void SetForm::on_pushButton_2_clicked()
{
    this->hide();
}

void SetForm::on_pushButton_clicked()
{
    this->m_base->getModel()->setFilter("");
    this->m_base->getModel()->select();
}


void SetForm::on_tableView_doubleClicked(const QModelIndex &index)
{
    qDebug()<<"on_tableView_doubleClicked";
    qDebug()<<index.column();
    if(index.column()==0){
        this->blockSignals(true);
    }else{
        this->blockSignals(false);
    }
}

void SetForm::on_tableView_clicked(const QModelIndex &index)
{

}
