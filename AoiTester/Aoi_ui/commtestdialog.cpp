#include "commtestdialog.h"
#include "ui_commtestdialog.h"
#include "rxsFixdal.h"

commTestDialog::commTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::commTestDialog)
{
    ui->setupUi(this);
    clientListFill();
}

commTestDialog::~commTestDialog()
{
    delete ui;
}

void commTestDialog::on_cmdSend_clicked()
{
    QString content = ui->cmdText->toPlainText();
    QByteArray c = content.toLocal8Bit();
    QList<QListWidgetItem*> ips = ui->ipList->selectedItems();
    if(ips.count()<1){
        return;
    }
    QByteArray tageip =  ips[0]->text().toLocal8Bit();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->LocServerSend(tageip.data(),c.data(),c.length());
}


void commTestDialog::clientListFill(){
    if(ui->ipList->count()>0){
        ui->ipList->clear();
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    int i =0;
    const char *p=0;
    while((p=wrkpd->LocServerAccessForeach(i))!=0){
        ui->ipList->addItem(QString::fromLocal8Bit(p));
    }

    rxsMainFrame *psys = rxsMainFrameGet();
    kvpdChr* val = psys->FacParam->KvpRead("服务IP");
    if(val !=0 && val->val != 0){
        ui->LocIp->setText(QString::fromLocal8Bit(val->val));
    }
    val = psys->FacParam->KvpRead("服务端口");
    if(val !=0 && val->val != 0){
        ui->LocPort->setText(QString::fromLocal8Bit(val->val));
    }
    val = psys->FacParam->KvpRead("服务启动方式");
    if(val !=0 && val->val != 0 && val->val[0]=='1'){
        ui->flowSw->setChecked(true);
    }else{
        ui->flowSw->setChecked(false);
    }
}


void commTestDialog::on_ipList_doubleClicked(const QModelIndex &index)
{
    int i = index.row();
    QListWidgetItem* tp = ui->ipList->item(i);
    QByteArray tage = tp->text().toLocal8Bit();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *v = wrkpd->LocServerReceive(tage.data());
    ui->recvText->clear();
    ui->recvText->setText(QString::fromLocal8Bit(v));
}


void commTestDialog::on_openTcpServer_clicked()
{
    rxsMainFrame *psys = rxsMainFrameGet();
    QByteArray locip = ui->LocIp->text().toLocal8Bit();
    QByteArray lcoport = ui->LocPort->text().toLocal8Bit();
    psys->FacParam->KvpUpdater("服务IP",locip.data(),"本软件开启本地服务的IP地址!");
    psys->FacParam->KvpUpdater("服务端口",lcoport.data(),"本软件开启本地服务的端口!");
    if(ui->flowSw->isChecked()){
        psys->FacParam->KvpUpdater("服务启动方式","1","本软件的开启本地服务的方式，1随软件启动，0手动");
    }else{
        psys->FacParam->KvpUpdater("服务启动方式","0","本软件的开启本地服务的方式，1随软件启动，0手动");
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->LocServerOn(true);
}


void commTestDialog::on_CloseServer_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->LocServerOn(false);
}


void commTestDialog::on_refClient_clicked()
{
    clientListFill();
}

