#include "convertformatform.h"
#include "ui_convertformatform.h"

ConvertFormatForm::ConvertFormatForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ConvertFormatForm)
{
    ui->setupUi(this);
}

ConvertFormatForm::~ConvertFormatForm()
{
    delete ui;
}



bool ConvertFormatForm::SetParamValue(void *d){

    return true;
}


bool ConvertFormatForm::GetParamValue(void *d){

    return true;
}

void ConvertFormatForm::on_efSelected_clicked()
{

}

