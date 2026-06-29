#include "pcoutform.h"
#include "ui_pcoutform.h"

pcOutForm::pcOutForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcOutForm)
{
    ui->setupUi(this);
}

pcOutForm::~pcOutForm()
{
    delete ui;
}


bool pcOutForm::SetParamValue(void *d){

    return true;
}


bool pcOutForm::GetParamValue(void *d){

    return true;
}
