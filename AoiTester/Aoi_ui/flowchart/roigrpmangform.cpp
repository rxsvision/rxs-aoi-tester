#include "roigrpmangform.h"
#include "ui_roigrpmangform.h"
#include <QFileDialog>
#include <iostream>
#include "../rxsFixdal.h"

roiGrpMangForm::roiGrpMangForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::roiGrpMangForm)
{
    ui->setupUi(this);
    ShowFromSrcLinker(ui->SelectLinker,Curid);
    ui->roiGrps->setColumnWidth(0,100);
    ui->roiGrps->setColumnWidth(3,180);
    ui->roiGrps->setColumnWidth(4,180);
    ui->roiGrps->setColumnWidth(5,100);
}

roiGrpMangForm::~roiGrpMangForm()
{
    delete ui;
}

bool roiGrpMangForm::SetParamValue(void *d){
    roiMangeParam *rmp=(roiMangeParam *)d;
    //int nums= ui->roiGrps->columnCount();
    roiTableClr();
    for(int x=0;x<rmp->roiNums ;x++){
        roiRect *rrt = &rmp->roiArea[x];
        roiTableAdd(rrt,x);
        if(fcui != 0){
            fcui->photoLabel->set_RoiArea(rrt->SORC.x,rrt->SORC.y,rrt->LWH.x,rrt->LWH.y);
        }
    }
    return true;
}


bool roiGrpMangForm::GetParamValue(void *d){
    roiMangeParam *rmp=(roiMangeParam *)d;
    roiGrpMangerStep *sbb = (roiGrpMangerStep *)((*itfper)[Curid]);
    rmp->roiNums = ui->roiGrps->rowCount();
    std::vector<roiRect> ris;
    roiTableGet(ris,rmp->roiNums);
    sbb->roiAreaSet(&ris[0],ris.size());
    SelectLinkerGetFromUi(ui->SelectLinker,&rmp->srcLinker);
    return true;
}

void roiGrpMangForm::roiTableClr(){
    while(ui->roiGrps->rowCount()>0){
        ui->roiGrps->removeRow(0);
    }
}

void roiGrpMangForm::roiTableGet(std::vector<roiRect> &data,int rowCount){
    for(int i =0;i<rowCount; i++){
        u8 flag = 0;
        QString v1 = ui->roiGrps->item(i,1)->text();
        QString v2 = ui->roiGrps->item(i,2)->text();
        if(v1 == QString::fromLocal8Bit("是")){
            flag|=1;
        }else{
            flag &= (~1);
        }
        if(v2 == QString::fromLocal8Bit("相对")){
            flag|=2;
        }else{
            flag &= (~2);
        }
        QStringList spos= ui->roiGrps->item(i,3)->text().split(',');
        QStringList co_of = ui->roiGrps->item(i,4)->text().split(',');
        QStringList lwh_of = ui->roiGrps->item(i,5)->text().split(',');
        if(spos.length()<3 || co_of.length()<3 || lwh_of.length()<3){
            return;
        }
        rxsPointCouldp p1={spos.at(0).toFloat(),spos.at(1).toFloat(),spos.at(2).toFloat()};
        rxsPointCouldp p2={co_of.at(0).toFloat(),co_of.at(1).toFloat(),co_of.at(2).toFloat()};
        rxsPointCouldp p3={lwh_of.at(0).toFloat(),lwh_of.at(1).toFloat(),lwh_of.at(2).toFloat()};
        QString rname = ui->roiGrps->item(i,0)->text();
        QString forrin = ui->roiGrps->item(i,6)->text();
        //需要对应的 text()有内容不然会奔溃
        QByteArray rbtn = rname.toLocal8Bit();
        QByteArray forbtn = forrin.toLocal8Bit();
        roiRect ri = {flag,p1,p2,p3,(forbtn.length()>0? forbtn.data():0),(rbtn.length()>0? rbtn.data():0)};
        data.push_back(ri);
    }
}

