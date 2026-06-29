#include "tcpserverrecvform.h"
#include "ui_tcpserverrecvform.h"

TcpServerRecvForm::TcpServerRecvForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::TcpServerRecvForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

TcpServerRecvForm::~TcpServerRecvForm()
{
    delete ui;
}

void TcpServerRecvForm::on_SendTester_clicked()
{
    TcpServerRecvStep *tsrs  = (TcpServerRecvStep *)(*itfper)[Curid];
    tsrs->Run();
}

bool TcpServerRecvForm::SetParamValue(void *d) {
    TcpServerRecvStep *tsrs  = (TcpServerRecvStep *)(*itfper)[Curid];
    int port = -1;
    const char* ip =  tsrs->ipprConfig(port);
    const char* st = tsrs->sendTxt();
    ui->tageIp->setText(QString::fromLocal8Bit(ip));
    ui->tagePort->setText(QString::number(port));
    ui->testSRtext->setPlainText(QString::fromLocal8Bit(st));
    //return SelectLinkerSetToUi(ui->SelectLinker,&scp->srcLinker);
    return true;
}

bool TcpServerRecvForm::GetParamValue(void *d) {
    int port = ui->tagePort->text().toInt();
    QByteArray qbaIP = ui->tageIp->text().toLocal8Bit();
    const char *ip = qbaIP.data();
    QByteArray qbaST = ui->testSRtext->toPlainText().toLocal8Bit();
    const char *st = qbaST.data();
    TcpServerRecvStep *tsrs  = (TcpServerRecvStep *)(*itfper)[Curid];
    tsrs->ipprConfig(ip,port);
    tsrs->sendTxt(st);
    return true;
}

