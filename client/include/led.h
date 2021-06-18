#ifndef LED_H
#define LED_H

#include <QWidget>
#include <QPushButton>
#include "animationbutton.h"
#include "test.pb.h"

namespace Ui {
class LED;
}

class LED : public AnimationButton
{
    Q_OBJECT

public:
    explicit LED(QWidget *parent = 0);
    ~LED();

    void mousePressEvent(QMouseEvent *event);   //点击事件重写


public:
    Ui::LED *ui;
signals:
    void SendSignal(my::Node);
private slots:

};

#endif // LED_H
