#include "lasescaner.h"
#include "qtimer.h"
#include "ui_lasescaner.h"
#include "rxsFixdal.h"
#include <QFileDialog>
#include "mainwindow.h"
#include "ccConsole.h"

ImageableSensorUnit *LsrScaner;


LaseScaner::LaseScaner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LaseScaner)
{
    ui->setupUi(this);
    ccConsole::Print("");
    photoLabel = ((MainWindow *)parent)->photoLabel;
    LsrScaner = 0;
    optflag = 0;
    ui->EncTabCB->insertItem(0,QString::fromLocal8Bit("连续触发"));
    ui->EncTabCB->insertItem(1,QString::fromLocal8Bit("外部触发"));
    ui->EncTabCB->insertItem(2,QString::fromLocal8Bit("编码器0"));
    ui->EncTabCB->insertItem(3,QString::fromLocal8Bit("编码器X"));
    ui->EncTabCB->insertItem(4,QString::fromLocal8Bit("编码器Y"));
    ui->EncTabCB->insertItem(5,QString::fromLocal8Bit("编码器3"));
    FillImageableNames(0);
    connect(ui->EncTabCB, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboxSelect(int)));
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->setInterval(20);
    ShowData_ref();
    FillShowLightInfo();
}

LaseScaner::~LaseScaner()
{
    if(timer != 0){
        timer->stop();

        delete timer;
        timer=0;
    }
    delete ui;
}

void LaseScaner::FillImageableNames(QComboBox *isCombox){
    bool isStart = true;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    const char *v = 0;
    ImageableSensorUnit::DevType idt;
    if(isCombox!=0){
        isCombox->clear();
        while((v =  wrkpd->ImageableSensorForeach(isStart,idt))!=0){
            QString Val = QString::fromLocal8Bit(v);
            isCombox->addItem(Val);
        }
    }else{
        ui->sensorList->clear();
        while((v =  wrkpd->ImageableSensorForeach(isStart,idt))!=0){
            QString Val = QString::fromLocal8Bit(v);
            ui->sensorList->addItem(Val);
        }
    }
}

void LaseScaner::mainReadyOpter(){
    on_StopCollection_clicked();
    photoLabel->clearShow();
}


void LaseScaner::ShowData_ref(){
    int CtrSi=0;
    int ChannelNo=0;
    if(LsrScaner == 0){
        return ;
    }

    if(LsrScaner->CtrInfo[CtrSi].TiggerMode == 0){
        ui->EncTabCB->setCurrentIndex(0);
    }
    else if(LsrScaner->CtrInfo[CtrSi].TiggerMode == 2 ||LsrScaner->CtrInfo[CtrSi].TiggerMode == 3){
        ui->EncTabCB->setCurrentIndex(1);
    }else if(LsrScaner->CtrInfo[CtrSi].TiggerMode > 3){
        int enxAx = LsrScaner->CtrInfo[CtrSi].TiggerMode-4;
        if(enxAx == 0){
            ui->EncTabCB->setCurrentIndex(2);
            return;
        }
        if(enxAx == 1){
            ui->EncTabCB->setCurrentIndex(3);
            return;
        }
        if(enxAx == 2){
            ui->EncTabCB->setCurrentIndex(4);
            return;
        }
    }
    ui->Gain->setText(QString::number(LsrScaner->CtrInfo[CtrSi].gain));
    ui->ExporTime->setText(QString::number(LsrScaner->CtrInfo[CtrSi].expTime));
}


void LaseScaner::on_Conn_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    if(!LsrScaner->ResetConnect()){
        ccConsole::Print(QString::fromLocal8Bit("重新打开失败!"));
    }
}

void LaseScaner::CreamGraber(){
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner->Sensorinfo.devType != ImageableSensorUnit::Cream){
        ccConsole::Print(QString::fromLocal8Bit("选中的传感器不是【相机】类型"));
        return;
    }
    if(LsrScaner->Grab(false,1)){
        unsigned ttn=0;
        void *imgData = LsrScaner->DataGet(ttn);
        rxsImg rin={1,imgData,ttn,(unsigned)LsrScaner->CtrInfo[0].Height,(unsigned)LsrScaner->CtrInfo[0].Width};
        photoLabel->setPhoto(QString::fromLocal8Bit("tg"),&rin);
        return;
    }
    ccConsole::Print(QString::fromLocal8Bit("2D采集任务开启失败"));
}

