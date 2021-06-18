#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "test.pb.h"
#include "leddev.h"
#include "mainform.h"
#include "devcontrol.h"
#include "database.h"

#define PORT 9526
//#define IP "192.168.2.194"
#define IP "47.107.184.34"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectToServer();
    void disconnectfromhost();
    void disconnected();
    QTcpSocket *getSocket();

    void deviceControl(my::Node node);
    void devStuSearch();
    void closeAllDev();

    void initProtobuf(my::Node &node);

    void initTerminal();

    QString findDeviceType(QString name){
        if(name!=""&&m_datanode!=nullptr){
            for (int i=0;m_datanode->size();++i) {
                if(m_datanode->at(i).devicename==name){
                    return m_datanode->at(i).devicetype;
                }
            }
        }
    }


public slots:
    void connected();
    void recvFrom();
    void sendTo(my::Node node);
private slots:

    void on_bindBtn_clicked();

signals:
    void sendReady(my::Node node);

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket = nullptr;
    LedDev *leddev;
    my::Node m_node;
    QString id = "A1";

    MainForm *mainform = nullptr;
    devControl *devwin = nullptr;
    QVector<datanode> *m_datanode = nullptr;
};

#endif // MAINWINDOW_H
