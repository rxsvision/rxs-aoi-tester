#include "pcarclenform.h"
#include "ui_pcarclenform.h"

pcArclenForm::pcArclenForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcArclenForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcArclenForm::~pcArclenForm()
{
    delete ui;
}

bool pcArclenForm::SetParamValue(void *d){

    return true;
}


bool pcArclenForm::GetParamValue(void *d){

    return true;
}
