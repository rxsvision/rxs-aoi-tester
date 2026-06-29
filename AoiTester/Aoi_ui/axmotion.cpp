#include "axmotion.h"
#include "qtimer.h"
#include "ui_axmotion.h"
#include "rxsMainFrame.h"

AxisInfo    *AxGrpInfo;

AxMotion::AxMotion(void *pSystem,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AxMotion)
{
    ui->setupUi(this);
    rsxMainFramePtr = pSystem;
    layoutWidgetGrp = 0;
    orgMapID=0;
    jogMapID=0;
    servMapID=0;
    stopMapID=0;
    AxGrpInfo =((rxsMainFrame *) rsxMainFramePtr)->AxInfo(AxNum);
    if(AxNum>0){
        layoutWidgetGrp = new axInfoLayout[AxNum];
        memset(layoutWidgetGrp,0,sizeof(axInfoLayout)*AxNum);
        AxInfoShowLayout();
    }
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(statusUpdate()));
    timer->start(100);
}

AxMotion::~AxMotion()
{
    if(timer != 0){ delete timer; timer = 0;}
    ReleaseLayoutSource();
    if(layoutWidgetGrp!=0){
        delete[] layoutWidgetGrp;
        layoutWidgetGrp=0;
    }
    delete ui;
}

void AxMotion::ReleaseLayoutSource(){
    if(layoutWidgetGrp ==0){return;}
    for(unsigned x = 0;x<AxNum;x++){
        if(layoutWidgetGrp[x].Njog!=0){delete layoutWidgetGrp[x].Njog;layoutWidgetGrp[x].Njog = 0; }
        if(layoutWidgetGrp[x].Pjog!=0){delete layoutWidgetGrp[x].Pjog;layoutWidgetGrp[x].Pjog = 0; }
        if(layoutWidgetGrp[x].Org!=0){delete layoutWidgetGrp[x].Org;layoutWidgetGrp[x].Org = 0; }
        if(layoutWidgetGrp[x].PosShow!=0){delete layoutWidgetGrp[x].PosShow;layoutWidgetGrp[x].PosShow = 0; }
        if(layoutWidgetGrp[x].Sevron!=0){delete layoutWidgetGrp[x].Sevron;layoutWidgetGrp[x].Sevron = 0; }
        if(layoutWidgetGrp[x].Plimt!=0){delete layoutWidgetGrp[x].Plimt;layoutWidgetGrp[x].Plimt = 0; }
        if(layoutWidgetGrp[x].Nlimt!=0){delete layoutWidgetGrp[x].Nlimt;layoutWidgetGrp[x].Nlimt = 0; }
        if(layoutWidgetGrp[x].HomeS!=0){delete layoutWidgetGrp[x].HomeS;layoutWidgetGrp[x].HomeS = 0; }
        if(layoutWidgetGrp[x].GrpBox!=0){delete layoutWidgetGrp[x].GrpBox;layoutWidgetGrp[x].GrpBox = 0; }
    }
    if(orgMapID != 0){ delete orgMapID;orgMapID = 0; }
    if(jogMapID != 0){ delete jogMapID;jogMapID = 0; }
    if(servMapID != 0){delete servMapID;servMapID = 0;}
    if(stopMapID !=0){delete stopMapID;stopMapID = 0;}
}

