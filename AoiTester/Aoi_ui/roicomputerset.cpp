#include "roicomputerset.h"
#include "ui_roicomputerset.h"
#include "mainwindow.h"

roiComputerSet::roiComputerSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::roiComputerSet)
{
    ui->setupUi(this);
    //QAbstractItemView::ExtendedSelection: 按住ctrl多选
    //QAbstractItemView::SingleSelection: 单选， 默认
    //QAbstractItemView::MultiSelection：点击多选
    //ContiguousSelection 鼠标拖拉多选
    //QAbstractItemView::ContiguousSelection

//    QStringList scpGrpheader;
//    scpGrpheader<<QString::fromLocal8Bit("名称")<<QString::fromLocal8Bit("roi数量")<<QString::fromLocal8Bit("roi区ID");
//    scpGrpheader<<QString::fromLocal8Bit("工具名称")<<QString::fromLocal8Bit("输出报表");
//    ui->FSetList->setColumnCount(5);
//    ui->FSetList->setHorizontalHeaderLabels(scpGrpheader);
//    ui->FSetList->setSelectionBehavior(QAbstractItemView::SelectRows);
//    ui->FSetList->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList roiHead;
    roiHead<<QString::fromLocal8Bit("名称")<<QString::fromLocal8Bit("ID")<<QString::fromLocal8Bit("类型");
    roiHead<<"SX"<<"SY"<<"SZ"<<"EX"<<"EY"<<"EZ";
    ui->roiList->setColumnCount(9);
    ui->roiList->setHorizontalHeaderLabels(roiHead);
    ui->roiList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->roiList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QStringList couldHead;
    couldHead<<QString::fromLocal8Bit("名称")<<QString::fromLocal8Bit("类型");
    couldHead<<QString::fromLocal8Bit("可见状态")<<QString::fromLocal8Bit("选中状态");
    ui->CouldList->setColumnCount(4);
    ui->CouldList->setHorizontalHeaderLabels(couldHead);
    ui->CouldList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->CouldList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    UpdateCouldList();
    UpdateRoiList();
    ToolKitNamesFill();
//    UpdateRRTemp();
//    UpdateRRList();
//    UpdateCompTx();
}

roiComputerSet::~roiComputerSet()
{
    delete ui;
}


void roiComputerSet::ToolKitNamesFill(){
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    unsigned curNums = ui->toolKitList->count();
    unsigned Nums=0;
    const char *v=0;
    while((v=wrkpd->ToolKitForech(Nums))!=0){
        QString txt = QString::fromLocal8Bit(v);
        if(Nums<curNums){
            ui->toolKitList->item(Nums)->setText(txt);
        }else{
            ui->toolKitList->addItem(txt);
        }
        Nums+=1;
    }
}

//void roiComputerSet::UpdateRRTemp(){
//    if(ui->reportTemp->count()>0){
//        ui->reportTemp->clear();
//    }
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    unsigned Nums=0;
//    const char *v=0;
//    while((v=wrkpd->ResultReportTempNameForech(Nums))!=0){
//        QString txt = QString::fromLocal8Bit(v);
//        ui->reportTemp->addItem(txt);
//        Nums+=1;
//    }
//}

//void roiComputerSet::UpdateRRList(){
//    if(ui->reportList->count()>0){
//        ui->reportList->clear();
//    }
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    unsigned Nums=0;
//    const char *v=0;
//    while((v=wrkpd->ResultReportForech(Nums))!=0){
//        QString txt = QString::fromLocal8Bit(v);
//        ui->reportList->addItem(txt);
//        Nums+=1;
//    }
//}

//void roiComputerSet::UpdateCompTx(){
//    int rows = ui->FSetList->rowCount();
//    if(rows > 0){
//        ui->FSetList->clearContents();
//    }
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    int Nums=0;
//    WorkProcessDal::ComputerTx cptx;
//    while(true){
//        cptx = wrkpd->CompTranForech(Nums);
//        if(cptx.Name==0){break;}
//        if((rows-1) < Nums){
//            ui->FSetList->insertRow(Nums);
//        }
//        QTableWidgetItem *txtName = new QTableWidgetItem(QString::fromLocal8Bit(cptx.Name));
//        ui->FSetList->setItem(Nums,0,txtName);
//        QTableWidgetItem *txtRoinums = new QTableWidgetItem(QString::number(cptx.roiNums));
//        ui->FSetList->setItem(Nums,1,txtRoinums);
//        QString ids;
//        if(cptx.roiNums>0 && cptx.roiIDS != 0){
//            ids = QString::number(cptx.roiIDS[0]);
//            for(unsigned x = 1 ;x<cptx.roiNums;x++){
//                ids += (","+QString::number(cptx.roiIDS[x]));
//            }
//        }
//        QTableWidgetItem *txtRois = new QTableWidgetItem(ids);
//        ui->FSetList->setItem(Nums,2,txtRois);
//        QTableWidgetItem *txtTKName = new QTableWidgetItem(QString::fromLocal8Bit(cptx.toolKitName));
//        ui->FSetList->setItem(Nums,3,txtTKName);
//        QTableWidgetItem *txtRRName = new QTableWidgetItem(QString::fromLocal8Bit(cptx.rrName));
//        ui->FSetList->setItem(Nums,4,txtRRName);
//        Nums+=1;
//    }
//}

