#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QEvent>
#include "common.h"
namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();
    bool eventFilter(QObject *watched, QEvent *event);
signals:
    void SendSignal(my::Node);

private:
    Ui::MainWin *ui;
    my::Node node;
    //my::Message *message = new my::Message;
    my::Message *message;
};

#endif // MAINWIN_H
