#include <QFileDialog>
#include "pcsrcinform.h"
#include "ui_pcsrcinform.h"
#include "rxsFixdal.h"


PCSrcInForm::PCSrcInForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::PCSrcInForm)
{
    ui->setupUi(this);
    ui->cbSensorSelected->clear();
    bool isStart = true;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *v = 0;
    ImageableSensorUnit::DevType dt;
    while((v =  wrkpd->ImageableSensorForeach(isStart,dt))!=0){
        if(dt== ImageableSensorUnit::Cream){continue;}
        QString Val = QString::fromLocal8Bit(v);
        ui->cbSensorSelected->addItem(Val);
    }
}

PCSrcInForm::~PCSrcInForm()
{
    delete ui;
}

bool PCSrcInForm::SetParamValue(void *d){
    PointCouldSourceParam *pcsp = (PointCouldSourceParam *)d;
    int rc = 0;
    while((rc = ui->axMotionList->rowCount())>0)
    {
        ui->axMotionList->removeRow(0);
    }
    if((pcsp->outFlag & 1) == 1){
        ui->soutPC->setCheckState(Qt::Checked);
    }else{
        ui->soutPC->setCheckState(Qt::Unchecked);
    }
    if((pcsp->outFlag & 2) == 2){
        ui->soutDeptPIC->setCheckState(Qt::Checked);
    }else{
        ui->soutDeptPIC->setCheckState(Qt::Unchecked);
    }
    if((pcsp->outFlag & 4) == 4){
        ui->pcSaveFile->setCheckState(Qt::Checked);
    }else{
        ui->pcSaveFile->setCheckState(Qt::Unchecked);
    }

    if(pcsp->outSavePath != 0){
        ui->saveDirText->setText(QString::fromLocal8Bit(pcsp->outSavePath));
    }else{
        ui->saveDirText->setText("");
    }

    if(pcsp->sType == 0){
        ui->sensor->setChecked(true);
        ui->sdkin->setChecked(false);
        ui->locPc->setChecked(false);
        if(pcsp->pcParams.Lpcsp.selectedSensor){
            ui->cbSensorSelected->setCurrentText(QString::fromLocal8Bit(pcsp->pcParams.Lpcsp.selectedSensor));
        }
        int ti = pcsp->pcParams.Lpcsp.method;
        ui->methosSelect->setCurrentIndex(ti);
        if(pcsp->pcParams.Lpcsp.cfgPath != 0){
            ui->paramsPath->setText(QString::fromLocal8Bit(pcsp->pcParams.Lpcsp.cfgPath));
        }
        QTableWidgetItem *qwi = 0;
        MotionMovParam mmp = pcsp->pcParams.Lpcsp.mmp;
        for(int x=0;x< mmp.AxNum;x++){
            ui->axMotionList->insertRow(x);
            qwi = new QTableWidgetItem(QString::fromLocal8Bit(mmp.AxisLister[x].AxName));
            ui->axMotionList->setItem(x,0,qwi);
            char v[2]={(char)mmp.AxisLister[x].method,0};
            qwi = new QTableWidgetItem(QString::fromLocal8Bit(v));
            ui->axMotionList->setItem(x,1,qwi);
            qwi = new QTableWidgetItem(QString::number(mmp.AxisLister[x].TageVel));
            ui->axMotionList->setItem(x,2,qwi);
            qwi = new QTableWidgetItem(QString::number(mmp.AxisLister[x].TagePos));
            ui->axMotionList->setItem(x,3,qwi);
            qwi = new QTableWidgetItem(QString::number(mmp.AxisLister[x].acc));
            ui->axMotionList->setItem(x,4,qwi);
            qwi = new QTableWidgetItem(QString::number(mmp.AxisLister[x].dec));
            ui->axMotionList->setItem(x,5,qwi);
        }
    }else if(pcsp->sType == 1){
        ui->sensor->setChecked(false);
        ui->sdkin->setChecked(false);
        ui->locPc->setChecked(true);
        ui->LocPicFile->setText(QString::fromLocal8Bit(pcsp->pcParams.locPath));
    }else{
        ui->locPc->setChecked(false);
        ui->sensor->setChecked(false);
        ui->sdkin->setChecked(true);
    }
    return true;
}


