#include "coloridentification.h"
#include "ui_coloridentification.h"
#include "rxsFixdal.h"
#include <QFileDialog>

ColorIdentification::ColorIdentification(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::ColorIdentification)
{
    ui->setupUi(this);

    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

ColorIdentification::~ColorIdentification()
{
    delete ui;
}


bool ColorIdentification::SetParamValue(void *d){
    ColorRecogParam *data =(ColorRecogParam *)d;

    QString dir = QString::fromLocal8Bit(data->samplePath );
    ui->sampRoot->setText(dir);

    dir = QString::fromLocal8Bit(data->modelPath);
    ui->modRoot->setText(dir);

    dir = QString::fromLocal8Bit(data->gatherPath);
    ui->regPath->setText(dir);
    return true;
}

bool ColorIdentification::GetParamValue(void *d){
    ColorRecogParam *data =(ColorRecogParam *)d;
    ColorRecognitionStep *crs = (ColorRecognitionStep *)((*itfper)[Curid]);

    QString dir = ui->modRoot->text();
    QByteArray mr_qba = dir.toLocal8Bit();

    dir = ui->sampRoot->text();
    QByteArray sr_qba = dir.toLocal8Bit();

    dir = ui->regPath->text();
    QByteArray rp_qba = dir.toLocal8Bit();

    crs->pathSet(sr_qba.data(),mr_qba.data(),rp_qba.data());
    return true;
}


bool ColorIdentification::GetTestResult(void *){
    ColorRecognitionStep *crs = (ColorRecognitionStep *)((*itfper)[Curid]);
    rxsResultReport *rpp = 0;
    crs->GetResult(&rpp);
    if(rpp != 0){
        const char *v = rpp->GetValue(0,1);
        ui->result->setText(QString::fromLocal8Bit(v));
    }
    return true;
}


void ColorIdentification::on_rootDirSelected_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    if(selectDir.size()>0){
        ui->sampRoot->setText(selectDir);
    }
}

void ColorIdentification::on_pushButton_clicked()
{
    QString selectDir = QFileDialog::getExistingDirectory();
    if(selectDir.size()>0){
        ui->modRoot->setText(selectDir);
    }
}

void ColorIdentification::on_gtr_clicked()
{
    ColorRecognitionStep *crs = (ColorRecognitionStep *)((*itfper)[Curid]);
    QString samp = ui->sampRoot->text();
    QString model = ui->modRoot->text();
    if(samp.length()>1 && model.length()>1){
        QByteArray sqba = samp.toLocal8Bit();
        const char *samppath = sqba.data();
        QByteArray mqba = model.toLocal8Bit();
        const char *modelpath = mqba.data();
        crs->modeTrain(samppath,modelpath);
    }
}


void ColorIdentification::on_pushButton_2_clicked()
{
    //路径拾取
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->exec();
    auto selectDir = fileDialog->selectedFiles();
    if (selectDir.size()>0){
        ui->regPath->setText(selectDir[0]);
    }
}

