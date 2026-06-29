#include "scancodeform.h"
#include "ui_scancodeform.h"
#include "imgProcess.h"

ScanCodeForm::ScanCodeForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ScanCodeForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

ScanCodeForm::~ScanCodeForm()
{
    delete ui;
}



bool ScanCodeForm::SetParamValue(void *d){
    ScanCodeParam *scp = (ScanCodeParam *)d;
    if(scp->codeType==0){
        ui->barCode->setChecked(true);
        ui->qrCode->setChecked(false);
        ui->dmCode->setChecked(false);
    }else if(scp->codeType==1){
        ui->barCode->setChecked(false);
        ui->qrCode->setChecked(true);
        ui->dmCode->setChecked(false);
    }else if(scp->codeType==2){
        ui->barCode->setChecked(false);
        ui->qrCode->setChecked(false);
        ui->dmCode->setChecked(true);
    }
    if(scp->roiEnable){
        ui->roiChk->setChecked(true);
    }else{
        ui->roiChk->setChecked(false);
    }
    QString val = QString::number(scp->roiArea.x);
    ui->roi_Sx->setText(val);
    val = QString::number(scp->roiArea.y);
    ui->roi_Sy->setText(val);
    val = QString::number(scp->roiArea.w);
    ui->roi_W->setText(val);
    val = QString::number(scp->roiArea.h);
    ui->roi_H->setText(val);

    return SelectLinkerSetToUi(ui->SelectLinker,&scp->srcLinker);
}

bool ScanCodeForm::GetParamValue(void *d){
    ScanCodeParam *scp = (ScanCodeParam *)d;
    if(ui->barCode->isChecked()){
        scp->codeType = 0;
    }else if(ui->qrCode->isChecked()){
        scp->codeType = 1;
    }else if(ui->dmCode->isChecked()){
        scp->codeType = 2;
    }
    if(ui->roiChk->isChecked()){
        scp->roiEnable = true;
        QString gv = ui->roi_Sx->text();
        scp->roiArea.x =gv.toInt();
        gv = ui->roi_Sy->text();
        scp->roiArea.y =gv.toInt();
        gv = ui->roi_W->text();
        scp->roiArea.w =gv.toInt();
        gv = ui->roi_H->text();
        scp->roiArea.h =gv.toInt();
    }else{
        scp->roiEnable = false;
    }

    return SelectLinkerGetFromUi(ui->SelectLinker,&scp->srcLinker);
}

void ScanCodeForm::on_roiAdd_clicked()
{

}

