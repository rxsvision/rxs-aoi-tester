#include "scopter.h"
#include "ui_scopter.h"
#include "rxsFixdal.h"
#include <QFileDialog>

rxsMainFrame *rxsPtr;
WorkProcessDal *wrkpdr;

scOpter::scOpter(void *mainFramer,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::scOpter)
{
    ui->setupUi(this);
    rxsPtr = (rxsMainFrame *)mainFramer;
    wrkpdr = WorkProcessDalGet();
    ui->hgDim->addItem("X");
    ui->hgDim->addItem("Y");
    ui->hgDim->addItem("Z");

    ui->WrkDView->addItem(QString::fromLocal8Bit("顶/俯视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("底/仰视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("前视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("后视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("左视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("右视图"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("等轴侧"));
    ui->WrkDView->addItem(QString::fromLocal8Bit("反等轴侧"));

    ui->motionMethod->addItem(QString::fromLocal8Bit("点位"));
    ui->motionMethod->addItem(QString::fromLocal8Bit("直线插补"));
    ui->motionMethod->addItem(QString::fromLocal8Bit("圆弧插补"));
    ui->motionMethod->addItem(QString::fromLocal8Bit("随动"));

    //按住ctrl多选
    ui->rrNeedFileds->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->rrAllFileds->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->AxList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    //ui->AxCombiner->setSelectionModel(QAbstractItemView::SingleSelection);
    UpdateValue();
    AllReportFiledFill();
    ReportTempFill();
    AxFill();
    mCombinerFill();
}

scOpter::~scOpter()
{
    delete ui;
}

void scOpter::on_pushButton_clicked()
{
    QString fc = ui->firstColor->text();
    QString sc = ui->secondColor->text();

    if(fc.length() ==0 && sc.length()==0){
        rxsPtr->FacParam->KvpUpdater("高程色","0",0);
    }else{

    }

    QString hgfreq = ui->hgFreq->text();
    QString hgDim =QString::number( ui->hgDim->currentIndex());

    QByteArray qa = hgDim.toLocal8Bit();
    const char* okey = qa.data();
    rxsPtr->FacParam->KvpUpdater("高程维度",okey,0);
}

void scOpter::AllReportFiledFill(){
    unsigned curNums = ui->rrAllFileds->count();
    unsigned Nums=0;
    const char *v=0;
    while((v=wrkpdr->ResultReportFieldForech(Nums))!=0){
        QString txt = QString::fromLocal8Bit(v);
        if(Nums<curNums){
            ui->rrAllFileds->item(Nums)->setText(txt);
        }else{
            ui->rrAllFileds->addItem(txt);
        }
        Nums+=1;
    }
}
void scOpter::AxFill(){
    unsigned AxNum=0;
    AxisInfo *axi=rxsPtr->AxInfo(AxNum);
    if(AxNum>0 && axi != 0){
        if(ui->AxList->count()>0){
            ui->AxList->clear();
        }
        for(unsigned m = 0;m< AxNum;m++){
            ui->AxList->addItem(QString::fromLocal8Bit(axi[m].AxName));
        }
    }
}

void scOpter::mCombinerFill(){
    if(ui->AxCombiner->count()>0){
        ui->AxCombiner->clear();
    }
    const char* V = 0;
    bool isStart = true;
    while((V = wrkpdr->motionCombinerForeach(isStart)) != 0){
        ui->AxCombiner->addItem(QString::fromLocal8Bit(V));
    }
}

void scOpter::mCombinerSalveFill(QString cn){
    if(ui->AxGrpList->count()>0){
        ui->AxGrpList->clear();
    }
    const char *v =0;
    QByteArray qba = cn.toLocal8Bit();
    int x =0;
    while((v = wrkpdr->motionCombinerSalveForeach(qba.data(),x))!=0){
        ui->AxGrpList->addItem(QString::fromLocal8Bit(v));
        x+=1;
    }
}

void scOpter::ReportTempFill(){
    if(ui->rrItems->count()>0)
        ui->rrItems->clear();

    unsigned Nums=0;
    const char *v=0;
    while((v=wrkpdr->ResultReportTempNameForech(Nums))!=0){
        QString txt = QString::fromLocal8Bit(v);
        ui->rrItems->addItem(txt);
        Nums+=1;
    }
}

void scOpter::UpdateValue(){
    kvpdChr *kvpv= rxsPtr->FacParam->KvpRead("高程色");
    if(kvpv != 0){
        if(kvpv->val[0]=='0'){
            ui->firstColor->setText("");
            ui->secondColor->setText("");
        }else{

        }
    }
    kvpv= rxsPtr->FacParam->KvpRead("高程维度");
    if(kvpv != 0){
        if(kvpv->val != 0){
            ui->hgDim->setCurrentIndex(atoi(kvpv->val));
        }
    }

    kvpv=rxsPtr->FacParam->KvpRead("自动高程");
    if(kvpv != 0){
        if(kvpv->val != 0){
            if(atoi(kvpv->val)==0){
                ui->AutoGCT->setCheckState(Qt::Unchecked);
            }else{
                ui->AutoGCT->setCheckState(Qt::Checked);
            }
        }
    }
    kvpv=rxsPtr->FacParam->KvpRead("默认视角");
    if(kvpv != 0){
        if(kvpv->val != 0){
            ui->WrkDView->setCurrentIndex(atoi(kvpv->val));
        }else{
            ui->WrkDView->setCurrentIndex(0);
        }
    }
    kvpv=rxsPtr->FacParam->KvpRead("加工显示");
    if(kvpv != 0){
        if(kvpv->val != 0){
            if(atoi(kvpv->val)==0){
                ui->wrkShow->setCheckState(Qt::Unchecked);
            }else{
                ui->wrkShow->setCheckState(Qt::Checked);
            }
        }
    }
}

void scOpter::on_FilePathSelecte_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("标准点云文件"),"",tr("Text files(*.txt)"));
    if(fileName.isNull()){
        return;
    }
    QByteArray qba = fileName.toLocal8Bit();
    const char *fn1 =qba.constData();
}


void scOpter::on_AutoGCT_stateChanged(int arg1)
{
    //高程图复选框变更
    if(ui->AutoGCT->checkState() == Qt::Checked){
        rxsPtr->FacParam->KvpUpdater("自动高程","1",0);
    }else{
        rxsPtr->FacParam->KvpUpdater("自动高程","0",0);
    }
}

void scOpter::on_wrkShow_stateChanged(int arg1)
{
    //加工3Dview显示
    if(ui->wrkShow->checkState() == Qt::Checked){
        rxsPtr->FacParam->KvpUpdater("加工显示","1",0);
    }else{
        rxsPtr->FacParam->KvpUpdater("加工显示","0",0);
    }
}


void scOpter::on_WrkDView_currentIndexChanged(int index)
{
    //加工运行等轴侧视角
    int val = ui->WrkDView->currentIndex();
    QString v = QString::number(val);
    QByteArray qba = v.toLocal8Bit();
    rxsPtr->FacParam->KvpUpdater("默认视角",qba.data(),0);
}


void scOpter::on_SoutRtl_clicked()
{
    //选中转入
    QList<QListWidgetItem *>Selected = ui->rrAllFileds->selectedItems();
    foreach (QListWidgetItem *si, Selected) {
        QString txt = si->text();
        ui->rrNeedFileds->addItem(txt);
    }
}

void scOpter::on_SinLtr_clicked()
{
    //选中转出
    QList<QListWidgetItem *>Selected = ui->rrNeedFileds->selectedItems();
    foreach (QListWidgetItem *si, Selected) {
        QString txt = si->text();
        ui->rrNeedFileds->removeItemWidget(si);
    }
}

void scOpter::on_NeedFiledclr_clicked()
{
    ui->rrNeedFileds->clear();
}

void scOpter::on_repSetSave_clicked()
{
    //设定保存
    ui->rrAllFileds->clear();
    QList<QListWidgetItem *> SS = ui->rrItems->selectedItems();
    if(SS.length()<1){
        return;
    }
    QString sval = SS[0]->text();
    QByteArray qba = sval.toLocal8Bit();
    const char* tmpName = qba.data();
    int len = ui->rrNeedFileds->count();
    QByteArray filed;
    for(unsigned m = 0;m< len;m++){
        filed = ui->rrNeedFileds->item(m)->text().toLocal8Bit();
        wrkpdr->ResultReportTempFieldAppends(tmpName,filed.data());
    }
    ui->rrNeedFileds->clear();
    ReportTempFill();
    AllReportFiledFill();
}

void scOpter::on_AddReport_clicked()
{
    //新增报表模板
    QString tmpn = ui->tempName->text();
    if(tmpn.length()>0){
        ui->rrItems->addItem(tmpn);
        ui->tempName->setText("");
    }
    return;
}

void scOpter::on_ReportDetail_clicked(QString val)
{
    QByteArray qba = val.toLocal8Bit();
    const char *tmpName = qba.data();

    if(ui->rrNeedFileds->count()>0)
        ui->rrNeedFileds->clear();

    unsigned Nums=0;
    const char *v=0;
    while((v=wrkpdr->ResultReportTempFieldForech(tmpName,Nums))!=0){
        QString txt = QString::fromLocal8Bit(v);
        ui->rrNeedFileds->addItem(txt);
        Nums+=1;
    }
}


void scOpter::on_DelReport_clicked()
{
    //删除条目
}


void scOpter::on_axGrpCreater_clicked()
{
    QString name = ui->grpName->text();
    if(name.length()<1){
        return;
    }
    int mt = ui->motionMethod->currentIndex();
    QList<QListWidgetItem *> sl= ui->AxList->selectedItems();
    int nums = sl.count();
    if(nums<2){
        return ;
    }
    QByteArray mAx = sl[0]->text().toLocal8Bit();
    QByteArray qba = name.toLocal8Bit();
    const char * cn = qba.data();
    if(mt==0){
        wrkpdr->motionCombinerAdd(cn,MotionModel::GROUP_LINE_INTERPOLATION_MASTER,mAx.data(),nums-1);
    }else if(mt==1){
        wrkpdr->motionCombinerAdd(cn,MotionModel::GROUP_ARC_INTERPOLATION_MASTER,mAx.data(),nums-1);
    }else if(mt == 2){
        wrkpdr->motionCombinerAdd(cn,MotionModel::GROUP_FOLLOW_MASTER,mAx.data(),nums-1);
    }else{
        return;
    }
    for (int i =1;i<nums;i++) {
        QByteArray sn = sl[i]->text().toLocal8Bit();
        wrkpdr->motionCombinerAppened(cn,sn.data());
    }
    wrkpdr->motionCombinerSave();
    mCombinerFill();
    ui->grpName->clear();
}


void scOpter::on_axGrpRemover_clicked()
{

}


void scOpter::on_AxCombiner_itemClicked(QListWidgetItem *item)
{
    QString msname = item[0].text();
    mCombinerSalveFill(msname);
}


void scOpter::on_rrItems_itemClicked(QListWidgetItem *item)
{
    QString rrname = item[0].text();
    on_ReportDetail_clicked(rrname);
}

