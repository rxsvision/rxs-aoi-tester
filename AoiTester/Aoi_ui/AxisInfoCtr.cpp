#include "AxisInfoCtr.h"
#include "qstyle.h"
#include "ui_AxisInfoCtr.h"
#include "ccConsole.h"

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
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    mfper->AxServon(Axi->AxName,!Axi->Status.OnServ);
}


void AxisInfoCtr::on_jogPmove_pressed()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    mfper->AxMove(Axi->AxName,'+');
}


void AxisInfoCtr::on_jogPmove_released()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    mfper->AxStop(Axi->AxName);
}


void AxisInfoCtr::on_jogNmove_pressed()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    mfper->AxMove(Axi->AxName,'-');
}


void AxisInfoCtr::on_jogNmove_released()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    mfper->AxStop(Axi->AxName);
}


void AxisInfoCtr::on_absMove_clicked()
{
    QString tagPos = ui->movPosSet->text();
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    Axi->TagePos =tagPos.toFloat();
    mfper->AxMove(Axi->AxName,'A',true);
}


void AxisInfoCtr::on_relMov_clicked()
{
    QString tagPos = ui->movPosSet->text();
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    Axi->TagePos =tagPos.toFloat();
    mfper->AxMove(Axi->AxName,'R',true);
}