void roiGrpMangForm::roiTableAdd(roiRect *ra,int x){
    ui->roiGrps->insertRow(x);
    //QTableWidgetItem *riName = new QTableWidgetItem(QString::fromLocal8Bit(ra->rName.c_str()));
    QTableWidgetItem *riName = new QTableWidgetItem(QString::fromStdString(ra->rName.c_str()));
        ui->roiGrps->setItem(x,0,riName);
        if((ra->Flag& 1)== 1){
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("是"));
            ui->roiGrps->setItem(x,1,val);
        }else{
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("否"));
            ui->roiGrps->setItem(x,1,val);
        }
        if((ra->Flag& 2)== 2){
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("相对"));
            ui->roiGrps->setItem(x,2,val);
        }else{
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("绝对"));
            ui->roiGrps->setItem(x,2,val);
        }
        QString sxyzpos = QString::number(ra->SORC.x)+","+QString::number(ra->SORC.y)+","+QString::number(ra->SORC.z);
        QTableWidgetItem *val2 = new QTableWidgetItem(sxyzpos);
        ui->roiGrps->setItem(x,3,val2);

        QString zepos =
            QString::fromLocal8Bit("%1,%2,%3").arg(ra->End.x).arg(ra->End.y).arg(ra->End.z);
        QTableWidgetItem *val3 = new QTableWidgetItem(zepos);
        ui->roiGrps->setItem(x,4,val3);

        QString lwh =
            QString::fromLocal8Bit("%1,%2,%3").arg(ra->LWH.x).arg(ra->LWH.y).arg(ra->LWH.z);
        QTableWidgetItem *val4 = new QTableWidgetItem(lwh);
        ui->roiGrps->setItem(x,5,val4);

        std::cout<<ra->forRi<<std::endl;
        QTableWidgetItem *val5 = new QTableWidgetItem( QString::fromStdString(ra->forRi.c_str()));
        ui->roiGrps->setItem(x,6,val5);
}

void roiGrpMangForm::on_addNew_clicked()
{
    //int nums= ui->roiGrps->columnCount();
    int row = ui->roiGrps->rowCount();
    ui->roiGrps->insertRow(row);
    QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("roi-%1").arg(row));
    ui->roiGrps->setItem(row,0,val);
    ui->roiGrps->setItem(row,1,new QTableWidgetItem(QString::fromLocal8Bit("是")));
    ui->roiGrps->setItem(row,2,new QTableWidgetItem(QString::fromLocal8Bit("绝对")));
    ui->roiGrps->setItem(row,3,new QTableWidgetItem(QString::fromLocal8Bit("0,0,0")));
    ui->roiGrps->setItem(row,4,new QTableWidgetItem(QString::fromLocal8Bit("0,0,0")));
    ui->roiGrps->setItem(row,5,new QTableWidgetItem(QString::fromLocal8Bit("0,0,0")));
    ui->roiGrps->setItem(row,6,new QTableWidgetItem(QString::fromLocal8Bit("none")));
}


void roiGrpMangForm::on_delSel_clicked()
{
    int curSelIndex = ui->roiGrps->currentRow();
    ui->roiGrps->removeRow(curSelIndex);
}


void roiGrpMangForm::on_techImp_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    int index = 0;
    const char *name = 0;
    u8 raType = 0;
    unsigned id = 0;
    rxsPointCouldp rbpa[3]={{0},{0},{0}};
    roiTableClr();
    while((name = wrkpd->RoiAreaForech(index,id,raType,rbpa))!=0)
    {
        roiRect ra;
        ra.rName = name;
        ra.SORC = rbpa[0];
        ra.End = rbpa[1];
        ra.LWH = rbpa[2];
        ra.Flag = raType;
        ra.forRi = QString::fromLocal8Bit("").toStdString();
        roiTableAdd(&ra,index);
        index+=1;
    }
}


void roiGrpMangForm::on_clrBtn_clicked()
{
    roiTableClr();
}


void roiGrpMangForm::on_impTech_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->RoiClr();
    std::vector<roiRect> ris;
    roiTableGet(ris,ui->roiGrps->rowCount());
    rxsPointCouldp sep[3]={{0},{0},{0}};
    for(int x=0;x<ris.size();x++){
        sep[0]=ris[x].SORC;
        sep[1]=ris[x].End;
        sep[2]=ris[x].LWH;
        wrkpd->RoiAreaSet(ris[x].rName.c_str(),ris[x].Flag,sep,0,0);
    }
}
