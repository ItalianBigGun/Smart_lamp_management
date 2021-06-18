#ifndef LEDWIN_H
#define LEDWIN_H

#include <QMainWindow>
#include "common.h"
#include <QEvent>
#include <QVector>
namespace Ui {
class LedWin;
}

class LedWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit LedWin(QWidget *parent = 0);
    ~LedWin();
    bool eventFilter(QObject *watched, QEvent *event);
    QWidget *getLevWidget();
    void clearLedName();
    void addLedName(QString name);
signals:
    void SendSignal(my::Node msgnode);

private slots:


private:
    Ui::LedWin *ui;
    my::Node node;
    //my::Message *message = new my::Message;
    my::Message *message;
    QVector<QString> lednames;
};

#endif // LEDWIN_H
