#ifndef USRMANGER_H
#define USRMANGER_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class usrManger;
}

class usrManger : public QDialog
{
    Q_OBJECT

public:
    explicit usrManger(void *,QWidget *parent = nullptr);
    ~usrManger();

private slots:
    void on_AddUser_clicked();

    void on_DelUser_clicked();

    void on_Save_clicked();

private:
    Ui::usrManger *ui;
    void *mainFrame;
    QStandardItemModel *model;
    void loadUserData();
};

#endif // USRMANGER_H
