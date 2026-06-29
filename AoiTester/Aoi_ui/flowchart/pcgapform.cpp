#include "pcgapform.h"
#include "ui_pcgapform.h"

pcGapForm::pcGapForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcGapForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcGapForm::~pcGapForm()
{
    delete ui;
}


bool pcGapForm::SetParamValue(void *d){
    PointCouldGapParam *pcdp = (PointCouldGapParam *)d;
    int i = pcdp->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcdp->srcLinker);
    ui->range->setText(QString::number(pcdp->range));
    if(pcdp->pNum>0){
        ui->sXYpos->setText(QString::number( pcdp->p[0].xs));
        ui->coplThrev->setText(QString::number( pcdp->p[0].coplanarityThreshold));
        ui->skewThres->setText(QString::number( pcdp->p[0].skewThreshold));
    }
    ShowRRLIst_Val(ui->resultRR);
    return true;
}


bool pcGapForm::GetParamValue(void *d){
    PointCouldGapParam *pcdp = (PointCouldGapParam *)d;
    pcdp->dir = ui->dirSelected->currentIndex();
    pcdp->range = ui->range->text().toInt();
    pcdp->pNum = 1;
    Xscstp xsp={
        ui->sXYpos->text().toFloat(),
        ui->coplThrev->text().toFloat(),
        ui->skewThres->text().toFloat()
    };
    PointCouldGapStep *sbb = (PointCouldGapStep *)((*itfper)[Curid]);
    sbb->XscstpClr();
    sbb->XscstpPush(&xsp);
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcdp->srcLinker);
}
