#include "dataoutset.h"
#include "ui_dataoutset.h"

DataOutSet::DataOutSet(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::DataOutSet)
{
    ui->setupUi(this);
}

DataOutSet::~DataOutSet()
{
    delete ui;
}
