#include "pcfilterform.h"
#include "ui_pcfilterform.h"

pcFilterForm::pcFilterForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcFilterForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcFilterForm::~pcFilterForm()
{
    delete ui;
}

bool pcFilterForm::SetParamValue(void *d){

    return true;
}


bool pcFilterForm::GetParamValue(void *d){

    return true;
}