void LaseScaner::lpGraberScaner(){
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner->Sensorinfo.devType == ImageableSensorUnit::Cream){
        ccConsole::Print(QString::fromLocal8Bit("请先选择正确的传感器类型在进行操作!"));
        return;
    }else{
        int gCount=10000;
        MainWindow *mw =(MainWindow *) this->parent();
        mw->db()->clrAllEntities();
        QString num = ui->CollectionPointNum->text();
        if(num.length()>0){
            gCount = num.toInt();
        }
        if(LsrScaner->Grab(true,gCount)){
            timer->start();
            ccConsole::Print(QString::fromLocal8Bit("采集任务已经开始执行,请耐心等待结束..."));
        }else{
            ccConsole::Print(QString::fromLocal8Bit("采集任务开启失败"));
        }
    }
}

void LaseScaner::on_scanTigger_clicked()
{
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    lpGraberScaner();
}

void LaseScaner::on_StopCollection_clicked()
{
    optflag = 0;
    timer->stop();
    if(LsrScaner == 0){
        return;
    }
    if(LsrScaner->Sensorinfo.devType == ImageableSensorUnit::Cream){
        return;
    }
    LsrScaner->Stop();
    ccConsole::Print(QString::fromLocal8Bit("采集任务停止"));
}

void LaseScaner::on_paramSet_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    int CtrSi=0;
    int ChannelNo=0;
    QString GainText = ui->Gain->text();
    QString ExpTimes = ui->ExporTime->text();
    QString Freq = ui->Sample->text();
    //QString MeasureType = ui->MesVal->text();
    if(LsrScaner->CtrInfo == nullptr){return;}
    LsrScaner->ChannelSelectedGet(ChannelNo);
    //LsrScaner->CtrInfo[CtrSi].gain=GainText.toFloat();
    LsrScaner->CtrInfo[CtrSi].expTime = ExpTimes.toFloat();
    //LsrScaner->CtrInfo[CtrSi].sampleFreq = Freq.toInt();
    //LsrScaner->CtrInfo[CtrSi].MeasureType =MeasureType.toInt();
    //LsrScaner->CtrInfo[CtrSi].CaptureMode = ui->CaptureMode->text().toInt();
    //LsrScaner->CtrInfo[CtrSi].TiggerMode = ui->TiggerMode->text().toInt();
    LsrScaner->SetSensorparam();
    ShowData_ref();
}

void LaseScaner::onTimer(){
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
        if((optflag & 1) == 1){
            CreamGraber();
            //QThread::msleep(10);
        }else{
            return;
        }
}

void LaseScaner::on_chwBower_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("点光谱校准文件"),"",tr("hardware files(*.hwc)"));
    if(fileName.isNull()){
        return;
    }
    ui->port_bit->setText(fileName);
}

void LaseScaner::on_GathPrev_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    if(!LsrScaner->HasOpen()){
        WorkProcessDal *wrkpd = WorkProcessDalGet();
        wrkpd->SensorOpen();
    }
    //1.需要先执行扫描
    //2.将扫描的结果 内存数据值传送给 mainwinow 中的显示内容并切换
    int gCount=10;
    QString num = ui->CollectionPointNum->text();
    if(num.length()>0){
        gCount = num.toInt();
    }
    if(LsrScaner->Grab(false,gCount)){
        timer->start();
        ccConsole::Print(QString::fromLocal8Bit("采集任务已经开始执行,请耐心等待结束..."));
    }else{
        ccConsole::Print(QString::fromLocal8Bit("采集任务开启失败"));
    }
}

void LaseScaner::on_cfgBower_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("点光谱配置文件"),"",tr("config files(*.dcfg)"));
    if(fileName.isNull()){
        return;
    }
    ui->ip_com->setText(fileName);
}


