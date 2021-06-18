#ifndef LED_H
#define LED_H

#include <QJsonObject>
#include <QWidget>
#include <QPushButton>
#include "animationbutton.h"

namespace Ui {
class LED;
}

class LED : public AnimationButton
{
    Q_OBJECT

public:
    explicit LED(QWidget *parent = 0);
    ~LED();

public:
    Ui::LED *ui;
signals:
    void sendDevSig();
private slots:

};

#endif // LED_H
