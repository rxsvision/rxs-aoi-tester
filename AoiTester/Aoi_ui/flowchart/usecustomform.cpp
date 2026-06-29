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
    UseCustomParam *scp = (UseCustomParam *)d;
    UserCustomStep *ucs = (UserCustomStep*)(*itfper)[Curid];
    QString mName = ui->selectedMethod->text();
    QString Pls = ui->params->text();
    ucs->CallSet(mName.toLocal8Bit().data(),Pls.toLocal8Bit().data());
    return SelectLinkerGetFromUi(ui->SelectLinker,&scp->srcLinker);
}

bool UseCustomForm::SetParamValue(void *d) {
    UseCustomParam *scp = (UseCustomParam *)d;
    ui->selectedMethod->setText(QString::fromLocal8Bit(scp->wichMethod));
    ui->params->setText(QString::fromLocal8Bit(scp->params));
    return SelectLinkerSetToUi(ui->SelectLinker,&scp->srcLinker);
}

void UseCustomForm::on_apiGrp_itemClicked(QTableWidgetItem *item)
{

}

