#include "pclenform.h"
#include "ui_pclenform.h"

pcLenForm::pcLenForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcLenForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
    ShowFromSrcLinker(ui->boundLinker,Curid);
    ui->resultReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultReport->setSelectionMode(QAbstractItemView::SingleSelection);
}

pcLenForm::~pcLenForm()
{
    delete ui;
}

bool pcLenForm::SetParamValue(void *d){
    PointCouldLenParam *pcdp = (PointCouldLenParam *)d;
    SelectLinkerSetToUi(ui->SelectLinker,&pcdp->srcLinker);
    if(pcdp->ispredict){
        ui->isPrectid->setChecked(true);
        //pcdp->Params.Plp;

    }else{
        ui->isPrectid->setChecked(false);
        ui->dirSelected->setCurrentIndex(pcdp->Params.Mlp.dir);
        SelectLinkerSetToUi(ui->boundLinker,&pcdp->Params.Mlp.boundLinker);
        ui->condy->setChecked(pcdp->Params.Mlp.consider_y);
        ui->Px1->setText(QString::number(pcdp->Params.Mlp.Bound[0].x));
        ui->Py1->setText(QString::number(pcdp->Params.Mlp.Bound[0].y));
        ui->Pz1->setText(QString::number(pcdp->Params.Mlp.Bound[0].z));
        ui->Px2->setText(QString::number(pcdp->Params.Mlp.Bound[1].x));
        ui->Py2->setText(QString::number(pcdp->Params.Mlp.Bound[1].y));
        ui->Pz2->setText(QString::number(pcdp->Params.Mlp.Bound[1].z));

        ShowRRLIst_Val(ui->resultReport);
    }
    return true;
}


bool pcLenForm::GetParamValue(void *d){
    PointCouldLenParam *pcdp = (PointCouldLenParam *)d;
    pcdp->ispredict = ui->isPrectid->isChecked();
    if(pcdp->ispredict){

    }else{
        pcdp->Params.Mlp.dir = ui->dirSelected->currentIndex();
        pcdp->Params.Mlp.consider_y = ui->condy->isChecked();
        pcdp->Params.Mlp.Bound[0]={ui->Px1->text().toFloat(),ui->Py1->text().toFloat(),ui->Pz1->text().toFloat()};
        pcdp->Params.Mlp.Bound[1]={ui->Px2->text().toFloat(),ui->Py2->text().toFloat(),ui->Pz2->text().toFloat()};
        SelectLinkerGetFromUi(ui->boundLinker,&pcdp->Params.Mlp.boundLinker);
    }
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcdp->srcLinker);
}

void pcLenForm::on_boundSrc_clicked(bool checked)
{
    if(checked){
        ui->Px1->setReadOnly(false);
        ui->Py1->setReadOnly(false);
        ui->Pz1->setReadOnly(false);
        ui->Px2->setReadOnly(false);
        ui->Py2->setReadOnly(false);
        ui->Pz2->setReadOnly(false);
        ui->boundLinker->setEnabled(false);
    }else{
        ui->Px1->setReadOnly(true);
        ui->Py1->setReadOnly(true);
        ui->Pz1->setReadOnly(true);
        ui->Px2->setReadOnly(true);
        ui->Py2->setReadOnly(true);
        ui->Pz2->setReadOnly(true);
        ui->boundLinker->setEnabled(true);
    }
}