void roiComputerSet::UpdateCouldList(){
    unsigned curRoiRows = ui->CouldList->rowCount();
    if(curRoiRows>0){
        ui->CouldList->clearContents();
    }
    MainWindow *mui =((MainWindow *)this->parent());
    ccHObject* TopObj = mui->dbRootObject();
    if(TopObj->getParent() == 0){
        unsigned m =0;
        ccHObject* entObj= nullptr;
        ccHObject* cldEnt = nullptr;
        while((entObj= TopObj->getChild(m))!= nullptr){
            unsigned X=0;
            while((cldEnt = entObj->getChild(X))!=nullptr){
                QTableWidgetItem *V1 = new QTableWidgetItem(cldEnt->getName());
                QTableWidgetItem *V2 = 0;
                if(cldEnt->isKindOf(CC_TYPES::POINT_CLOUD)){
                    V2 = new QTableWidgetItem(QString::fromLocal8Bit("点云实体"));
                }else if(cldEnt->isKindOf(CC_TYPES::LABEL_2D)){
                    V2 = new QTableWidgetItem(QString::fromLocal8Bit("2D标签"));
                }else if(cldEnt->isKindOf(CC_TYPES::VIEWPORT_2D_LABEL)){
                    V2 = new QTableWidgetItem(QString::fromLocal8Bit("2D视图标签"));
                }else if (cldEnt->isKindOf(CC_TYPES::IMAGE)){
                    V2 = new QTableWidgetItem(QString::fromLocal8Bit("图片"));
                }else{
                    V2 = new QTableWidgetItem(QString::fromLocal8Bit("其他"));
                }
                QTableWidgetItem *SorH =new QTableWidgetItem(QString::fromLocal8Bit(cldEnt->isDisplayed()?"可见":"隐藏"));
                QTableWidgetItem *isSel = new QTableWidgetItem(QString::fromLocal8Bit(cldEnt->isSelected()?"已选中":"未选中"));
                if(curRoiRows>X){
                    ui->CouldList->setItem(X,0,V1);
                    ui->CouldList->setItem(X,1,V2);
                    ui->CouldList->setItem(X,2,SorH);
                    ui->CouldList->setItem(X,3,isSel);
                }else{
                    ui->CouldList->insertRow(X);
                    ui->CouldList->setItem(X,0,V1);
                    ui->CouldList->setItem(X,1,V2);
                    ui->CouldList->setItem(X,2,SorH);
                    ui->CouldList->setItem(X,3,isSel);
                }
                X++;
            }
            m++;
        }
    }
}

