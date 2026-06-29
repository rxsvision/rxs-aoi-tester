#include "imgoutparam.h"
#include "ui_imgoutparam.h"
#include <QFileDialog>
#include "imgProcess.h"
#include <iostream>


ImgOutParam::ImgOutParam(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ImgOutParam)
{
    ui->setupUi(this);
    ui->EleListTabe->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->EleSelected->setSelectionBehavior(QAbstractItemView::SelectRows);

    if(itfper != 0){
        int i=0;
        cuOutLinker *cuolr=0;
        while((cuolr = itfper->ForeachCanUseOutLinker(i))!=0){
            i+=1;
            if(cuolr->id==Curid){
                continue;
            }
            int rowNum = ui->EleListTabe->rowCount();
            ui->EleListTabe->insertRow(rowNum);
            ui->EleListTabe->setItem(rowNum, 0, new QTableWidgetItem(QString::number(cuolr->id)));
            ui->EleListTabe->setItem(rowNum, 1, new QTableWidgetItem(QString::fromLocal8Bit(cuolr->Name)));
            ui->EleListTabe->setItem(rowNum, 2, new QTableWidgetItem(QString::number(cuolr->outType)));
        }
    }
}

ImgOutParam::~ImgOutParam()
{
    delete ui;
}


bool ImgOutParam::GetParamValue(void *d) {
    ImgOutParamer *iopr = (ImgOutParamer *)d;
    QString v = ui->SrcBaseImg->text();
    QStringList iov = v.split('-');
    if(iov .length()<2){return false;}
    iopr->srcLinker.id = iov[0].toInt();
    iopr->srcLinker.outChoose = iov[1].toInt();
    QString sp = ui->savePath->text();
    if(sp.length()>0){
        QByteArray qba = sp.toLocal8Bit();
        iopr->savePath = qba.data();
    }else{
        iopr->savePath ="";
    }

    ImgOutStep *iospSbb = (ImgOutStep *)(*itfper)[Curid];
    if(iospSbb ==0){
        return false;
    }
    iospSbb->RenderDataClr();
    int ri = ui->EleSelected->rowCount();
    for(int i =0;i<ri;i++){
        FromLinker fl;
        fl.id = ui->EleSelected->item(i,0)->text().toInt();
        fl.outChoose =ui->EleSelected->item(i,2)->text().toInt();
        iospSbb->RenderDataPush(&fl);
    }
    return true;
}


bool ImgOutParam::SetParamValue(void *d) {
    ImgOutParamer *iopr = (ImgOutParamer *)d;
    QString val = QString::number(iopr->srcLinker.id)+QString::fromLocal8Bit("-")+QString::number(iopr->srcLinker.outChoose);
    ui->SrcBaseImg->setText(val);
    if(iopr->savePath.length()>1){
        ui->savePic->setChecked(true);
        QString p = QString::fromLocal8Bit(iopr->savePath.c_str());
        ui->savePath->setText(p);
    } else {
        ui->savePic->setChecked(false);
        ui->savePath->setText("");
    }
    for(int m =0;m< iopr->renderingNums;m++){
        int ri = ui->EleSelected->rowCount();
        ui->EleSelected->insertRow(ri);
        StepBlockBase *sbb = (*itfper)[iopr->renderingIds[m]->id];
        QTableWidgetItem *qit = new QTableWidgetItem(QString::number( iopr->renderingIds[m]->id));
        ui->EleSelected->setItem(ri,0,qit);
        qit = new QTableWidgetItem(QString::fromLocal8Bit(sbb->Name()));
        ui->EleSelected->setItem(ri,1,qit);
        qit = new QTableWidgetItem(QString::number(iopr->renderingIds[m]->outChoose));
        ui->EleSelected->setItem(ri,2,qit);
    }
    return true;
}

void ImgOutParam::on_savePathSet_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    //qDebug() << "Dir Path:" << selectDir;
    ui->savePath->setText(selectDir);
}


void ImgOutParam::on_srcSelected_clicked()
{
    //table中单选中的 行 内容为
    QList<QTableWidgetSelectionRange> ss = ui->EleListTabe->selectedRanges();
    int rc = ss.count();
    if(rc<1){return;}
    foreach (QTableWidgetSelectionRange s, ss) {
        int r = s.topRow();
        int Csi = s.leftColumn();
        int Cei = s.rightColumn()+1;
        QString val;
        for(int ci=Csi;ci<Cei;ci++){
            if(ci==1){
                val += QString::fromLocal8Bit("-");
            }else{
                val += ui->EleListTabe->item(r,ci)->text();
            }
        }
        ui->SrcBaseImg->setText(val);
        break;
    }
}


void ImgOutParam::on_Addele_clicked()
{
    //table 中选中加入 显示合集中
    QList<QTableWidgetSelectionRange> ss = ui->EleListTabe->selectedRanges();
    int rc = ss.count();
    if(rc<1){return;}
    foreach (QTableWidgetSelectionRange s, ss) {
        int r = s.topRow();
        int r_end = s.bottomRow()+1;
        int Csi = s.leftColumn();
        int Cei = s.rightColumn()+1;
        for(int rj = r;rj < r_end;rj++){
            int ri = ui->EleSelected->rowCount();
            ui->EleSelected->insertRow(ri);
            for(int ci=Csi;ci<Cei;ci++){
                QTableWidgetItem *val =
                    new QTableWidgetItem(ui->EleListTabe->item(rj,ci)->text());
                ui->EleSelected->setItem(ri,ci,val);
            }
        }
    }
}

void ImgOutParam::on_clrSelected_clicked()
{
    while(ui->EleSelected->rowCount()>0){
        ui->EleSelected->removeRow(0);
    }
}
