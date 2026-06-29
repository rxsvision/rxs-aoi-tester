#include "thirdparty.h"
#include "ui_thirdparty.h"
#include "imgProcess.h"
#include <QFileDialog>

Thirdparty::Thirdparty(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::Thirdparty)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
}

Thirdparty::~Thirdparty()
{
    delete ui;
}


bool Thirdparty::SetParamValue(void *d){
    ThridPartyCallBlkParam *data =(ThridPartyCallBlkParam *)d;
    QString val = QString::fromLocal8Bit(data->path);
    ui->InFilePath->setText(val);
    val = QString::fromLocal8Bit(data->inImgName);
    ui->inBlkName->setText(val);
    val = QString::fromLocal8Bit(data->outDataName);
    ui->outBlkName->setText(val);
    val = QString::fromLocal8Bit(data->outImgName);
    ui->outImgBlkName->setText(val);
    return SelectLinkerSetToUi(ui->SelectLinker,&data->srcLinker);
}


bool Thirdparty::GetParamValue(void *d){
    ThridPartyCallBlkParam *data =(ThridPartyCallBlkParam *)d;
    memset(data->path,0,sizeof(data->path));
    memset(data->inImgName,0,sizeof(data->inImgName));
    memset(data->outDataName,0,sizeof(data->outDataName));
    memset(data->outImgName,0,sizeof(data->outImgName));
    QByteArray qba = ui->InFilePath->text().toLocal8Bit();
    memcpy(data->path,qba.data(),qba.length());

    qba = ui->inBlkName->text().toLocal8Bit();
    memcpy(data->inImgName,qba.data(),qba.length());

    qba = ui->outBlkName->text().toLocal8Bit();
    memcpy(data->outDataName,qba.data(),qba.length());

    qba = ui->outImgBlkName->text().toLocal8Bit();
    memcpy(data->outImgName,qba.data(),qba.length());
    return SelectLinkerGetFromUi(ui->SelectLinker,&data->srcLinker);
}

void Thirdparty::on_LoadFileAction_clicked()
{
    QString currentOpenDlgFilter;
    QStringList filterStrings;
    filterStrings<<tr("VM SOL (*.sol)" ) << tr("All (*.*)" );
    QString currentPath="";
    QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
                                                              QString::fromLocal8Bit("文件选择"),
                                                              currentPath,
                                                              filterStrings.join(";;"),
                                                              &currentOpenDlgFilter,
                                                              QFileDialog::Options());

    if (selectedFiles.isEmpty())
        return;
    ui->InFilePath->setText(selectedFiles[0]);
}

