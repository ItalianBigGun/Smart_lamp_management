#include "led.h"
#include "ui_led.h"
#include <QDebug>

LED::LED(QWidget *parent) :
    AnimationButton(parent),
    ui(new Ui::LED)
{
    ui->setupUi(this);
    this->show();
}

LED::~LED()
{
    delete ui;
}

void LED::mousePressEvent(QMouseEvent *event)
{
    my::Node m_node;

    bool status = 0;

    status = this->property("status").toBool();

    m_node.set_operation("control");
    my::DevNode *m_dev = m_node.mutable_m_message()->add_dev();
    m_dev->set_name(this->getText().toStdString().c_str());
    m_dev->set_status(status?"0":"1");
    m_dev->set_isdn("LED-A001");
    qDebug()<<"status value is:"<<m_dev->status().c_str();

    emit SendSignal(m_node);
}