void LaseScaner::onComboxSelect(int index)
{
    int mindex = ui->EncTabCB->currentIndex();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(wrkpd ==0){
        ccConsole::Print(QString::fromLocal8Bit("错误：请确认软件已经正确打开!"));
        return;
    }
    if(wrkpd->TiggerTaber(mindex)!=0){
        ccConsole::Print(QString::fromLocal8Bit("触发切换失败!"));
    }
    //on_paramSet_clicked();
}

void LaseScaner::on_dark_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    LsrScaner->expActive(1,0);
}

void LaseScaner::on_numZero_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    LsrScaner->expActive(2,0);
}


void LaseScaner::on_CloseCon_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    LsrScaner->Close();
}


void LaseScaner::on_sensorList_itemClicked(QListWidgetItem *item)
{
    QString sName = item->text();
    QByteArray qba = sName.toLocal8Bit();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    LsrScaner = wrkpd->ImageableSensorGet(qba.data());
    if(LsrScaner ==0){return; }
//    HWND appwnd = (HWND)ui->tdShowWnd->winId();
//    LsrScaner->FrontendDisplayWndPush(&appwnd);
    if(LsrScaner->HasOpen()){
        ui->openStatus->setText(QString::fromLocal8Bit("已打开"));
    }else{
        ui->openStatus->setText(QString::fromLocal8Bit("未开启"));
    }
}


void LaseScaner::on_lsOpen_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    WorkProcessDal *wrpd = WorkProcessDalGet();
    int cbi = ui->lsBSelected->currentIndex();
    CST_LightSource::ColorIndex civ = (CST_LightSource::ColorIndex)GetColorIndex();
    if(cbi<1){
        wrpd->LightOpen(-1,civ);
    }else{
        wrpd->LightOpen(cbi-1,civ);
    }
}

void LaseScaner::on_lsClose_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    WorkProcessDal *wrpd = WorkProcessDalGet();
    int cbi = ui->lsBSelected->currentIndex();
    if(cbi<1){
        wrpd->LightClose(-1,CST_LightSource::None);
        ui->whiteChkbox->setCheckState(Qt::Unchecked);
        ui->redChkbox->setCheckState(Qt::Unchecked);
        ui->blueChkbox->setCheckState(Qt::Unchecked);
        ui->yellowChkbox->setCheckState(Qt::Unchecked);
        ui->redChkboxW1->setCheckState(Qt::Unchecked);
        ui->redChkboxW2->setCheckState(Qt::Unchecked);
        ui->uvChkbox->setCheckState(Qt::Unchecked);
    }else{
        CST_LightSource::ColorIndex civ = (CST_LightSource::ColorIndex)GetColorIndex();
        wrpd->LightClose(cbi-1,civ);
    }
}

void LaseScaner::FillShowLightInfo(){
    ui->lsBSelected->clear();
    ui->lsBSelected->addItem(QString::fromLocal8Bit("All"));
    WorkProcessDal *wrpd = WorkProcessDalGet();
    short num=0;
    char* Blocks=0;
    wrpd->LightInfoGet(num,(u8 *)Blocks);
    for(short x =0;x<4;x++){
        QString xv = QString::fromLocal8Bit("分区%1").arg(x);
        ui->lsBSelected->addItem(xv);
    }
    ui->lsBSelected->setCurrentIndex(0);
}

int LaseScaner::GetColorIndex(){
    if(ui->whiteChkbox->checkState()==Qt::Checked){
        return CST_LightSource::White;
    }
    if(ui->redChkbox->checkState()==Qt::Checked){
        return CST_LightSource::RED;
    }
    if(ui->greenChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Green;
    }
    if(ui->blueChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Blue;
    }
    if(ui->yellowChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Yellow;
    }
    if(ui->redChkboxW1->checkState()==Qt::Checked){
        return CST_LightSource::RED_730;
    }
    if(ui->redChkboxW2->checkState()==Qt::Checked){
        return CST_LightSource::RED_850;
    }
    if(ui->uvChkbox->checkState()==Qt::Checked){
        return CST_LightSource::UV;
    }
    return CST_LightSource::None;
}

