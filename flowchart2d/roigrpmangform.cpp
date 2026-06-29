#include "roigrpmangform.h"
#include "ui_roigrpmangform.h"

roiGrpMangForm::roiGrpMangForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::roiGrpMangForm)
{
    ui->setupUi(this);
}

roiGrpMangForm::~roiGrpMangForm()
{
    delete ui;
}


bool roiGrpMangForm::SetParamValue(void *d){

    return true;
}


bool roiGrpMangForm::GetParamValue(void *d){

    return true;
}
