#include "lambertianphotometricstereo.h"
#include "ui_lambertianphotometricstereo.h"

LambertianPhotometricStereo::LambertianPhotometricStereo(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::LambertianPhotometricStereo)
{
    ui->setupUi(this);
}

LambertianPhotometricStereo::~LambertianPhotometricStereo()
{
    delete ui;
}
