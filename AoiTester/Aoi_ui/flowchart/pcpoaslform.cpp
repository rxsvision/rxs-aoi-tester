#include "pcpoaslform.h"
#include "ui_pcpoaslform.h"

pcPoaslForm::pcPoaslForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcPoaslForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcPoaslForm::~pcPoaslForm()
{
    delete ui;
}

bool pcPoaslForm::SetParamValue(void *d){
    PointCouldPoaslParam *poasl = (PointCouldPoaslParam *)d;
    if(poasl->tp_s_or_l==1){
        ui->lineProf->setChecked(true);
        ui->surfaceProf->setChecked(false);
    }else if(poasl->tp_s_or_l==2){
        ui->lineProf->setChecked(false);
        ui->surfaceProf->setChecked(true);
    }
    if(poasl->roiEnable){
        ui->roiChk->setChecked(true);
    }else{
        ui->roiChk->setChecked(false);
    }
    QString val = QString::number(poasl->roiArea.x);
    ui->roi_Sx->setText(val);
    val = QString::number(poasl->roiArea.y);
    ui->roi_Sy->setText(val);
    val = QString::number(poasl->roiArea.w);
    ui->roi_W->setText(val);
    val = QString::number(poasl->roiArea.h);
    ui->roi_H->setText(val);

    return SelectLinkerSetToUi(ui->SelectLinker,&poasl->srcLinker);
}


bool pcPoaslForm::GetParamValue(void *d){
    PointCouldPoaslParam *poasl = (PointCouldPoaslParam *)d;
    if(ui->lineProf->isChecked()){
        poasl->tp_s_or_l = 1;
    }else if(ui->surfaceProf->isChecked()){
        poasl->tp_s_or_l = 2;
    }
    if(ui->roiChk->isChecked()){
        poasl->roiEnable = true;
        QString gv = ui->roi_Sx->text();
        poasl->roiArea.x =gv.toInt();
        gv = ui->roi_Sy->text();
        poasl->roiArea.y =gv.toInt();
        gv = ui->roi_W->text();
        poasl->roiArea.w =gv.toInt();
        gv = ui->roi_H->text();
        poasl->roiArea.h =gv.toInt();
    }else{
        poasl->roiEnable = false;
    }

    return SelectLinkerGetFromUi(ui->SelectLinker,&poasl->srcLinker);
}
