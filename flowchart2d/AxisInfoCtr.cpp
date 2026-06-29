#include "AxisInfoCtr.h"
#include "qstyle.h"
#include "ui_AxisInfoCtr.h"

AxisInfoCtr::AxisInfoCtr(AxisInfo *axi,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AxisInfoCtr)
{
    ui->setupUi(this);
    Axi = axi;
    QString axname = QString::fromLocal8Bit(Axi->AxName);
    ui->AxName->setText(axname);
}

AxisInfoCtr::~AxisInfoCtr()
{
    delete ui;
}

void AxisInfoCtr::ui_refush_dataupdata(){
    if(Axi->Status.OnOrg){
        ui->Org->setStyleSheet("background-color: rgb(255, 0, 0);");
    }else{
        ui->Org->setStyleSheet("background-color: rgb(0, 255, 0);");
    }
    if(Axi->Status.OnLimtP){
        ui->LimtP->setStyleSheet("background-color: rgb(255, 0, 0);");
    }else{
        ui->LimtP->setStyleSheet("background-color: rgb(0, 255, 0);");
    }
    if(Axi->Status.OnLimtN){
        ui->LimtN->setStyleSheet("background-color: rgb(255, 0, 0);");
    }else{
        ui->LimtN->setStyleSheet("background-color: rgb(0, 255, 0);");
    }
    if(Axi->Status.OnServ){
        ui->ServOn->setStyleSheet("background-color: rgb(0, 255, 0);");
    }else{
        ui->ServOn->setStyleSheet("background-color: rgb(255, 0, 0);");
    }
    QString curpos=QString::number(Axi->Status.EncPos);
    ui->curPosShower->setText(curpos);
}

void AxisInfoCtr::on_ServOn_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    mfper->AxServon(Axi->AxName,!Axi->Status.OnServ);
}


void AxisInfoCtr::on_jogPmove_pressed()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    mfper->AxMove(Axi->AxName,'+');
}


void AxisInfoCtr::on_jogPmove_released()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    mfper->AxStop(Axi->AxName);
}


void AxisInfoCtr::on_jogNmove_pressed()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    mfper->AxMove(Axi->AxName,'-');
}


void AxisInfoCtr::on_jogNmove_released()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    mfper->AxStop(Axi->AxName);
}


void AxisInfoCtr::on_absMove_clicked()
{
    QString tagPos = ui->movPosSet->text();
    rxsMainFrame *mfper = rxsMainFrameGet();
    Axi->TagePos =tagPos.toFloat();
    mfper->AxMove(Axi->AxName,'A',true);
}


void AxisInfoCtr::on_relMov_clicked()
{
    QString tagPos = ui->movPosSet->text();
    rxsMainFrame *mfper = rxsMainFrameGet();
    Axi->TagePos =tagPos.toFloat();
    mfper->AxMove(Axi->AxName,'R',true);
}

