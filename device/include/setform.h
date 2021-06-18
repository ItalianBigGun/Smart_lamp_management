#ifndef SETFORM_H
#define SETFORM_H

#include <QWidget>
#include "database.h"
#include <QDebug>
#include <QThread>
namespace Ui {
class SetForm;
}

class SetForm : public QWidget
{
    Q_OBJECT

public:
    explicit SetForm(QWidget *parent = nullptr);
    ~SetForm();
    void showEvent(QShowEvent *e){
        //this->show();
        //this->move(
                    //(this->parentWidget()->width()-this->width())*0.5+this->parentWidget()->pos().x(),
                    //(this->parentWidget()->height()-this->height())*0.5+this->parentWidget()->pos().y()
                    //);
    }
private slots:

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

   // void on_tableView_activated(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::SetForm *ui;
    DataBase *m_base;
};

#endif // SETFORM_H
