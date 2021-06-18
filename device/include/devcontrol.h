#ifndef DEVCONTROL_H
#define DEVCONTROL_H

#include <QMainWindow>
#include "test.pb.h"
#include "database.h"
#include "setform.h"
#include <QDebug>
#include <QVector>
#include "digitalclock.h"
//#include "monitoring.h"

namespace Ui {
class devControl;
}

class devControl : public QMainWindow
{
    Q_OBJECT

public:
    explicit devControl(QWidget *parent = nullptr);
    ~devControl();
    void showEvent(QShowEvent *e){
        //qDebug()<<(this->parentWidget()->width()-this->width())*0.5+this->parentWidget()->pos().x();
        //qDebug()<<(this->parentWidget()->height()-this->height())*0.5+this->parentWidget()->pos().y();
        //this->show();
        //this->move(
                    //(this->parentWidget()->width()-this->width())*0.5+this->parentWidget()->pos().x(),
                    //(this->parentWidget()->height()-this->height())*0.5+this->parentWidget()->pos().y()
                    //);
    }
    void clearDeviceName();
    void addDeviceName(QString name);

    bool eventFilter(QObject *watched, QEvent *e);

signals:
    void sendReady(my::Node node);
    void disconnectSignal();

private slots:


private:
    Ui::devControl *ui;
    SetForm *setform = nullptr;
    QVector<QString> devicenames;

};

#endif // DEVCONTROL_H