void roiComputerSet::UpdateRoiList(){
    while( ui->roiList->rowCount()>0){
        ui->roiList->removeRow(0);
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *name=0;
    unsigned ID=0;
    u8 Type=0;
    rxsPointCouldp rxsCp[3];
    unsigned index=0;
    while((name = wrkpd->RoiAreaForech(index,ID,Type,rxsCp))!=0){
        QTableWidgetItem *V1 = new QTableWidgetItem(QString::fromLocal8Bit(name));
        QTableWidgetItem *V2 = new QTableWidgetItem(QString::number(ID));
        QTableWidgetItem *V3 = new QTableWidgetItem(QString::number(Type));
        QTableWidgetItem *P1x = new QTableWidgetItem(QString::number(rxsCp[0].x));
        QTableWidgetItem *P1y = new QTableWidgetItem(QString::number(rxsCp[0].y));
        QTableWidgetItem *P1z = new QTableWidgetItem(QString::number(rxsCp[0].z));
        QTableWidgetItem *P2x = new QTableWidgetItem(QString::number(rxsCp[1].x));
        QTableWidgetItem *P2y = new QTableWidgetItem(QString::number(rxsCp[1].y));
        QTableWidgetItem *P2z = new QTableWidgetItem(QString::number(rxsCp[1].z));
//        QTableWidgetItem *P3x = new QTableWidgetItem(QString::number(rxsCp[2].x));
//        QTableWidgetItem *P3y = new QTableWidgetItem(QString::number(rxsCp[2].y));
//        QTableWidgetItem *P3z = new QTableWidgetItem(QString::number(rxsCp[2].z));
        ui->roiList->insertRow(index);
        ui->roiList->setItem(index,0,V1);
        ui->roiList->setItem(index,1,V2);
        ui->roiList->setItem(index,2,V3);
        ui->roiList->setItem(index,3,P1x);
        ui->roiList->setItem(index,4,P1y);
        ui->roiList->setItem(index,5,P1z);
        ui->roiList->setItem(index,6,P2x);
        ui->roiList->setItem(index,7,P2y);
        ui->roiList->setItem(index,8,P2z);
//        ui->roiList->setItem(index,9,P3x);
//        ui->roiList->setItem(index,10,P3y);
//        ui->roiList->setItem(index,11,P3z);
        index+=1;
    }
}

void roiComputerSet::ref_ui_data(){
    UpdateCouldList();
    UpdateRoiList();
//    UpdateRRTemp();
//    UpdateCompTx();
}

void roiComputerSet::on_LookSelected_clicked()
{
    //将选中的roi 区域在 主视图中显示 boundbox
    ((MainWindow *)this->parent())->myClipBoxShow();
}

//void roiComputerSet::on_AddRC_clicked()
//{
//    //新增计算流程，新增前需要先选中对应ROI
//    QByteArray rrrName;
//    QList<QListWidgetItem*> SS = ui->reportList->selectedItems();
//    if(SS.length()<1){
//        ui->msgShow->setText(QString::fromLocal8Bit("新增失败，请先选中对应结果输出报告!"));
//        return;
//    }
//    rrrName = SS[0]->text().toLocal8Bit();
//    SS = ui->toolKitList->selectedItems();
//    if(SS.length()<1){
//        ui->msgShow->setText(QString::fromLocal8Bit("新增失败，请先选中对应的算法工具!"));
//        return;
//    }
//    QByteArray tookitName = SS[0]->text().toLocal8Bit();
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    QByteArray name = ui->flowerName->text().toLocal8Bit();
//    int rowCount = ui->roiList->rowCount();
//    if(rowCount>0){
//        QList<QTableWidgetItem *> qwiList = ui->roiList->selectedItems();
//        QByteArray qba;
//        int sr=0;
//        int sc=0;
//        foreach (QTableWidgetItem * s, qwiList) {
//            if(sr !=  s->row()){
//                sr = s->row();
//                sc = s->column();
//                QString qs = ui->roiList->item(sr,sc)->text();
//                qba = qs.toLocal8Bit();
//                wrkpd->CompTranSet(name.data(),qba.data(),tookitName.data(),rrrName.data());
//            }
//        }
//    }else{
//        wrkpd->CompTranSet(name.data(),0,tookitName.data(),rrrName.data());
//    }
//    UpdateCompTx();
//    ui->flowerName->clear();
//}

//void roiComputerSet::on_AddRep_clicked()
//{
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    QString curSt = ui->reportTemp->currentText();
//    QByteArray tmpName = curSt.toLocal8Bit();
//    QByteArray rrqba = ui->rrName->text().toLocal8Bit();
//    wrkpd->ResultReportCreater(tmpName.data(),rrqba.data());
//    UpdateRRList();
//    ui->rrName->clear();
//}

//void roiComputerSet::on_CfgSave_clicked()
//{
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    wrkpd->CompTranCfgSave();
//}

//void roiComputerSet::on_testCT_clicked()
//{
//    QList<QTableWidgetItem *> ss = ui->FSetList->selectedItems();
//    if(ss.size()>0){
//        MainWindow *mui =((MainWindow *)this->parent());
//        foreach(QTableWidgetItem * s , ss){
//            QString name = s->text();
//            QByteArray qba = name.toLocal8Bit();
//            mui->myComputerFlowTester(qba.data());
//        }
//        return;
//    }
//    ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要测试的流程!"));
//}



void roiComputerSet::on_roiOK_clicked()
{
    QString rn = ui->roiName->text();
    if(rn.length()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先输入一个ROI的名称设定!"));
        return;
    }
    ((MainWindow *)this->parent())->myRoiAreaSet(rn);
    UpdateRoiList();
    ui->roiName->clear();
}

void roiComputerSet::on_roiUnSelected_clicked()
{
    ((MainWindow *)this->parent())->myClipBoxDestory();
}

void roiComputerSet::on_removeRoi_clicked()
{
    QList<QTableWidgetItem *> qls = ui->roiList->selectedItems();
    if(qls.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要删除的ROI!"));
        return;
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    foreach (QTableWidgetItem *s, qls) {
        QString name = s[0].text();
        QByteArray qba = name.toLocal8Bit();
        wrkpd->RoiReomve(qba.data());
    }
    UpdateRoiList();
}


void roiComputerSet::on_delCould_clicked()
{
    MainWindow *mui =((MainWindow *)this->parent());
    mui->unSelectedEntities();
    mui->refreshAll();
    mui->redrawAll();
    QList<QTableWidgetItem *> ss=ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要删除的点云实体!"));
        return;
    }

    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);
    if(ccho ==0){return;}
    mui->db()->removeElement(ccho);
    mui->refreshAll();
    mui->redrawAll();
    UpdateCouldList();
}

