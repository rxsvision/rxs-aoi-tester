#include "pcboundolform.h"
#include "ui_pcboundolform.h"

pcBoundolForm::pcBoundolForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcBoundolForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

pcBoundolForm::~pcBoundolForm()
{
    delete ui;
}


bool pcBoundolForm::SetParamValue(void *d){
    PointCouldBoundolParam *pcap = (PointCouldBoundolParam *)d;
    while(ui->params->rowCount()>0){
        ui->params->removeRow(0);
    }
    if(pcap->isRoifp == 1){
        ui->autoFind->setChecked(true);
    }else{
        ui->autoFind->setChecked(false);
    }
    QString fn = QString::fromLocal8Bit(pcap->funName);
    if(fn == "_")
    {
        ui->funcName->clear();
    }else{
        ui->funcName->setText(fn);
    }
    QString ps = QString::fromLocal8Bit( pcap->funParams);
    QStringList psss =  ps.split('|');
    for(int m=0;m<psss.size();m++){
        if(psss[m]=="_"){
            continue;
        }
        ui->params->insertRow(m);
        ui->params->setItem(m,0,new QTableWidgetItem(psss[m]));
    }
    int i = pcap->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcap->srcLinker);
    ShowRRLIst_Val(ui->resultReport);
    return true;
}

bool pcBoundolForm::GetParamValue(void *d){
    PointCouldBoundolParam *pcap = (PointCouldBoundolParam *)d;
    pcap->dir = ui->dirSelected->currentIndex();
    pcap->isRoifp = (ui->autoFind->isChecked()?1:0);
    PointCouldBoundolStep *sbb = (PointCouldBoundolStep *)((*itfper)[Curid]);
    QByteArray qbaFunn =  ui->funcName->text().toLocal8Bit();
    sbb->FunNameSet(qbaFunn.data());
    int l = ui->params->rowCount();
    QString ql;
    for(int i =0;i<l;i++){
        ql += ui->params->item(i,0)->text();
        if(i<(l-1)){
            ql += "|";
        }
    }
    QByteArray qbaParams = ql.toLocal8Bit();
    sbb->FunParamsSet(qbaParams.data());
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcap->srcLinker);
}

void pcBoundolForm::on_AddParam_clicked()
{
    int r = ui->params->rowCount();
    ui->params->insertRow(r);
    ui->params->setItem(r,0,new QTableWidgetItem(QString::number(r)));
}

void pcBoundolForm::on_DelParam_clicked()
{
    int csr = ui->params->currentRow();
    ui->params->removeRow(csr);
}


void pcBoundolForm::tableClrCols(){
    while(ui->params->columnCount()>0){
        ui->params->removeColumn(0);
    }
}

void pcBoundolForm::tableClrRows(){
    while(ui->params->rowCount()>0){
        ui->params->removeRow(0);
    }
}


void pcBoundolForm::on_autoFind_stateChanged(int arg1)
{
    if(ui->autoFind->isChecked()){
        //tableClrCols();
        ui->AddParam->setEnabled(false);
        ui->DelParam->setEnabled(false);
        ui->funcName->setReadOnly(true);
    }else{
        //tableClrCols();
        ui->AddParam->setEnabled(true);
        ui->DelParam->setEnabled(true);
        ui->funcName->setReadOnly(false);
    }
}

