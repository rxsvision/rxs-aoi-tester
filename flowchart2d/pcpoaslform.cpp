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

    return true;
}


bool pcPoaslForm::GetParamValue(void *d){

    return true;
}