void roiComputerSet::on_algTestButton_clicked()
{
    QList<QTableWidgetItem *> ss=ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要删除的点云实体!"));
        return;
    }
    MainWindow *mui =((MainWindow *)this->parent());

    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);

    QList<QListWidgetItem*> SS = ui->toolKitList->selectedItems();
    if(SS.length()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中对应的算法工具!"));
        return;
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(!wrkpd->AlgorithmTester(SS[0]->text().toStdString(),ui->algParams->toPlainText().toStdString())){
        ui->msgShow->setText(QString::fromLocal8Bit("选中算法工具测试失败!"));
    }
}

void roiComputerSet::on_pcHidden_clicked()
{
    QList<QTableWidgetItem *> ss=ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要隐藏的点云!"));
        return;
    }
    MainWindow *mui =((MainWindow *)this->parent());
    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);
    if(ccho ==0){return;}
    ccho->setEnabled(false);
    mui->refreshAll();
    mui->redrawAll();
    UpdateCouldList();
}


void roiComputerSet::on_pcShow_clicked()
{
    QList<QTableWidgetItem *> ss=ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要显示的点云!"));
        return;
    }
    MainWindow *mui =((MainWindow *)this->parent());
    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);
    if(ccho ==0){return;}
    ccho->setEnabled(true);
    mui->refreshAll();
    mui->redrawAll();
    UpdateCouldList();
}


void roiComputerSet::on_UnSelected_clicked()
{
    QList<QTableWidgetItem *> ss=ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中取消选中的点云!"));
        return;
    }
    MainWindow *mui =((MainWindow *)this->parent());
    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);
    if(ccho ==0){return;}
    mui->db()->unselectEntity(ccho);
    mui->refreshAll();
    mui->redrawAll();
    UpdateCouldList();
}

void roiComputerSet::on_SelectedCE_clicked()
{
    QList<QTableWidgetItem *> ss = ui->CouldList->selectedItems();
    if(ss.size()<1){
        ui->msgShow->setText(QString::fromLocal8Bit("请先选中需要选择的点云!"));
        return;
    }

    MainWindow *mui =((MainWindow *)this->parent());
    QString spcname = ss[0]->text();
    ccHObject *ccho = mui->myGetCCObjFromDB(spcname);
    if(ccho ==0){return;}
    mui->db()->selectEntity(ccho);
    mui->refreshAll();
    mui->redrawAll();
    UpdateCouldList();
}

void roiComputerSet::on_clrBtn_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->RoiClr();
    UpdateRoiList();
}


void roiComputerSet::on_absRoiConvert_clicked()
{

    UpdateRoiList();
}


void roiComputerSet::on_relRoiConvert_clicked()
{
    QModelIndexList list = ui->roiList->selectionModel()->selectedRows();
    int sr = list.count();
    if(sr<2){
        QString msg = QString::fromLocal8Bit("Ctrl+鼠标单击可进行多选，必要选中两个条目，同时最后一条为绝对参考!");
        ui->msgShow->setText(msg);
        return;
    }
    int refindex = list.at(sr-1).row();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    std::vector<roiRect> cr;
    for(int x=0;x<sr-1;x++){
        int srci = list.at(x).row();
        roiRect r = wrkpd->RoiConvertAbstoref(refindex,srci);
        cr.push_back(r);
    }
    UpdateRoiList();
}

