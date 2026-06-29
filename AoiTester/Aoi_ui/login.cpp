#include "login.h"
#include "ui_login.h"
#include "rxsMainFrame.h"

Login::Login(void *pSystem,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);//只显示关闭按钮。
    mainFramePtr = pSystem;
}

Login::~Login()
{
    delete ui;
}

void Login::on_login_clicked()
{
    QString uname = ui->Uname->text();
    QString pwd = ui->Pwd->text();
    if(uname.length()<1 ||pwd.length()<1){
        ui->msgText->setText(QString::fromLocal8Bit("用户名或密码未填写!"));
        return;
    }
    QByteArray qbaUn = uname.toLocal8Bit();
    QByteArray qbaPw = pwd.toLocal8Bit();

    char *Un = (char *)qbaUn.data();
    char *Pwd = (char *)qbaPw.data();

    if(!((rxsMainFrame *)mainFramePtr)->userManger->Login(Un,Pwd)){
        ui->msgText->setText(QString::fromLocal8Bit("用户名或密码错误!"));
    }else{
        ui->Pwd->setText("");
        ui->msgText->setText("");
        this->close();
    }
}


void Login::on_channel_clicked()
{
    ui->Pwd->setText("");
    ui->msgText->setText("");
    this->close();
}

