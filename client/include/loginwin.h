#ifndef LOGINWIN_H
#define LOGINWIN_H

#include <QMainWindow>
#include <QEvent>
#include "common.h"
#include "led.h"
namespace Ui {
class LoginWin;
}

class LoginWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWin(QWidget *parent = 0);
    ~LoginWin();
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void Created();
signals:
    void SendSignal(my::Node msgnode);

public slots:

private:
    Ui::LoginWin *ui;
    my::Node node;
    //my::Message *message = new my::Message;
    my::Message *message;
};

#endif // LOGINWIN_H
