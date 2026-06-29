#include "pcarclenform.h"
#include "ui_pcarclenform.h"

pcArclenForm::pcArclenForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::pcArclenForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
    ui->resultReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultReport->setSelectionMode(QAbstractItemView::SingleSelection);
}

pcArclenForm::~pcArclenForm()
{
    delete ui;
}

void pcArclenForm::ShowRRLIst_Val(){
    PointCouldArclenStep *sbb = (PointCouldArclenStep *)((*itfper)[Curid]);
    if(sbb == 0){return;}
    int i=0;
    int rc = 0;
    while((rc = ui->resultReport->rowCount())>0)
    {
        ui->resultReport->removeRow(0);
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
        ui->resultReport->setColumnCount(ns);
        ui->resultReport->setHorizontalHeaderLabels(scpGrpheader);
    }
    ns = rpo->RowCount();
    for(int row=0;row< ns;row++){
        ui->resultReport->insertRow(row);
        for(int cols =0;cols<nums;cols++){
            fv = rpo->GetValue(row,cols);
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit(fv));
            ui->resultReport->setItem(row,cols,val);
        }
    }
}

bool pcArclenForm::SetParamValue(void *d){
    PointCouldArclenParam *pcap = (PointCouldArclenParam *)d;
    if(pcap->isDebug){
        ui->isDebug->setChecked(true);
    }else{
        ui->isDebug->setChecked(false);
    }
    int i = pcap->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcap->srcLinker);
    ShowRRLIst_Val();
    return true;
}


bool pcArclenForm::GetParamValue(void *d){
    PointCouldArclenParam *pcap = (PointCouldArclenParam *)d;
    pcap->isDebug = ui->isDebug->isChecked();
    pcap->dir = ui->dirSelected->currentIndex();
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcap->srcLinker);
}
