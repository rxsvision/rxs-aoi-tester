#include "pcpreprocessform.h"
#include "ui_pcpreprocessform.h"

pcPreprocessForm::pcPreprocessForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcPreprocessForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcPreprocessForm::~pcPreprocessForm()
{
    delete ui;
}


bool pcPreprocessForm::SetParamValue(void *d){
    PointCouldPreProParam *pcdp = (PointCouldPreProParam *)d;
    int i = pcdp->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcdp->srcLinker);
    if(pcdp->SinMethhod==1){
        ui->sdkIn->setChecked(false);
        ui->fileIn->setChecked(false);
        ui->srcIn->setChecked(true);
    }else if(pcdp->SinMethhod==2){
        ui->sdkIn->setChecked(false);
        ui->fileIn->setChecked(true);
        ui->srcIn->setChecked(false);
    }else{
        ui->sdkIn->setChecked(true);
        ui->fileIn->setChecked(false);
        ui->srcIn->setChecked(false);
    }
    while(ui->pcPreProcessTable->rowCount()>0){
        ui->pcPreProcessTable->removeRow(0);
    }

    PointCouldPreProcessStep *sbb = (PointCouldPreProcessStep *)((*itfper)[Curid]);
    std::string name;
    std::string params;
    i = 0;
    while(i < pcdp->funcNum){
        name = "";
        params = "";
        sbb->funNameParamGet(i,name,params);
        ui->pcPreProcessTable->insertRow(i);
        ui->pcPreProcessTable->setItem(i,0,new QTableWidgetItem(QString::fromStdString(name)));
        ui->pcPreProcessTable->setItem(i,1,new QTableWidgetItem(QString::fromStdString(params)));
        i+=1;
    }
    return true;
}

bool pcPreprocessForm::GetParamValue(void *d){
    PointCouldPreProParam *pcdp = (PointCouldPreProParam *)d;
    pcdp->dir = ui->dirSelected->currentIndex();
    if(ui->srcIn->isChecked()){
        pcdp->SinMethhod = 1;
    }else if(ui->fileIn->isChecked()){
        pcdp->SinMethhod = 2;
    }else{
        pcdp->SinMethhod = 0;
    }

    PointCouldPreProcessStep *sbb = (PointCouldPreProcessStep *)((*itfper)[Curid]);
    int r = ui->pcPreProcessTable->rowCount();
    sbb->funNameParamClr();
    const char *name = 0;
    const char *params = 0;
    for(int s = 0 ; s < r;s++){
        QByteArray qbaName =  ui->pcPreProcessTable->item(s,0)->text().toLocal8Bit();
        name = qbaName.data();
        QByteArray qbaParam = ui->pcPreProcessTable->item(s,1)->text().toLocal8Bit();
        params = qbaParam.data();
        sbb->funNameParamSet(name,params);
    }
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcdp->srcLinker);
}

void pcPreprocessForm::on_AddFNPS_clicked()
{
    int r = ui->pcPreProcessTable->rowCount();
    ui->pcPreProcessTable->insertRow(r);
    ui->pcPreProcessTable->setItem(r,0, new QTableWidgetItem(QString::fromLocal8Bit("funcName")));
    ui->pcPreProcessTable->setItem(r,1, new QTableWidgetItem(QString::fromLocal8Bit("p1|p2")));
}


void pcPreprocessForm::on_DelFNPS_clicked()
{
    int csr = ui->pcPreProcessTable->currentRow();
    ui->pcPreProcessTable->removeRow(csr);
}