void AxMotion::AxInfoShowLayout(){
    if(AxNum<1){return;}
    ReleaseLayoutSource();
    ui->HlPanel->setSpacing(20);
    orgMapID = new QSignalMapper();
    jogMapID = new QSignalMapper();
    servMapID = new QSignalMapper();
    stopMapID = new QSignalMapper();
    int w=0,h=0;
    int ySpace = 20;
    if(AxGrpInfo != 0){
    for(unsigned x = 0;x<AxNum;x++){
        layoutWidgetGrp[x].GrpBox = new QGroupBox(ui->horizontalLayoutWidget);
        layoutWidgetGrp[x].GrpBox->setTitle("");
        layoutWidgetGrp[x].GrpBox->setObjectName("Grp_"+QString::number(x));

        w = layoutWidgetGrp[x].GrpBox->width();
        h = layoutWidgetGrp[x].GrpBox->height();

        layoutWidgetGrp[x].Pjog = new QPushButton(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Pjog->setText("+");
        layoutWidgetGrp[x].Pjog->setGeometry(QRect(25,5,35,35));
        jogMapID->setMapping(layoutWidgetGrp[x].Pjog,"+"+QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        stopMapID->setMapping(layoutWidgetGrp[x].Pjog,QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        connect(layoutWidgetGrp[x].Pjog,SIGNAL(pressed()),jogMapID,SLOT(map()));
        connect(layoutWidgetGrp[x].Pjog,SIGNAL(released()),stopMapID,SLOT(map()));

        layoutWidgetGrp[x].Njog = new QPushButton(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Njog->setText("-");
        layoutWidgetGrp[x].Njog->setGeometry(QRect(85,5,35,35));
        jogMapID->setMapping(layoutWidgetGrp[x].Njog,"-"+QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        stopMapID->setMapping(layoutWidgetGrp[x].Njog,QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        connect(layoutWidgetGrp[x].Njog,SIGNAL(pressed()),jogMapID,SLOT(map()));
        connect(layoutWidgetGrp[x].Njog,SIGNAL(released()),stopMapID,SLOT(map()));

        layoutWidgetGrp[x].Org = new QPushButton(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Org->setText(QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        layoutWidgetGrp[x].Org->setGeometry(QRect((w-100)/2,40+ySpace,100,100));
        orgMapID->setMapping(layoutWidgetGrp[x].Org,QString::fromLocal8Bit(AxGrpInfo[x].AxName));
        connect(layoutWidgetGrp[x].Org,SIGNAL(clicked()),orgMapID,SLOT(map()));

        layoutWidgetGrp[x].PosShow = new QLineEdit(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].PosShow->setText(QString::number(0));
        layoutWidgetGrp[x].PosShow->setGeometry(QRect((w-100)/2,140+2*ySpace,100,25));
        layoutWidgetGrp[x].PosShow->setReadOnly(true);

        layoutWidgetGrp[x].Sevron  = new QPushButton(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Sevron->setText("");
        layoutWidgetGrp[x].Sevron->setGeometry(QRect((w-40)/2,165+3*ySpace,40,40));
        servMapID->setMapping(layoutWidgetGrp[x].Sevron,QString::fromLocal8Bit(AxGrpInfo[x].AxName)+"_"+QString::number(x));
        connect(layoutWidgetGrp[x].Sevron,SIGNAL(clicked()),servMapID,SLOT(map()));

        layoutWidgetGrp[x].HomeS = new QLabel(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Plimt = new QLabel(layoutWidgetGrp[x].GrpBox);
        layoutWidgetGrp[x].Nlimt = new QLabel(layoutWidgetGrp[x].GrpBox);

        layoutWidgetGrp[x].HomeS->setGeometry(QRect((w-40)/2,205+4*ySpace,40,40));
        layoutWidgetGrp[x].Plimt->setGeometry(QRect((w-40)/2,245+5*ySpace,40,40));
        layoutWidgetGrp[x].Nlimt->setGeometry(QRect((w-40)/2,285+6*ySpace,40,40));

        layoutWidgetGrp[x].HomeS->setText("0");
        layoutWidgetGrp[x].Plimt->setText("+");
        layoutWidgetGrp[x].Nlimt->setText("-");

        ui->HlPanel->addWidget(layoutWidgetGrp[x].GrpBox);
    }
    connect(orgMapID,SIGNAL(mapped(QString)),this,SLOT(AxOrg(QString)));
    connect(jogMapID,SIGNAL(mapped(QString)),this,SLOT(JogMove(QString)));
    connect(servMapID,SIGNAL(mapped(QString)),this,SLOT(AxServon(QString)));
    connect(stopMapID,SIGNAL(mapped(QString)),this,SLOT(AxMotionStop(QString)));
    }
}

void AxMotion::RefushStatus(){
    if(AxNum<1){return;}
    if(layoutWidgetGrp ==0){return;}
    for(unsigned x = 0;x<AxNum;x++){
        layoutWidgetGrp[x].PosShow->setText(QString::number(AxGrpInfo[x].Status.EncPos));
        if(AxGrpInfo[x].Status.OnServ){
            layoutWidgetGrp[x].Sevron->setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            layoutWidgetGrp[x].Sevron->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        if(AxGrpInfo[x].Status.OnOrg){
            layoutWidgetGrp[x].HomeS->setStyleSheet("background-color: rgb(0, 255, 0);");
            //layoutWidgetGrp[x].HomeS->setBackgroundRole(QColor);
        }else{
            layoutWidgetGrp[x].HomeS->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        if(AxGrpInfo[x].Status.OnLimtN){
            layoutWidgetGrp[x].Nlimt->setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            layoutWidgetGrp[x].Nlimt->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        if(AxGrpInfo[x].Status.OnLimtP){
            layoutWidgetGrp[x].Plimt->setStyleSheet("background-color: rgb(0, 255, 0);");
        }else{
            layoutWidgetGrp[x].Plimt->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
    }
}

void AxMotion::JogMove(QString sVal){
    QByteArray qba = sVal.toLocal8Bit();
    const char *AxName = qba.data();
    ((rxsMainFrame *)rsxMainFramePtr)->AxMove(&AxName[1],AxName[0]);
}

void AxMotion::AxMotionStop(QString AxName){
    QByteArray qba = AxName.toLocal8Bit();
    const char *axName = qba.data();
    ((rxsMainFrame *)rsxMainFramePtr)->AxStop(axName);
}

void AxMotion::AxOrg(QString AxName){
    QByteArray qba = AxName.toLocal8Bit();
    const char *an= qba.data();
    ((rxsMainFrame *)rsxMainFramePtr)->AxHome(an);
}

void AxMotion::AxServon(QString AxName){
    QStringList vs = AxName.split('_');
    QByteArray qba = vs[0].toLocal8Bit();
    const char *an= qba.data();
    int x = vs[1].toInt();
    QColor c = layoutWidgetGrp[x].Sevron->palette().button().color();
    bool onoff= (c == QColor(255,0,0 ))?true:false;
    ((rxsMainFrame *)rsxMainFramePtr)->AxServon(an,onoff);
}

void AxMotion::statusUpdate(){
    RefushStatus();
}
