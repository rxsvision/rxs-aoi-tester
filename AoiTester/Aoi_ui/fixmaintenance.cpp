#include "fixmaintenance.h"
#include "ui_fixmaintenance.h"
#include "rxsFixdal.h"

AxisInfo *axi=0;

FixMaintenance::FixMaintenance(void *mainFrame,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FixMaintenance)
{
    ui->setupUi(this);
    AxNum = 0;
    mainFramePtr = mainFrame;
    rxsMainFrame *rxsMfptr = (rxsMainFrame*)mainFramePtr;
    axi = rxsMfptr->AxInfo(AxNum);
    Kvpdchr Val={0,0,0};
    rxsMainFrame *psys = rxsMainFrameGet();
    psys->WrkProparam->Selected(2);
    psys->WrkProparam->DataReaderKvpd((unsigned)0,Val);
    if(Val.val != 0){
        float v = atof(Val.val);
        ui->filterThrev->setText( QString::number(v));
    }
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataReaderKvpd(1,Val);
    if(Val.val != 0){
        ui->outRepName->setText( QString::fromLocal8Bit(Val.val));
    }
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataReaderKvpd(3,Val);
    if(Val.val != 0){
        ui->myFlowPath->setText( QString::fromLocal8Bit(Val.val));
    }
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataReaderKvpd(6,Val);
    if(Val.val != 0){
        ui->modPath->setText( QString::fromLocal8Bit(Val.val));
    }
    pageValSetRef();
}

FixMaintenance::~FixMaintenance()
{
    delete ui;
}

void FixMaintenance::uiref_(){
    if(AxNum <1 || axi==0){return;}
    QString X = QString::number(axi[0].Status.EncPos);
    QString Y = QString::number(axi[1].Status.EncPos);
    QString Z = QString::number(axi[2].Status.EncPos);


}

void FixMaintenance::on_spRecord_clicked()
{


}

void FixMaintenance::on_epRecord_clicked()
{


}

void FixMaintenance::on_StartScan_clicked()
{

}


void FixMaintenance::on_ReadCurVal_clicked()
{

}


void FixMaintenance::on_SetCurVal_clicked()
{
    QString txtVal = ui->filterThrev->text();
    if(txtVal.length()>0){
       QByteArray qba = txtVal.toLocal8Bit();
       Kvpdchr Val={0,qba.data(),0};
       rxsMainFrame *psys = rxsMainFrameGet();
       psys->WrkProparam->Selected(2);
       psys->WrkProparam->DataEditer(0,&Val);
    }
}

void FixMaintenance::on_dgScaner_clicked()
{

}

void FixMaintenance::on_addVal_clicked()
{
    int r = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(r);
    ui->tableWidget->setItem(r,1,new QTableWidgetItem(QString::number(0.0)));
    ui->tableWidget->setItem(r,2,new QTableWidgetItem(QString::number(0.1)));
    ui->tableWidget->setItem(r,3,new QTableWidgetItem(QString::number(-0.1)));
    ui->tableWidget->setItem(r,4,new QTableWidgetItem(QString::fromLocal8Bit("1")));
}


void FixMaintenance::on_delVal_clicked()
{
    QList<QTableWidgetItem*> sl = ui->tableWidget->selectedItems();
    rxsMainFrame *psys = rxsMainFrameGet();
    for(int x=0;x<sl.count();x++){
       int r = sl[x]->row();
        ui->tableWidget->removeRow(r);
       QString miname = ui->tableWidget->item(r,0)->text();
       QByteArray qba = miname.toLocal8Bit();
       psys->WrkProparam->MeasurementStandRemove(qba.data());
    }
    pageValSetRef();
}


void FixMaintenance::on_fullClr_clicked()
{
    rxsMainFrame *psys = rxsMainFrameGet();
    psys->WrkProparam->MeasurementStandAllClr();
    pageValSetRef();
}

