#include "imgsrcinform.h"
#include "ui_imgsrcinform.h"
#include "imgProcess.h"
#include "rxsFixdal.h"
#include <QFileDialog>

ImgSrcInForm::ImgSrcInForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ImgSrcInForm)
{
    ui->setupUi(this);
    ui->lightSource->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->lightSource->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lightSource->setColumnWidth(3, 60);
    ui->cbSensorSelected->clear();
    bool isStart = true;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *v = 0;
    ImageableSensorUnit::DevType dt;
    while((v =  wrkpd->ImageableSensorForeach(isStart,dt))!=0){
        if(dt!= ImageableSensorUnit::Cream){continue;}
        QString Val = QString::fromLocal8Bit(v);
        ui->cbSensorSelected->addItem(Val);
    }
}

ImgSrcInForm::~ImgSrcInForm()
{
    delete ui;
}

void ImgSrcInForm::on_pushButton_clicked()
{

}


void ImgSrcInForm::on_lightSource_cellDoubleClicked(int row, int column)
{

}

bool ImgSrcInForm::SetParamValue(void *d){
    ImgSourceParam *data =(ImgSourceParam *)d;
    if(data->sType==0){
        ui->locPic->setChecked(false);
        ui->sdkin->setChecked(false);
        ui->sensor->setChecked(true);
        QString sv = QString::fromLocal8Bit(data->inParam.lsp.selectedCream);
        int i= ui->cbSensorSelected->findText(sv);
        if(i<ui->cbSensorSelected->count()){
            ui->cbSensorSelected->setCurrentIndex(i);
        }
    }else if(data->sType==1)
    {
        ui->sensor->setChecked(false);
        ui->sdkin->setChecked(false);
        ui->locPic->setChecked(true);
        ui->LocPicFile->setText(QString::fromLocal8Bit(data->inParam.locReadPath));
    }else{
        ui->locPic->setChecked(false);
        ui->sensor->setChecked(false);
        ui->sdkin->setChecked(true);
    }

    if((data->ssFlag & 1)==1){
        ui->saveSrcPic->setChecked(true);
        if(data->savePath.length()>0){
            ui->savePath->setText(QString::fromLocal8Bit(data->savePath.c_str()));
        }
    }else{
        ui->saveSrcPic->setChecked(false);
        ui->savePath->setText("");
    }
    if((data->ssFlag & 16)  == 16){
        ui->outGray->setChecked(true);
    }else{
        ui->outGray->setChecked(false);
    }
    if((data->ssFlag & 2)==2){
        for(int i=0;i<4;i++){
            int ri = ui->lightSource->rowCount();
            ui->lightSource->insertRow(ri);
            QTableWidgetItem *no = new QTableWidgetItem(QString::number(i));
            ui->lightSource->setItem(ri,0,no);

            if(data->inParam.lsp.Lcp[i].isOn){
                QTableWidgetItem *v = new QTableWidgetItem("true");
                ui->lightSource->setItem(ri,1,v);
            }else{
                QTableWidgetItem *v = new QTableWidgetItem("false");
                ui->lightSource->setItem(ri,1,v);
            }
            QTableWidgetItem *ci = new QTableWidgetItem(QString::number(data->inParam.lsp.Lcp[i].curCi));
            ui->lightSource->setItem(ri,2,ci);
            QTableWidgetItem *cv = new QTableWidgetItem(QString::number(data->inParam.lsp.Lcp[i].curVal));
            ui->lightSource->setItem(ri,3,cv);
        }
    }else{
        ui->lightSource->clearContents();
        for(int i=0;i<4;i++){
            int ri = ui->lightSource->rowCount();
            ui->lightSource->insertRow(ri);
            QTableWidgetItem *no = new QTableWidgetItem(QString::number(i));
            ui->lightSource->setItem(ri,0,no);
            QTableWidgetItem *v = new QTableWidgetItem("true");
            ui->lightSource->setItem(ri,1,v);
            QTableWidgetItem *ci = new QTableWidgetItem(QString::number(1));
            ui->lightSource->setItem(ri,2,ci);
            QTableWidgetItem *cv = new QTableWidgetItem(QString::number(250));
            ui->lightSource->setItem(ri,3,cv);
        }
    }
    return true;
}

bool ImgSrcInForm::GetParamValue(void *d){
    ImgSourceParam *data =(ImgSourceParam *)d;
    if(ui->sensor->isChecked()){
        data->sType = 0;
        int lsrn = ui->lightSource->rowCount();
        memset(&data->inParam,0,sizeof(data->inParam));
        QByteArray sqba = ui->cbSensorSelected->currentText().toLocal8Bit();
        memcpy(data->inParam.lsp.selectedCream,sqba.data(),sqba.size());
        if(lsrn >0){
            for(int x=0;x<4;x++){
                if(x < lsrn){
                    bool on = (ui->lightSource->item(x,1)->text() == "true");
                    data->inParam.lsp.Lcp[x].isOn = on;
                    if(on){
                        int ci = ui->lightSource->item(x,2)->text().toInt();
                        int val = ui->lightSource->item(x,3)->text().toInt();
                        data->inParam.lsp.Lcp[x].curCi = (CST_LightSource::ColorIndex)ci;
                        data->inParam.lsp.Lcp[x].curVal= val;
                    }
                }else{
                    data->inParam.lsp.Lcp[x].isOn = false;
                }
            }
            data->ssFlag |=2;
        }else{
            data->ssFlag &= (~2);
        }
    }
    else if(ui->locPic->isChecked())
    {
        data->sType = 1;
        memset(&data->inParam,0,sizeof(data->inParam));
        QString ifle =  ui->LocPicFile->text();
        QByteArray qba;
        if(ifle.length()>0){
            qba = ifle.toLocal8Bit();
            memcpy(data->inParam.locReadPath,qba.data(),qba.length());
        }else {
            ifle =  ui->LocPicDir->text();
            if(ifle.length()>0){
                qba = ifle.toLocal8Bit();
                memcpy(data->inParam.locReadPath,qba.data(),qba.length());
            }
        }
    }
    else if(ui->sdkin->isChecked()){
        memset(&data->inParam,0,sizeof(data->inParam));
        data->sType = 2;
    }

    if(ui->saveSrcPic->isChecked()){
        data->ssFlag |= 1;
        QByteArray qba = ui->savePath->text().toLocal8Bit();
        data->savePath = qba.data();
    }else{
        data->ssFlag &= (~1);
    }
    if(ui->outGray->isChecked()){
        data->ssFlag |= 16;
    }else{
        data->ssFlag &= (~16);
    }
    return true;
}

void ImgSrcInForm::on_selectFile_clicked()
{
    QString currentOpenDlgFilter;
    QStringList filterStrings;
    filterStrings<<tr("image (*.bmp)" )<<tr("image (*.jpeg)" )<<tr("image (*.jpg)" )<< tr("image (*.png)" ) << tr("All (*.*)" );
    QString currentPath="";
    QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
                                                              QString::fromLocal8Bit("文件选择"),
                                                              currentPath,
                                                              filterStrings.join(";;"),
                                                              &currentOpenDlgFilter,
                                                              QFileDialog::Options());

    if (selectedFiles.isEmpty())
         return;
    ui->LocPicFile->setText(selectedFiles[0]);
}

void ImgSrcInForm::on_selecteDir_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    //qDebug() << "Dir Path:" << selectDir;
    ui->LocPicDir->setText(selectDir);
}
