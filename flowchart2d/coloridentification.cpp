#include "coloridentification.h"
#include "ui_coloridentification.h"
#include "rxsFixdal.h"
#include <QFileDialog>

ColorIdentification::ColorIdentification(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ColorIdentification)
{
    ui->setupUi(this);

    ui->creamSelected->clear();
    bool isStart = true;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *v = 0;
    ImageableSensorUnit::DevType dt;
    while((v =  wrkpd->ImageableSensorForeach(isStart,dt))!=0){
        if(dt == ImageableSensorUnit::Cream){
            QString Val = QString::fromLocal8Bit(v);
            ui->creamSelected->addItem(Val);
        }
    }

    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

ColorIdentification::~ColorIdentification()
{
    delete ui;
}


bool ColorIdentification::SetParamValue(void *d){
    ColorRecogParam *data =(ColorRecogParam *)d;

    QString dir = QString::fromLocal8Bit(data->rootDir );
    ui->root_Dir->setText(dir);

    dir = QString::fromLocal8Bit(data->objDir );
    ui->obj_Dir->setText(dir);

    ui->lsVal->setText(QString::number(data->lightVal));

    QString sv = QString::fromLocal8Bit(data->selectedCream);
    int i= ui->creamSelected->findText(sv);
    if(i<ui->creamSelected->count()){
        ui->creamSelected->setCurrentIndex(i);
    }

    return true;
}

bool ColorIdentification::GetParamValue(void *d){
    ColorRecogParam *data =(ColorRecogParam *)d;
    int lsv = ui->lsVal->text().toInt();
    data->lightVal = lsv>255 ?255:lsv;
    QByteArray qba;
    QString dir = ui->root_Dir->text();
    if(dir.length()>1){
        qba = dir.toLocal8Bit();
        memcpy(&data->rootDir[0],qba.data(),qba.length());
    }
    dir = ui->obj_Dir->text();
    if(dir.length()>1){
        qba = dir.toLocal8Bit();
        memcpy(&data->objDir[0],qba.data(),qba.length());
    }

    qba = ui->creamSelected->currentText().toLocal8Bit();
    memcpy(&data->selectedCream[0],qba.data(),qba.size());
    return true;
}

void ColorIdentification::on_rootDirSelected_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    ui->root_Dir->setText(selectDir);
}

void ColorIdentification::on_pushButton_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    ui->obj_Dir->setText(selectDir);
}

void ColorIdentification::on_gtr_clicked()
{
    ColorRecognitionStep *crs = (ColorRecognitionStep *)((*itfper)[Curid]);
    QByteArray qba;
    QString dir = ui->root_Dir->text();
    if(dir.length()>1){
        qba = dir.toLocal8Bit();
        const char *path = qba.data();
        crs->rpicGather(path);
    }
}

