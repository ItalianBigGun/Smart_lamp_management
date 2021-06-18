#include "led.h"
#include "ui_led.h"

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