void FixMaintenance::pageValSetRef()
{
    while(ui->tableWidget->rowCount()>0){
       ui->tableWidget->removeRow(0);
    }
    rxsMainFrame *psys = rxsMainFrameGet();
    MeasurementJudgment mj;
    const char* mjvn = 0;
    int row=0;
    while ((mjvn = psys->WrkProparam->MeasurementStandForeach(mj)) != 0) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(QString::fromLocal8Bit(mjvn)));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(QString::number(mj.stdVal)));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString::number(mj.pTolerance)));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(QString::number(mj.nTolerance)));
        ui->tableWidget->setItem(row,4,new QTableWidgetItem(QString(mj.enable?"1":"0")));
        row+=1;
    }

    while(ui->thRemap->rowCount()>0){
        ui->thRemap->removeRow(0);
    }
    row = 0;
    char *outname = 0;
    while ((mjvn = psys->WrkProparam->rrpThForeach( &outname,mj.enable)) != 0) {
        ui->thRemap->insertRow(ui->thRemap->rowCount());
        ui->thRemap->setItem(row,0,new QTableWidgetItem(QString::fromLocal8Bit(mjvn)));
        ui->thRemap->setItem(row,1,new QTableWidgetItem(QString::number(mj.stdVal)));
        ui->thRemap->setItem(row,2,new QTableWidgetItem(QString(mj.enable?"1":"0")));
        row+=1;
    }
}


void FixMaintenance::on_allSave_clicked()
{
    rxsMainFrame *psys = rxsMainFrameGet();
    int rs = ui->tableWidget->rowCount();
    MeasurementJudgment mj;
    QByteArray miname;
    for(int x=0;x<rs;x++){
        QTableWidgetItem *qwi1 = ui->tableWidget->item(x,0);
        QTableWidgetItem *qwi2 = ui->tableWidget->item(x,1);
        QTableWidgetItem *qwi3 = ui->tableWidget->item(x,2);
        QTableWidgetItem *qwi4 = ui->tableWidget->item(x,3);
        QTableWidgetItem *qwi5 = ui->tableWidget->item(x,4);
        miname = qwi1->text().toLocal8Bit();
        mj.stdVal = qwi2->text().toFloat();
        mj.pTolerance = qwi3->text().toFloat();
        mj.nTolerance = qwi4->text().toFloat();
        mj.enable = (qwi5->text()==QString("1"));
        psys->WrkProparam->MeasurementStandSet(miname.data(),mj);
    }

    QByteArray newName;
    rs = ui->thRemap->rowCount();
    for(int x=0;x<rs;x++){
        QTableWidgetItem *qwi1 = ui->thRemap->item(x,0);
        QTableWidgetItem *qwi2 = ui->thRemap->item(x,1);
        QTableWidgetItem *qwi3 = ui->thRemap->item(x,2);
        miname = qwi1->text().toLocal8Bit();
        newName = qwi2->text().toLocal8Bit();
        mj.enable = qwi3->text().toInt()==1;
        psys->WrkProparam->rrpThRemapSet(miname.data(),newName.data(),mj.enable);
    }
    Kvpdchr setv={0,0,0};
    setv.val = ui->filterThrev->text().toLocal8Bit().data();
    psys->WrkProparam->Selected(2);
    psys->WrkProparam->DataEditer((unsigned)0,&setv);

    setv.val = ui->outRepName->text().toLocal8Bit().data();
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataEditer(1,&setv);

    setv.val = ui->myFlowPath->text().toLocal8Bit().data();
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataEditer(3,&setv);

    setv.val = ui->modPath->text().toLocal8Bit().data();
    psys->WrkProparam->Selected(1);
    psys->WrkProparam->DataEditer(6,&setv);

    pageValSetRef();
    psys->WrkProparam->Save(0);
}

void FixMaintenance::on_OutPosSet_clicked()
{

}

void FixMaintenance::on_InPosSet_clicked()
{

}


void FixMaintenance::on_AddNew_clicked()
{
    int r = ui->thRemap->rowCount();
    ui->thRemap->insertRow(r);
    ui->thRemap->setItem(r,0,new QTableWidgetItem(QString::fromLocal8Bit("item1")));
    ui->thRemap->setItem(r,1,new QTableWidgetItem(QString::fromLocal8Bit("NewItem")));
    ui->thRemap->setItem(r,2,new QTableWidgetItem(QString::fromLocal8Bit("1")));
}


void FixMaintenance::on_DelSel_clicked()
{
    QList<QTableWidgetItem*> sl = ui->thRemap->selectedItems();
    rxsMainFrame *psys = rxsMainFrameGet();
    for(int x=0;x<sl.count();x++){
        int r = sl[x]->row();
        ui->thRemap->removeRow(r);
        QString miname = ui->thRemap->item(r,0)->text();
        QByteArray qba = miname.toLocal8Bit();
        psys->WrkProparam->rrpThRemove(qba.data());
    }
    pageValSetRef();
}


void FixMaintenance::on_ClrRemap_clicked()
{
    rxsMainFrame *psys = rxsMainFrameGet();
    psys->WrkProparam->rrpThAllClr();
    pageValSetRef();
}

