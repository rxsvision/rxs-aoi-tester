#include "focuslensform.h"
#include "ui_focuslensform.h"

FocusLensForm::FocusLensForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::FocusLensForm)
{
    ui->setupUi(this);
}

FocusLensForm::~FocusLensForm()
{
    delete ui;
}


bool FocusLensForm::SetParamValue(void *d) {
    FocusingLensStep *fls = (FocusingLensStep *)(*itfper)[Curid];
    unsigned short x = fls->AdjValue();
    ui->curVal->setText(QString::number(x));
    ui->adjVal->setValue(x);
    unsigned dv = fls->DmsValue();
    ui->dms->setText(QString::number(dv));
    return true;
}

bool FocusLensForm::GetParamValue(void *d) {
    int v = ui->curVal->text().toInt();
    FocusingLensStep *fls = (FocusingLensStep *)(*itfper)[Curid];
    fls->AdjValue(v);
    int dms = ui->dms->text().toInt();
    fls->DmsValue(dms);
    return true;
}

void FocusLensForm::on_adjVal_valueChanged(int value)
{
    QString cv = QString::number(value);
    ui->curVal->setText(cv);
    FocusingLensStep *fls = (FocusingLensStep *)(*itfper)[Curid];
    fls->AdjValue(value);
}

