#include "lambertianphotometricstereo.h"
#include "ui_lambertianphotometricstereo.h"
#include "rxsFixdal.h"
#include <QFileDialog>
#include <iostream>

LambertianPhotometricStereo::LambertianPhotometricStereo(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::LambertianPhotometricStereo)
{
    ui->setupUi(this);
    mType=-1;
    ShowFromSrcLinker(ui->SelectLinker,Curid);
    StepBlockBase *sbb = (*itfp)[Curid];
    QStringList header;
    if(sbb->blkType()== StepBlockBase::IMG_MERG_PROCESS){
        header<<QString::fromLocal8Bit("来源ID")<<QString::fromLocal8Bit("输出索引");
        header<<QString::fromLocal8Bit("曝光时间");
    }else if(sbb->blkType()== StepBlockBase::IMG_LAMBPHOTOMETRIC){
        header<<QString::fromLocal8Bit("来源ID") <<QString::fromLocal8Bit("输出索引");
        header<<QString::fromLocal8Bit("倾斜角") << QString::fromLocal8Bit("俯仰角");
    }
    if(header.size()>0){
        ui->srcLinkTable->setColumnCount(header.size());
        ui->srcLinkTable->setHorizontalHeaderLabels(header);
    }
}

LambertianPhotometricStereo::~LambertianPhotometricStereo()
{
    delete ui;
}

void LambertianPhotometricStereo::imTypeSet(int t){
    mType = t;
}


void LambertianPhotometricStereo::on_dirSelected_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    ui->SelPicDir->setText(selectDir);
}


bool LambertianPhotometricStereo::SetParamValue(void *d) {
    StepBlockBase *sbb = (*itfper)[Curid];
    if(sbb->blkType()== StepBlockBase::IMG_MERG_PROCESS){
        ImgMergingParam *imp = (ImgMergingParam *)d;
        if(imp->imMethod == 0){
            //本地目录
            ui->locPics->setChecked(true);
            ui->srcLiner->setChecked(false);
            QString p = QString::fromLocal8Bit(imp->Method.locDir);
            ui->SelPicDir->setText(p);
        }else{
            ui->locPics->setChecked(false);
            ui->srcLiner->setChecked(true);
            QTableWidgetItem *qwi=0;
            for(int x=0;x<imp->Nums;x++){
                ui->srcLinkTable->insertRow(x);
                qwi = new QTableWidgetItem(QString::number(imp->Method.srcLinker[x].id));
                ui->srcLinkTable->setItem(x,0,qwi);
                qwi = new QTableWidgetItem(QString::number(imp->Method.srcLinker[x].outChoose));
                ui->srcLinkTable->setItem(x,1,qwi);
                if(imp->hf==0){continue;}
                ui->srcLinkTable->setItem(x,2,new QTableWidgetItem(QString::number(imp->hf[x].times)));
            }
        }
        if((imp->outFlag & 1)==1){
            ui->isGrayOut->setChecked(true);
        }else{
            ui->isGrayOut->setChecked(false);
        }
    }else if(sbb->blkType()== StepBlockBase::IMG_LAMBPHOTOMETRIC){
        LambertianPhotoMetriParam *imp = (LambertianPhotoMetriParam *)d;
        LambertianPhotoMetricstereoStep *ims = (LambertianPhotoMetricstereoStep *)sbb;
        if(imp->imMethod == 0){
            //本地目录
            ui->locPics->setChecked(true);
            ui->srcLiner->setChecked(false);
            QString p = QString::fromLocal8Bit(imp->Method.locDir);
            ui->SelPicDir->setText(p);
        }else{
            ui->locPics->setChecked(false);
            ui->srcLiner->setChecked(true);
            QTableWidgetItem *qwi=0;
            for(int x=0;x<imp->Nums;x++){
                ui->srcLinkTable->insertRow(x);
                qwi = new QTableWidgetItem(QString::number(imp->Method.srcLinker[x].id));
                ui->srcLinkTable->setItem(x,0,qwi);
                qwi = new QTableWidgetItem(QString::number(imp->Method.srcLinker[x].outChoose));
                ui->srcLinkTable->setItem(x,1,qwi);
                if(imp->Lcp==0){continue;}
                ui->srcLinkTable->setItem(x,2,new QTableWidgetItem(QString::number(imp->Lcp[x].slant)));
                ui->srcLinkTable->setItem(x,3,new QTableWidgetItem(QString::number(imp->Lcp[x].titl)));
            }
        }
        if((imp->outFlag & 1)==1){
            ui->isGrayOut->setChecked(true);
        }else{
            ui->isGrayOut->setChecked(false);
        }
    }
    return true;
}

