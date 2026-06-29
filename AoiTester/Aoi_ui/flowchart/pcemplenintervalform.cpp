#include "pcemplenintervalform.h"
#include "ui_pcemplenintervalform.h"

pcEmplenIntervalForm::pcEmplenIntervalForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcEmplenIntervalForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcEmplenIntervalForm::~pcEmplenIntervalForm()
{
    delete ui;
}


bool pcEmplenIntervalForm::SetParamValue(void *d){
    PointCouldEmplenParam *pcdp = (PointCouldEmplenParam *)d;
    int i = pcdp->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcdp->srcLinker);
    ui->range->setText(QString::number( pcdp->rang));
    ShowRRLIst_Val(ui->resultRR);
    return true;
}


bool pcEmplenIntervalForm::GetParamValue(void *d){
    PointCouldEmplenParam *pcdp = (PointCouldEmplenParam *)d;
    pcdp->dir = ui->dirSelected->currentIndex();
    pcdp->rang = ui->range->text().toInt();
//    QByteArray qba = ui->modelFilePath->text().toLocal8Bit();
//    const char *mpf = qba.data();
//    PointCouldDeformationStep *sbb = (PointCouldDeformationStep *)((*itfper)[Curid]);
//    sbb->ModelFilePathSet(mpf);
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcdp->srcLinker);
}
