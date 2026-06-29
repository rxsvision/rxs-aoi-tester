#include "usrmanger.h"
#include "ui_usrmanger.h"
#include "rxsMainFrame.h"

usrManger::usrManger(void *mframe,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::usrManger)
{
    ui->setupUi(this);

    mainFrame = mframe;

    model =new QStandardItemModel(this);
    QStringList header;
    header <<QString::fromLocal8Bit("用户名")<<QString::fromLocal8Bit("密码")<<QString::fromLocal8Bit("权限");
    model->setHorizontalHeaderLabels(header);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
}

usrManger::~usrManger()
{
    delete model;
    delete ui;
}

void usrManger::loadUserData(){
    int rowI=0;
    UserManger *um = ((rxsMainFrame *)mainFrame)->userManger;
    if(um == NULL){return;}
    UserManger::UserInfo* USR = 0;
    QString UAG[4] = { "NOLOGIN", "OPTER", "TECH", "ADMIN" };
    um->UserForech(true);
    while ((USR = um->UserForech(false)) != 0) {

        QStandardItem Un = QStandardItem(QString(USR->Uname));
        QStandardItem Pwd = QStandardItem(QString(USR->Uname));
        QStandardItem Ua = QStandardItem(UAG[USR->Auth]);
        Un.setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        Pwd.setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        Ua.setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        model->setItem(rowI, 0, &Un);
        model->setItem(rowI, 1, &Pwd);
        model->setItem(rowI, 2, &Ua);

        rowI+=1;
    }
}

void usrManger::on_AddUser_clicked()
{
    int rnum = model->rowCount();
    model->insertRow(rnum);
    QStandardItem Un = QStandardItem(QString::fromLocal8Bit("User"));
    QStandardItem Pwd = QStandardItem(QString::fromLocal8Bit("密码"));
    QStandardItem Ua = QStandardItem(QString::fromLocal8Bit("1"));
    model->setItem(rnum, 0, &Un);
    model->setItem(rnum, 1, &Pwd);
    model->setItem(rnum, 2, &Ua);
}


void usrManger::on_DelUser_clicked()
{
    int sr = ui->tableView->currentIndex().row();
    model->removeRow(sr);
}


void usrManger::on_Save_clicked()
{
    int rc = model->rowCount();
    if(rc>0){
        UserManger *um = ((rxsMainFrame *)mainFrame)->userManger;
        if(um==NULL){return ;}
        for(unsigned xm =0;xm<rc;xm++){

            QString u = model->item(xm,0)->text();
            QString p = model->item(xm,0)->text();
            QString a = model->item(xm,0)->text();
            std::string uba = u.toStdString();
            std::string pba = p.toStdString();
            std::string aba = a.toStdString();
            um->AddUser(uba.c_str(),pba.c_str(),UserManger::USER_AUTH::OPTER);
        }
    }
}