int LaseScaner::GetColorIndexAndVal(unsigned char &val){
    QString v = ui->Val_duoguangpu->text();
    val = v.toShort();
    if(ui->whiteChkbox->checkState()==Qt::Checked){
        return CST_LightSource::White;
    }
    if(ui->redChkbox->checkState()==Qt::Checked){
        return CST_LightSource::RED;
    }
    if(ui->greenChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Green;
    }
    if(ui->blueChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Blue;
    }
    if(ui->yellowChkbox->checkState()==Qt::Checked){
        return CST_LightSource::Yellow;
    }
    if(ui->redChkboxW1->checkState()==Qt::Checked){
        return CST_LightSource::RED_730;
    }
    if(ui->redChkboxW2->checkState()==Qt::Checked){
        return CST_LightSource::RED_850;
    }
    if(ui->uvChkbox->checkState()==Qt::Checked){
        return CST_LightSource::UV;
    }
    return CST_LightSource::None;
}



void LaseScaner::on_horizontalSlider_sliderMoved(int position)
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    QString avl = QString::number(position);
    ui->adjfVal->setText(avl);
    WorkProcessDal* wpd = WorkProcessDalGet();
    AdjustFocal *adjF =(AdjustFocal *) wpd->HrdWareObjectGet(0);
    adjF->Set(position);
}


void LaseScaner::on_conGraber_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    if(LsrScaner->Sensorinfo.devType == ImageableSensorUnit::Cream){
        optflag |= 1;
        timer->start();
        return;
    }
}


void LaseScaner::on_crmGraber_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    if(LsrScaner == 0){
        ccConsole::Print(QString::fromLocal8Bit("请先选中需要操作的传感器!"));
        return;
    }
    //CreamGraber();
    rxsMainFrame *psys = rxsMainFrameGet();
    kvpdChr *v = psys->FacParam->KvpRead("数据保存");
    if(v ==0 || v->val == 0){return;}
    photoLabel->Save(v->val);
}

void LaseScaner::on_AutoLCIGrab_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    QString t = ui->intensity->text();
    if(t.length()<0){
        ccConsole::Print(QString::fromLocal8Bit("请先输入需要测试的光强值!"));
        return;
    }
    WorkProcessDal *wrpd = WorkProcessDalGet();
    int v = t.toInt();
    wrpd->AutoSevenLightColorTakephoto(v);
}


void LaseScaner::on_AutoLPGrab_clicked()
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    QString t = ui->intensitys->text();
    if(t.length()<0){
        ccConsole::Print(QString::fromLocal8Bit("请先输入需要测试的光强值组，以','分隔!"));
        return;
    }
    WorkProcessDal *wrpd = WorkProcessDalGet();
    QByteArray qba = t.toLocal8Bit();
    const char *v = qba.data();
    u8 ci = GetColorIndex();
    wrpd->AutoLightIntensityTakephoto(v,CST_LightSource::ColorIndex(ci));
}


void LaseScaner::on_Slider_huangguang_valueChanged(int value)
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    ui->Val_huanguang->setText(QString::number(value));
    WorkProcessDal *wrpd = WorkProcessDalGet();
    u8 val = value;
    wrpd->LightRegulate(1,CST_LightSource::None,val);
}


void LaseScaner::on_Slider_tongzhou_valueChanged(int value)
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    ui->Val_tongzhou->setText(QString::number(value));
    WorkProcessDal *wrpd = WorkProcessDalGet();
    u8 val = value;
    wrpd->LightRegulate(0,CST_LightSource::None,val);
}


void LaseScaner::on_Slider_duoguangpu_valueChanged(int value)
{
    rxsMainFrame *mfper = rxsMainFrameGet();
    if(mfper->userManger->CurUser.Auth < UserManger::ADMIN){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    ui->Val_duoguangpu->setText(QString::number(value));
    unsigned char val=0;
    CST_LightSource::ColorIndex civ = (CST_LightSource::ColorIndex)GetColorIndex();
    if(civ == CST_LightSource::None){
        ccConsole::Print(QString::fromLocal8Bit("请先勾选要打开的颜色索引"));
        return;
    }
    WorkProcessDal *wrpd = WorkProcessDalGet();
    int cbi = ui->lsBSelected->currentIndex();
    if(cbi<1){
        wrpd->LightRegulate(-1,civ,val);
    }else{
        wrpd->LightRegulate(cbi-1,civ,val);
    }
}

