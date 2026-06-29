#include "facparamset.h"
#include "ui_facparamset.h"
#include "rxsFixdal.h"

FacParamSet::FacParamSet(void *MainFrame,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FacParamSet)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
    mainFramePtr = MainFrame;
    QStringList kvpheader;
    kvpheader<<QString::fromLocal8Bit("名称")<<QString::fromLocal8Bit("内容")<<QString::fromLocal8Bit("参数描述");
    ui->kvpData->setColumnCount(3);
    ui->kvpData->setHorizontalHeaderLabels(kvpheader);
    ui->kvpData->setColumnWidth(0,100);
    ui->kvpData->setColumnWidth(1,500);
    ui->kvpData->setColumnWidth(2,500);

    QStringList header;
    header<<QString::fromLocal8Bit("加速度")<<QString::fromLocal8Bit("减速度")<<QString::fromLocal8Bit("运行速度");
        header<<QString::fromLocal8Bit("回零速度")<<QString::fromLocal8Bit("校零速度")<<QString::fromLocal8Bit("回零模式");
            header<<QString::fromLocal8Bit("校零距离")<<QString::fromLocal8Bit("回零方向")<<QString::fromLocal8Bit("脉冲转换量");
                header<<QString::fromLocal8Bit("回零偏移");/*<<QString::fromLocal8Bit("回零方向")<<QString::fromLocal8Bit("脉冲转换量");*/
    ui->motionData->setColumnCount(12);
    ui->motionData->setHorizontalHeaderLabels(header);

    ui->motionData->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->kvpData->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只能单选
    ui->motionData->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->kvpData->setSelectionMode(QAbstractItemView::SingleSelection);
    LoadParam();
}

FacParamSet::~FacParamSet()
{
    delete ui;
}

void FacParamSet::on_InsertData_clicked()
{
    if(ui->radioKVP->isChecked()){
        int rowNum = ui->kvpData->rowCount();
        ui->kvpData->insertRow(rowNum);
        ui->kvpData->setItem(rowNum,0,new QTableWidgetItem("param"));
        ui->kvpData->setItem(rowNum,1,new QTableWidgetItem(QString::number(0)));
        ui->kvpData->setItem(rowNum,2,new QTableWidgetItem("对参数的说明描述"));
        return;
    }
    if(ui->radioMD->isChecked()){
        int rowNum = ui->motionData->rowCount();
        ui->motionData->insertRow(rowNum);
        return;
    }
}


void FacParamSet::on_DeleteData_clicked()
{
    int CurSelectedRowIndex=0;
    if(ui->radioKVP->isChecked()){
        CurSelectedRowIndex = ui->kvpData->currentRow();
        ui->kvpData->removeRow(CurSelectedRowIndex);
        return;
    }
    if(ui->radioMD->isChecked()){
        CurSelectedRowIndex = ui->motionData->currentRow();
        ui->motionData->removeRow(CurSelectedRowIndex);
        return;
    }
}


void FacParamSet::on_save_clicked()
{
    FactoryConfigManger *facParam = ((rxsMainFrame*) mainFramePtr)->FacParam;
    if(facParam == NULL){return ;}
    int rowNum = ui->kvpData->rowCount()-1;
    while(rowNum>-1){
        kvpdChr kvp={0,0,0};
        QString qkey = ui->kvpData->item(rowNum,0)->text();
        QString qval = ui->kvpData->item(rowNum,1)->text();
        QString qdes = ui->kvpData->item(rowNum,2)->text();

        QByteArray keyba = qkey.toLocal8Bit();
        kvp.key = keyba.data();
        QByteArray valba = qval.toLocal8Bit();
        kvp.val = valba.data();
        QByteArray desba = qdes.toLocal8Bit();
        kvp.des = desba.data();
        facParam->KvpUpdater(kvp.key,kvp.val,kvp.des);
        rowNum-=1;
    }
    rowNum= ui->motionData->rowCount();
    int colNums = ui->motionData->columnCount();
    for(unsigned x=0;x<rowNum;x++){
        for(unsigned j =0;j<colNums;j++){
            QString qval = ui->motionData->item(x,j)->text();
            QByteArray valba = qval.toLatin1();
            const char *val = valba.constData();
            facParam->MotionParamUpdater(x,j,val);
        }
    }
    facParam->Save();
    WorkProcessDal *wpd = WorkProcessDalGet();
    wpd->reLoadMotionParamSet();
    LoadParam();
}


void FacParamSet::LoadParam(){
    ui->kvpData->clearContents();
    ui->motionData->clearContents();
    int colNum = 0;
    int curRowCount=0;
    int row = 0;
    const char *val =0;
    FactoryConfigManger *facParam = ((rxsMainFrame*) mainFramePtr)->FacParam;
    if(facParam == NULL){return;}
    curRowCount = ui->motionData->rowCount();
    while ((colNum = facParam->SelectedRow(row)) > 0) {
        if((curRowCount-1) < row){
            ui->motionData->insertRow(row);
        }
        for (unsigned x = 0; x < colNum; x++) {
            val = (*facParam)[x];
            QTableWidgetItem *qval = new QTableWidgetItem(QString(val));
            ui->motionData->setItem(row, x, qval);
        }
        row += 1;
    }
    row = 0;
    kvpdChr *kvp=0;
    curRowCount = ui->kvpData->rowCount();
    facParam->KvpReadForech(true);
    while((kvp = facParam->KvpReadForech(false))!=0){
        if((curRowCount-1) < row){
            ui->kvpData->insertRow(row);
        }
        QString key = QString::fromLocal8Bit(kvp->key);
        QString val = QString::fromLocal8Bit(kvp->val);
        QString des = QString::fromLocal8Bit(kvp->des);
        QTableWidgetItem *qkey = new QTableWidgetItem(key);
        QTableWidgetItem *qval = new QTableWidgetItem(val);
        QTableWidgetItem *qdes = new QTableWidgetItem(des);
        ui->kvpData->setItem(row, 0, qkey);
        ui->kvpData->setItem(row, 1, qval);
        ui->kvpData->setItem(row, 2, qdes);
        row+=1;
    }
}

void FacParamSet::ref_ui_data(){
    LoadParam();
}
