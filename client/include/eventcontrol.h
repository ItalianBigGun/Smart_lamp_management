#ifndef EVENTCONTROL_H
#define EVENTCONTROL_H

#include <QObject>
#include <QMainWindow>
#include <QTcpSocket>
#include <QCloseEvent>
#include <QMessageBox>
#include <QThread>
#include <QVector>
#include <QGridLayout>

#include "dataanalyst.h"
#include "loginwin.h"
#include "ledwin.h"
#include "mainwin.h"
#include "common.h"
#include "led.h"

//#define IP "192.168.2.194"
#define IP "47.107.184.34"
namespace Ui {
class EventControl;
}

class EventControl : public QMainWindow,DataAnalyst
{
    Q_OBJECT

//共有成员函数
public:
    explicit EventControl(QWidget *parent = 0);
    ~EventControl();

    virtual bool analyze(my::Node msgnode);

//共有成员
public:
    QTcpSocket *socket = nullptr;
    LedWin *ledwin = nullptr;
    LoginWin *loginwin = nullptr;
    MainWin *mainwin = nullptr;

signals:
    void SendSignal(my::Node node);
//共有槽函数
public slots:
    void showWin();
    void GetSignal(my::Node node);
    void recvFrom();

private:
    Ui::EventControl *ui;
    QString username = nullptr, pwd = nullptr;
    my::Node node;
    my::Message message;
    QMessageBox *mbox;
    LED *led;
    QVector<LED*> ledVector;
    QGridLayout *grid;

    bool deleteflag = false;
};

class WinControl:public QObject
{
    Q_OBJECT

public:
    WinControl(QObject *parent = 0);
    ~WinControl();

public:

    template<typename T1 = MainWin,typename T2  = MainWin> static void showPrev(T1 *arg1=nullptr, T2 *arg2=nullptr);

    template<typename T1  = MainWin,typename T2  = MainWin> static void showNext(T1 *arg1, T2 *arg2=nullptr);

    template<typename T> static void showByParent(T *);

    static void insertWidget(QWidget *parent, LED *child);



};

#endif // EVENTCONTROL_H
