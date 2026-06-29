#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(void *pSystem,QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_login_clicked();

    void on_channel_clicked();

private:
    Ui::Login *ui;
    void *mainFramePtr;
};

#endif // LOGIN_H
