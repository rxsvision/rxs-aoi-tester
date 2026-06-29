#include "sysstatus.h"
#include "ui_sysstatus.h"
#include "rxsFixdal.h"


rxsMainFrame *rsmMfptr;

sysStatus::sysStatus(void *rxsmp,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sysStatus)
{
    ui->setupUi(this);
    rsmMfptr=(rxsMainFrame *)rxsmp;
    ui->noCalu->setChecked(false);
    ui->noCaluTwod->setChecked(false);
    ui->AutoOutMatter->setChecked(true);
}

sysStatus::~sysStatus()
{
    delete ui;
}

void sysStatus::UpdateCycleTime(long t){
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(t<0){
        QString txt = QString::number(0);
        ui->cycleTime->setText(txt);
    }else{

        long diffs = wrkpd->getCycleTime();
        QString txt = QString::number(diffs);
        ui->cycleTime->setText(txt);
    }
    ui->NGnums->setText(QString::number(wrkpd->product.ngNum));
    ui->OKnums->setText(QString::number(wrkpd->product.okNum));
    if(wrkpd->product.curPass){
        QPalette pe = ui->curPass->palette();
        pe.setColor(QPalette::WindowText,Qt::green);
        ui->curPass->setPalette(pe);
        ui->curPass->setText(QString::fromLocal8Bit("OK"));
    }else{
        QPalette pe = ui->curPass->palette();
        pe.setColor(QPalette::WindowText,Qt::red);
        ui->curPass->setPalette(pe);
        ui->curPass->setText(QString::fromLocal8Bit("NG"));
    }
}

void sysStatus::uiCtr_ref(){
    SystemInfo::FixStaus fxs =  rsmMfptr->sysStatus->CurFixStatus();

    switch(fxs){
    case SystemInfo::FixStaus::INIT :
        ui->siStatus->setText(QString::fromLocal8Bit("初始化中"));
        break;

    case SystemInfo::FixStaus::RDY :
        ui->siStatus->setText(QString::fromLocal8Bit("待机中"));
        break;

    case SystemInfo::FixStaus::PAUSEING :
        ui->siStatus->setText(QString::fromLocal8Bit("暂停中"));
        break;
    case SystemInfo::FixStaus::RESETING :
        ui->siStatus->setText(QString::fromLocal8Bit("复位中"));
        break;

    case SystemInfo::FixStaus::RUNING :
        ui->siStatus->setText(QString::fromLocal8Bit("加工中"));
        break;

    case SystemInfo::FixStaus::ALAMING :
        ui->siStatus->setText(QString::fromLocal8Bit("报警中"));
        break;
    }
    int wich=0;
    ui->vsStatus->setText(
        (WorkProcessDalGet()->ImageableHasOpen(wich))?
            QString::fromLocal8Bit("已打开"):
            QString::fromLocal8Bit("未打开")
        );

//    ui->mcStatus->setText(
//        )
}

QString sysStatus::GetRepCnt(){
    return ui->repRunCnt->text();
}

QString sysStatus::GetProductInfo(){
    QString ptnv = ui->prodType->text();
    return ptnv;
}

void sysStatus::on_noCalu_clicked(bool checked)
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(checked){
        wrkpd->OnlyShowNoCalculation();
    }else{
        wrkpd->OnlyShowNoCalculation(false);
    }
}


void sysStatus::on_checkBox_clicked(bool checked)
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(checked){
        wrkpd->OnlyMoveNoScan();
    }else{
        wrkpd->OnlyMoveNoScan(false);
    }
}

void sysStatus::on_checkBox_2_clicked(bool checked)
{
    if(checked){
        ui->repRunCnt->setReadOnly(false);
    }else{
        ui->repRunCnt->setReadOnly(true);
    }
}

void sysStatus::on_movOutPos_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->OutMatterPos_Move();
}

void sysStatus::on_movInPos_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->InMatterPos_Move();
}

void sysStatus::on_zSaftyPos_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->Zsaftypos(true);
}

void sysStatus::on_AutoOutMatter_clicked(bool checked)
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(checked){
        wrkpd->AutoOutMatterOpen(true);
    }else{
        wrkpd->AutoOutMatterOpen(false);
    }
}

void sysStatus::on_srcSave_clicked(bool checked)
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(checked){
        wrkpd->GatherSrcPicSave(true);
    }else{
        wrkpd->GatherSrcPicSave(false);
    }
}

void sysStatus::on_noCaluTwod_stateChanged(int arg1)
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(ui->noCaluTwod->checkState() == Qt::Checked){
        wrkpd->NoCalculationTwo();
    }else{
        wrkpd->NoCalculationTwo(false);
    }
}

