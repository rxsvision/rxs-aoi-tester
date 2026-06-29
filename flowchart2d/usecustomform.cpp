#include "usecustomform.h"
#include "ui_usecustomform.h"

UseCustomForm::UseCustomForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::UseCustomForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

UseCustomForm::~UseCustomForm()
{
    delete ui;
}


bool UseCustomForm::GetParamValue(void *d) {


    return true;
}



bool UseCustomForm::SetParamValue(void *d) {

    return true;
}

void UseCustomForm::on_apiGrp_itemClicked(QTableWidgetItem *item)
{

}

