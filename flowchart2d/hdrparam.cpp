#include "hdrparam.h"
#include "ui_hdrparam.h"

hdrParam::hdrParam(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::hdrParam)
{
    ui->setupUi(this);
}

hdrParam::~hdrParam()
{
    delete ui;
}
