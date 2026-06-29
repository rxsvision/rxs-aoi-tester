#include "conditionscomparison.h"
#include "ui_conditionscomparison.h"

ConditionsComparison::ConditionsComparison(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ConditionsComparison)
{
    ui->setupUi(this);
}

ConditionsComparison::~ConditionsComparison()
{
    delete ui;
}