bool LambertianPhotometricStereo::GetParamValue(void *d) {
    StepBlockBase *sbb = (*itfper)[Curid];
    if(sbb->blkType() == StepBlockBase::IMG_MERG_PROCESS){
        ImgMergingParam *imp = (ImgMergingParam *)d;
        imp->imType = mType;
        ImgMergingStep *ims = (ImgMergingStep *)sbb;
        if(ui->locPics->isChecked()){
            imp->imMethod = 0;
            QByteArray qba = ui->SelPicDir->text().toLocal8Bit();
            ims->LocDirSet(qba.data());
        }else if(ui->srcLiner->isChecked()){
            imp->imMethod = 1;
            imp->Nums = ui->srcLinkTable->rowCount();
            std::vector<FromLinker> v;
            std::vector<imHdrParam> v2;
            int id = 0, oc = 0;
            float exptimes=0;
            for (int x = 0; x < imp->Nums; x++) {
                id = ui->srcLinkTable->item(x,0)->text().toInt();
                oc = ui->srcLinkTable->item(x,1)->text().toInt();
                exptimes = ui->srcLinkTable->item(x,2)->text().toFloat();
                v.push_back({ id,oc });
                v2.push_back({exptimes});
            }
            ims->FromLinkerSet(&v[0], v.size());
            ims->HdrFiSet(&v2[0],v2.size());
        }
        if(ui->isGrayOut->isChecked()){
            imp->outFlag |= 1;
        }else{
            imp->outFlag &= (~1);
        }
    }else if(sbb->blkType() == StepBlockBase::IMG_LAMBPHOTOMETRIC){
        LambertianPhotoMetriParam *imp = (LambertianPhotoMetriParam *)d;
        LambertianPhotoMetricstereoStep *ims = (LambertianPhotoMetricstereoStep *)sbb;
        if(ui->locPics->isChecked()){
            imp->imMethod = 0;
            QByteArray qba = ui->SelPicDir->text().toLocal8Bit();
            ims->LocDirSet(qba.data());
        }else if(ui->srcLiner->isChecked()){
            imp->imMethod = 1;
            imp->Nums = ui->srcLinkTable->rowCount();
            std::vector<FromLinker> v;
            std::vector<imLbmParam> v2;
            int id = 0, oc = 0;
            float deg = 0,tit = 0;
            for (int x = 0; x < imp->Nums; x++) {
                id = ui->srcLinkTable->item(x,0)->text().toInt();
                oc = ui->srcLinkTable->item(x,1)->text().toInt();
                deg = ui->srcLinkTable->item(x,2)->text().toFloat();
                tit = ui->srcLinkTable->item(x,3)->text().toFloat();
                v.push_back({ id,oc });
                v2.push_back({deg,tit});
            }
            ims->FromLinkerSet(&v[0], v.size());
            ims->LbmSet(&v2[0],v2.size());
        }
        if(ui->isGrayOut->isChecked()){
            imp->outFlag |= 1;
        }else{
            imp->outFlag &= (~1);
        }
    }
    return true;
}

void LambertianPhotometricStereo::on_slAdd_clicked()
{
    FromLinker Flr={0,0};
    SelectLinkerGetFromUi(ui->SelectLinker,&Flr);

    int rc = ui->srcLinkTable->rowCount();
    ui->srcLinkTable->insertRow(rc);
    ui->srcLinkTable->setItem(rc,0,new QTableWidgetItem(QString::number(Flr.id)));
    ui->srcLinkTable->setItem(rc,1,new QTableWidgetItem(QString::number(Flr.outChoose)));
}


void LambertianPhotometricStereo::on_slDel_clicked()
{
    QList<QTableWidgetItem*> ss = ui->srcLinkTable->selectedItems();
    int rc = ss.count();
    if(rc<1){return;}
    foreach (QTableWidgetItem* s, ss) {
        ui->srcLinkTable->removeRow(s->row());
    }
}

