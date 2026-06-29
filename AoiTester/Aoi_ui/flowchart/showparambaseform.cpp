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

void ShowParamBaseForm::MainUiSet(FlowChart* mw){
    fcui = mw;
}

void ShowParamBaseForm::ShowFromSrcLinker(QComboBox *selectLinker,int filterID)
{
    if(itfper != 0){
        int i=0;
        cuOutLinker *cuolr=0;
        StepBlockBase *sbb=0;
        StepBlockBase *curSbb = (*itfper)[Curid];
        while((cuolr = itfper->ForeachCanUseOutLinker(i))!=0){
            i+=1;
            if(cuolr->id == filterID){
                continue;
            }
            sbb = (*itfper)[cuolr->id];
            if(sbb==0){
                continue;
            }
            if(sbb->OutType() == BLKOUTRESULTYPE_NONE){
                continue;
            }
            if(sbb->blkType() == StepBlockBase::ROIGRPMANGER){
                int index =-1;
                const char *name = "sin";
                do{
                    QString val = QString::number(cuolr->id)+"."+QString::number(index);
                    val =QString::fromLocal8Bit("%1-%2:").arg(cuolr->Name).arg(name)+val;
                    selectLinker->addItem(val);
                    index+=1;
                }while((name = sbb->GetCanOutSelecter(index))!=0);
                continue;
            }
            if((cuolr->outType & BLKOUTRESULTYPE_IMG)==BLKOUTRESULTYPE_IMG
                || (cuolr->outType & BLKOUTRESULTYPE_PC)==BLKOUTRESULTYPE_PC
                || (cuolr->outType & BLKOUTRESULTYPE_POINTSET)==BLKOUTRESULTYPE_POINTSET)
            {
                QString val = QString::number(cuolr->id)+"."+QString::number(cuolr->outType);
                val =QString::fromLocal8Bit(cuolr->Name)+":"+val;
                selectLinker->addItem(val);
            }
        }
    }
}

bool ShowParamBaseForm::SelectLinkerSetToUi(QComboBox *selectLinker,FromLinker *flr){
    QString val = QString::number(flr->id)+"."+QString::number(flr->outChoose);
    StepBlockBase *sbb = (*itfper)[flr->id];
    if(sbb ==0){ return false; }
    QString n = QString::fromLocal8Bit(sbb->Name());
    if(sbb->blkType() == StepBlockBase::ROIGRPMANGER){
        if(flr->outChoose == -1){
            QString nval =QString::fromLocal8Bit("%1-sin:").arg(n);
            selectLinker->setCurrentText(nval+val);
        }else{
            const char *nnn = sbb->GetCanOutSelecter(flr->outChoose);
            QString nval =QString::fromLocal8Bit("%1-%2:").arg(n).arg(nnn);
            selectLinker->setCurrentText(nval+val);
        }

    }else{
        selectLinker->setCurrentText(n+":"+val);
    }
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


void ShowParamBaseForm::ShowRRLIst_Val(QTableWidget * resultReport){
    StepBlockBase *sbb = ((*itfper)[Curid]);
    if(sbb == 0){return;}
    int i=0;
    int rc = 0;
    while((rc = resultReport->rowCount())>0)
    {
        resultReport->removeRow(0);
    }
    rxsResultReport* rpo=0;
    sbb->GetResult(&rpo);
    int nums=0;
    const char *fv=0;
    QStringList scpGrpheader;
    while((fv = rpo->GetFiled(nums))!=0){
        scpGrpheader<<QString::fromLocal8Bit(fv);
        nums+=1;
    }
    int ns = scpGrpheader.length();
    if(ns>0){
        resultReport->setColumnCount(ns);
        resultReport->setHorizontalHeaderLabels(scpGrpheader);
    }
    ns = rpo->RowCount();
    for(int row=0;row< ns;row++){
        resultReport->insertRow(row);
        for(int cols =0;cols<nums;cols++){
            fv = rpo->GetValue(row,cols);
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit(fv));
            resultReport->setItem(row,cols,val);
        }
    }
}


bool ShowParamBaseForm::SetParamValue(void *){
    return true;
}

bool ShowParamBaseForm::GetParamValue(void *){
    return true;
}

bool ShowParamBaseForm::GetTestResult(void *){
    return true;
}