bool PCSrcInForm::GetParamValue(void *d){
    PointCouldSourceParam *data =(PointCouldSourceParam *)d;
    data->outFlag = 0;
    if(ui->soutPC->isChecked()){
        data->outFlag |=1;
    }
    if(ui->soutDeptPIC->isChecked()){
        data->outFlag |=2;
    }
    if(ui->pcSaveFile->isChecked()){
        data->outFlag |=4;
    }
    PointCouldSourceStep *sbb = (PointCouldSourceStep *)((*itfper)[Curid]);
    if(ui->sensor->isChecked()){
        data->sType = 0;
        QByteArray ssn=ui->cbSensorSelected->currentText().toLocal8Bit();
        QByteArray cfgp = ui->paramsPath->text().toLocal8Bit();
        data->pcParams.Lpcsp.selectedSensor = ssn.data();
        data->pcParams.Lpcsp.cfgPath = cfgp.data();
        data->pcParams.Lpcsp.method = ui->methosSelect->currentIndex();
        int nums = ui->axMotionList->rowCount();
        if(nums<1){return false;}
        std::vector<AxMoveparam> svam;
        QByteArray *axnames = new QByteArray[nums];
        for(int x=0 ; x< nums ; x++){
            AxMoveparam amp;
            axnames[x] = ui->axMotionList->item(x,0)->text().toLocal8Bit();
            amp.AxName = axnames[x].data();
            amp.method = ui->axMotionList->item(x,1)->text().toLocal8Bit().data()[0];
            amp.TageVel=ui->axMotionList->item(x,2)->text().toFloat();
            amp.TagePos=ui->axMotionList->item(x,3)->text().toFloat();
            amp.acc=ui->axMotionList->item(x,4)->text().toFloat();
            amp.dec=ui->axMotionList->item(x,5)->text().toFloat();
            svam.push_back(amp);
        }
        sbb->InParamUpdateRefush();
        sbb->AxListCreater(nums,&svam[0]);
        delete[] axnames;
    }else if(ui->locPc->isChecked()){
        data->sType = 1;
        QByteArray lpf = ui->LocPicFile->text().toLocal8Bit();
        data->pcParams.locPath = lpf.data();
        sbb->InParamUpdateRefush();
    }else{
        data->sType = 2;
    }
    QString sdir = ui->saveDirText->text();
    if(sdir.length()>0){
        QByteArray qba_sdir = sdir.toLocal8Bit();
        sbb->outSavePathSet(qba_sdir.data());
    }
    return true;
}

void PCSrcInForm::on_selectFile_clicked()
{
    QString currentOpenDlgFilter;
    QStringList filterStrings;
    filterStrings<<tr("PCD (*.pcd)" )<<tr("BIN (*.bin)" )<<tr("TXT (*.txt)" )<< tr("All (*.*)" );
    QString currentPath = "";
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


void PCSrcInForm::on_selecteDir_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    ui->LocPicDir->setText(selectDir);
}


void PCSrcInForm::on_axAdd_clicked()
{
    int rc = ui->axMotionList->rowCount();
    ui->axMotionList->insertRow(rc);
    QTableWidgetItem *qwi = new QTableWidgetItem(QString::fromLocal8Bit("AOI-X"));
    ui->axMotionList->setItem(rc,0,qwi);
    ui->axMotionList->setItem(rc,1,new QTableWidgetItem(QString::fromLocal8Bit("A")));
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("20.0"));
    ui->axMotionList->setItem(rc,2,qwi);
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("20.0"));
    ui->axMotionList->setItem(rc,3,qwi);
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("100.0"));
    ui->axMotionList->setItem(rc,4,qwi);
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("100.0"));
    ui->axMotionList->setItem(rc,5,qwi);
}


void PCSrcInForm::on_sensorParams_clicked()
{
    QString currentOpenDlgFilter;
    QStringList filterStrings;
    filterStrings<<tr("TXT (*.txt)" )<< tr("All (*.*)" );
    QString currentPath = "";
    QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
                                                              QString::fromLocal8Bit("文件选择"),
                                                              currentPath,
                                                              filterStrings.join(";;"),
                                                              &currentOpenDlgFilter,
                                                              QFileDialog::Options());

    if (selectedFiles.isEmpty())
        return;
    ui->paramsPath->setText(selectedFiles[0]);
}


void PCSrcInForm::on_axDel_clicked()
{
    QList<QTableWidgetItem*> ss = ui->axMotionList->selectedItems();
    int rc = ss.count();
    if(rc<1){return;}
    foreach (QTableWidgetItem* s, ss) {
        ui->axMotionList->removeRow(s->row());
    }
}


void PCSrcInForm::on_savePath_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    ui->saveDirText->setText(selectDir);
}

