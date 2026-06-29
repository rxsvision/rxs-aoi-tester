#include "showparambaseform.h"
#include "ui_showparambaseform.h"
#include <iostream>


ShowParamBaseForm::ShowParamBaseForm(imgTaskFlowerProcesser *A,int curID,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowParamBaseForm)
{
    ui->setupUi(this);
    itfper = A;
    Curid = curID;
}

ShowParamBaseForm::~ShowParamBaseForm()
{
    delete ui;
}

void ShowParamBaseForm::ShowFromSrcLinker(QComboBox *selectLinker,int filterID)
{
    if(itfper != 0){
        int i=0;
        cuOutLinker *cuolr=0;
        while((cuolr = itfper->ForeachCanUseOutLinker(i))!=0){
            i+=1;
            if(cuolr->id == filterID){
                continue;
            }
            if((cuolr->outType & BLKOUTRESULTYPE_IMG)==BLKOUTRESULTYPE_IMG
                || (cuolr->outType & BLKOUTRESULTYPE_PC)==BLKOUTRESULTYPE_PC){
                QString val = QString::number(cuolr->id)+"."+QString::number(cuolr->outType);
                val =QString::fromLocal8Bit(cuolr->Name)+":"+val;
                selectLinker->addItem(val);
            }
        }
    }
}

bool ShowParamBaseForm::SelectLinkerSetToUi(QComboBox *selectLinker,FromLinker *flr){
    QString val = QString::number(flr->id)+"."+QString::number(flr->outChoose);
    QString n = (*itfper)[flr->id]->Name();
    selectLinker->setCurrentText(n+":"+val);
    return true;
}

bool ShowParamBaseForm::SelectLinkerGetFromUi(QComboBox *selectLinker,FromLinker *flr){
    int curi = selectLinker->currentIndex();
    QString curtxt = selectLinker->currentText();
    QStringList tv = curtxt.split(':',Qt::SkipEmptyParts);
    if(tv.length()<2){

        return false;
    }
    QStringList tio = tv[1].split('.');
    if(tv.length()<2){

        return false;
    }
    flr->id = tio[0].toInt();
    flr->outChoose= tio[1].toInt();
    return true;
}

bool ShowParamBaseForm::SetParamValue(void *){
    return true;
}

bool ShowParamBaseForm::GetParamValue(void *){
    return true;
}

