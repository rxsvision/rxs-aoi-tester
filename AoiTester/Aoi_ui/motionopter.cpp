#include "motionopter.h"
#include "ui_motionopter.h"
#include "qtimer.h"
#include "ccConsole.h"

MotionOpter::MotionOpter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MotionOpter)
{
    ui->setupUi(this);
    doutMapID = 0;

#if MOTIONDAL_OFF
    mfper = 0;
#else
    mfper = rxsMainFrameGet();
    unsigned anums=0;
    AxisInfo *ais = mfper->AxInfo(anums);
    for(unsigned x=0;x< anums;x++){
        AxisInfoCtr *aic = new AxisInfoCtr(&ais[x],ui->AxsGrp);
        AicGrps.push_back(aic);
        QPoint p = aic->pos();
        p.setY(p.y() + x * aic->height());
        aic->move(p);
        aic->show();
    }

    DiGrp = mfper->DinGrp(DinNum);
    DoGrp = mfper->DoutGrp(DoutNum);
    Dirc.Rows = 2;
    Dirc.Cols = 8;
    Dorc.Rows = 2;
    Dorc.Cols = 8;
    DoutBtn = new QPushButton[DoutNum];
    DinBtn = new QPushButton[DinNum];
    DIOLayout();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(dioUpdate()));
    timer->start(100);
#endif

}

MotionOpter::~MotionOpter()
{
    if(mfper != 0){
        for(int i=0;i<AicGrps.size();i++){
            AxisInfoCtr *aic=AicGrps[i];
            if(aic==0){ continue; }
            delete aic;
            AicGrps[i]=0;
        }
        AicGrps.clear();

        if(doutMapID != 0){
            delete doutMapID;
            doutMapID = 0;
        }
        if(timer != 0){
            delete timer;
            timer =0;
        }
        delete[] DoutBtn;
        delete[] DinBtn;
        DoutBtn = 0;
        DinBtn = 0;
    }
    delete ui;
}


void MotionOpter::DIOLayout(){
    int xSpace = 10;
    int ySpace = 10;
    int Sx=10,Sy=30;
    int btnW = 80;
    int btnH = 30;
    if(doutMapID != 0){
        delete doutMapID;
    }
    doutMapID = new QSignalMapper;

    for(unsigned o = 0;o<DoutNum;o++){
        DoutBtn[o].setParent(ui->Doutput);
        int mod = o%Dorc.Cols;
        int r = o/Dorc.Cols;
        QRect rect(Sx+mod * (xSpace + btnW),Sy + r * (ySpace+btnH),btnW,btnH);
        DoutBtn[o].setGeometry(rect);
        DoutBtn[o].setText(QString::fromLocal8Bit(DoGrp[o].ioName));
        DoutBtn[o].setObjectName("Dout"+QString::number(DoGrp[o].index));
        doutMapID->setMapping(&DoutBtn[o],QString::fromLocal8Bit(DoGrp[o].ioName));
        connect(&DoutBtn[o],SIGNAL(clicked()),doutMapID,SLOT(map()));
    }
    connect(doutMapID,SIGNAL(mapped(QString)),this,SLOT(DoutSignalSet(QString)));

    for(unsigned i = 0;i<DinNum;i++){
        DinBtn[i].setParent(ui->Dinput);
        int mod = i%Dirc.Cols;
        int r = i/Dirc.Cols;
        QRect rect(Sx+mod * (xSpace + btnW),Sy + r * (ySpace+btnH),btnW,btnH);
        DinBtn[i].setGeometry(rect);
        DinBtn[i].setText(QString::fromLocal8Bit(DiGrp[i].ioName));
    }
}

void MotionOpter::DoutSignalSet(QString oName){
    if(mfper->userManger->CurUser.Auth < UserManger::TECH){
        ccConsole::Print(QString::fromLocal8Bit("您无权限操作!"));
        return;
    }
    QByteArray onba = oName.toLocal8Bit();
    const char* okey = onba.data();
    mfper->OutSignelTiggle(okey);
}

void MotionOpter::RefsuhDio(){
    for(unsigned o = 0;o<DoutNum;o++){
        if(DoGrp[o].curStatus){
            DoutBtn[o].setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            DoutBtn[o].setStyleSheet("background-color: rgb(255, 0, 0);");
        }
    }

    for(unsigned i = 0;i<DinNum;i++){
        if(DiGrp[i].curStatus){
            DinBtn[i].setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            DinBtn[i].setStyleSheet("background-color: rgb(255, 0, 0);");
        }
    }
}

void MotionOpter::dioUpdate(){
    if(mfper != 0){
        for(int i=0;i<AicGrps.size();i++){
            AicGrps[i]->ui_refush_dataupdata();
        }
    }
    RefsuhDio();
}

void MotionOpter::on_fixReset_clicked()
{
    mfper->SysReset();
}

void MotionOpter::on_EmgStop_clicked()
{
    mfper->AxStop(0);
}

