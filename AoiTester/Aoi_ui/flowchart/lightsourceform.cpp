#include "lightsourceform.h"
#include "ui_lightsourceform.h"

LightSourceForm::LightSourceForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::LightSourceForm)
{
    ui->setupUi(this);
}

LightSourceForm::~LightSourceForm()
{
    delete ui;
}


bool LightSourceForm::SetParamValue(void *d) {

    LightSourceStep *lss = (LightSourceStep*) (*itfper)[Curid];
    u8 ttp = lss->lsType();
    if(ttp == 0){
        ui->rdo_channels->setChecked(true);
        ui->rdo_Multispectral->setChecked(false);
    }else if(ttp == 1){
        ui->rdo_channels->setChecked(false);
        ui->rdo_Multispectral->setChecked(true);
    }
    if(d == 0){
        return false;
    }
    msclspParam *p = (msclspParam*)d;
    ui->dms->setText(QString::number(p->dms));
    tableReSet(ttp,p);
    return true;
}

bool LightSourceForm::GetParamValue(void *d) {
    LightSourceStep *lss = (LightSourceStep*) (*itfper)[Curid];
    int ttp=0;
    if(ui->rdo_channels->isChecked()){
        ttp=0;
        lss->lsType(0);
    }
    else if(ui->rdo_Multispectral->isChecked()){
        ttp=1;
        lss->lsType(1);
    }
    if(d == 0){
        return false;
    }
    msclspParam *p = (msclspParam*)d;
    p->dms = ui->dms->text().toInt();
    tableDataGet(ttp,p);
    return true;
}


void LightSourceForm::tableDataGet(int w,msclspParam *ps){
    LightSourceStep *lss = (LightSourceStep*) (*itfper)[Curid];
    lss->LSParamClr();
    for(int x=0;x<ui->tableWidget->rowCount();x++){
        lsValueSet lvs ;
        if(w==0){
            QString v0 = ui->tableWidget->item(x,0)->text();
            QString v1 = ui->tableWidget->item(x,1)->text();
            lvs.ChannelNo = v0.toInt();
            lvs.Value = v1.toInt();
            lvs.curCi = CST_LightSource::ColorIndex::None;
        }else if(w == 1){
            QString v0 = ui->tableWidget->item(x,0)->text();
            QString v1 = ui->tableWidget->item(x,1)->text();
            QString v2 = ui->tableWidget->item(x,2)->text();
            lvs.ChannelNo = v0.toInt();
            lvs.Value = v1.toInt();
            lvs.curCi = CST_LightSource::ColorIndex(v2.toInt());
        }
        lss->mscSet(lvs);
    }
}


void LightSourceForm::tableReSet(int w,msclspParam *ps){
    while(ui->tableWidget->rowCount()>0){
        ui->tableWidget->removeRow(0);
    }
    ui->tableWidget->clear();
    QStringList header;
    if(w==0){
        header<<QString::fromLocal8Bit("通道号")<<QString::fromLocal8Bit("光强值");
        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setHorizontalHeaderLabels(header);
        if(ps == 0){return;}
        for(int x=0;x<ps->setNums;x++){
            lsValueSet lvs = ps->lsVs[x];
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(x,0,new QTableWidgetItem(QString::number(lvs.ChannelNo)));
            ui->tableWidget->setItem(x,1,new QTableWidgetItem(QString::number(lvs.Value)));
        }
    }else if(w == 1){
        header<<QString::fromLocal8Bit("扇区号")<<QString::fromLocal8Bit("颜色索引")<<QString::fromLocal8Bit("光强值");
        ui->tableWidget->setColumnCount(3);
        ui->tableWidget->setHorizontalHeaderLabels(header);
        if(ps == 0){return;}
        for(int x=0;x<ps->setNums;x++){
            lsValueSet lvs = ps->lsVs[x];
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(x,0,new QTableWidgetItem(QString::number(lvs.ChannelNo)));
            ui->tableWidget->setItem(x,1,new QTableWidgetItem(QString::number(lvs.curCi)));
            ui->tableWidget->setItem(x,2,new QTableWidgetItem(QString::number(lvs.Value)));
        }
    }
}

void LightSourceForm::on_rdo_channels_clicked(bool checked)
{
    if(checked){
        tableReSet(0,0);
    }
}


void LightSourceForm::on_rdo_Multispectral_clicked(bool checked)
{
    if(checked){
        tableReSet(1,0);
    }
}


void LightSourceForm::on_Add_clicked()
{
    int rc = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rc);
    if(ui->rdo_channels->isChecked()){
        ui->tableWidget->setItem(rc,0,new QTableWidgetItem(QString::number(rc+1)));
        ui->tableWidget->setItem(rc,1,new QTableWidgetItem(QString::number(255)));
    }
    else if(ui->rdo_Multispectral->isChecked()){
        ui->tableWidget->setItem(rc,0,new QTableWidgetItem(QString::number(rc)));
        ui->tableWidget->setItem(rc,1,new QTableWidgetItem(QString::number(1)));
        ui->tableWidget->setItem(rc,2,new QTableWidgetItem(QString::number(255)));
    }
}


void LightSourceForm::on_Remove_clicked()
{
    int selcr = ui->tableWidget->currentRow();
    if(selcr < 0){
        return;
    }
    ui->tableWidget->removeRow(selcr);
}

