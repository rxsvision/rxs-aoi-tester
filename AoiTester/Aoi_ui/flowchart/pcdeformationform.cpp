#include "pcdeformationform.h"
#include "ui_pcdeformationform.h"
#include <QFileDialog>

PcDeformationForm::PcDeformationForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::PcDeformationForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
    ShowFromSrcLinker(ui->boundLinker,Curid);
    ui->resultReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultReport->setSelectionMode(QAbstractItemView::SingleSelection);
}

PcDeformationForm::~PcDeformationForm()
{
    delete ui;
}

bool PcDeformationForm::SetParamValue(void *d){
    PointCouldDeformationParam *pcdp = (PointCouldDeformationParam *)d;
    int i = pcdp->dir;
    ui->dirSelected->setCurrentIndex(i);
    SelectLinkerSetToUi(ui->SelectLinker,&pcdp->srcLinker);
    SelectLinkerSetToUi(ui->boundLinker,&pcdp->boundLinker);
    if(pcdp->modefile!=0){
        QString mfp = QString::fromLocal8Bit(pcdp->modefile);
        ui->modelFilePath->setText(mfp);
    }
    ui->Px1->setText(QString::number(pcdp->ModeBound[0].x));
    ui->Py1->setText(QString::number(pcdp->ModeBound[0].y));
    ui->Pz1->setText(QString::number(pcdp->ModeBound[0].z));
    ui->Px2->setText(QString::number(pcdp->ModeBound[1].x));
    ui->Py2->setText(QString::number(pcdp->ModeBound[1].y));
    ui->Pz2->setText(QString::number(pcdp->ModeBound[1].z));
    ShowRRLIst_Val(ui->resultReport);
    return true;
}


bool PcDeformationForm::GetParamValue(void *d){
    PointCouldDeformationParam *pcdp = (PointCouldDeformationParam *)d;
    pcdp->dir = ui->dirSelected->currentIndex();
    QByteArray qba = ui->modelFilePath->text().toLocal8Bit();
    const char *mpf = qba.data();
    pcdp->ModeBound[0]={ui->Px1->text().toFloat(),ui->Py1->text().toFloat(),ui->Pz1->text().toFloat()};
    pcdp->ModeBound[1]={ui->Px2->text().toFloat(),ui->Py2->text().toFloat(),ui->Pz2->text().toFloat()};
    PointCouldDeformationStep *sbb = (PointCouldDeformationStep *)((*itfper)[Curid]);
    sbb->ModelFilePathSet(mpf);
    SelectLinkerGetFromUi(ui->boundLinker,&pcdp->boundLinker);
    return SelectLinkerGetFromUi(ui->SelectLinker,&pcdp->srcLinker);
}

void PcDeformationForm::on_modefileBtn_clicked()
{
    QString currentOpenDlgFilter;
    QStringList filterStrings;
    filterStrings<<tr("PCD (*.pcd)" )<<tr("BIN (*.bin)" )<<tr("TXT (*.txt)" );
    QString currentPath;
    QStringList selectedFiles = QFileDialog::getOpenFileNames(this,
                                                              QString::fromLocal8Bit("匹配模型选择"),
                                                              currentPath,
                                                              filterStrings.join(";;"));

    if (selectedFiles.isEmpty())
        return;
    ui->modelFilePath->setText(selectedFiles[0]);
}

