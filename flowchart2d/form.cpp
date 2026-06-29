#include "form.h"
#include "ui_form.h"
#include "imgsrcinform.h"
#include "imgoutparam.h"
#include "lambertianphotometricstereo.h"
#include "hdrparam.h"
#include "thirdparty.h"
#include "coloridentification.h"
#include "conditionscomparison.h"
#include "dataoutset.h"
#include "scancodeform.h"
#include "motionmovparamform.h"
#include "usecustomform.h"
#include "convertformatform.h"
#include "pcsrcinform.h"
#include "pcoutform.h"
#include "pcfilterform.h"
#include "pcarclenform.h"
#include "pcpoaslform.h"
#include "roigrpmangform.h"



Form::Form(StepBlockBase * a,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    curBlkName = QString::fromLocal8Bit(a->Name());
    rui = 0;
    ui->blkName->setText(curBlkName);
    sbb = a;
    mainFcer =(FlowChart *)(parent);
    itfpr = mainFcer->itfp;
    Vboxlayout = new QVBoxLayout(ui->inParamPanel);
    ui->inParamPanel->setLayout(Vboxlayout);
    ShowHasParamPage();
}

Form::~Form()
{
    if(Vboxlayout != 0) {
        delete Vboxlayout;
        Vboxlayout = 0;
    }

    unsigned sbt = sbb->blkType();
    if(rui != 0){
        if(sbt ==StepBlockBase::IMG_IN_SOURCE ){
            delete (ImgSrcInForm*)rui;
            rui = 0;
        }else if(sbt ==StepBlockBase::IMG_OUT ){
            delete (ImgOutParam*)rui;
            rui = 0;
        }else if(sbt ==StepBlockBase::THRD_PROCESS ){
            delete (Thirdparty*)rui;
            rui = 0;
        }else if(sbt ==StepBlockBase::IMG_SCANCODE ){
            delete (ScanCodeForm*)rui;
            rui = 0;
        }else if(sbt == StepBlockBase::COLORREG_UCM_PROCESS){
            delete (ColorIdentification *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::IMG_UCUSTOM_PROCESS){
            delete (UseCustomForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::MOTION_MOV){
            delete (motionmovparamform *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::CONVERT_DEPIMG_PC){
            delete (ConvertFormatForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_IN_SOURCE){
            delete (PCSrcInForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_OUT){
            delete (pcOutForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_FILTER){
            delete (pcFilterForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_ARC){
            delete (pcArclenForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_POA_SL){
            delete (pcPoaslForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::ROIGRPMANGER){
            delete (roiGrpMangForm *)rui;
            rui=0;
        }
    }
    delete ui;
}


void *Form::toSaveStruct(){
    return 0;
}

void Form::ShowHasParamPage(){
    unsigned sbt = sbb->blkType();
    switch(sbt){
        case StepBlockBase::IMG_IN_SOURCE:
        {
        ImgSourceBlkParamCreate();
        ImgSourceStep *ist = (ImgSourceStep *)sbb;
        ImgSourceParam *isp = ist->ImgSourceInputParamGet();
        if(isp != 0){
            rui->SetParamValue(isp);
        }

        }
        break;

        case StepBlockBase::IMG_OUT:
        {
        ImgOutBlkParamCreate();
        ImgOutStep *ios =(ImgOutStep *)sbb;
        ImgOutParamer *iop = ios->ImgOutInputParamGet();
        if(iop != 0){
            rui->SetParamValue(iop);
        }
        }
        break;

        case StepBlockBase::THRD_PROCESS:
        {
        ThridPartBlkParamCreate();
        ThridPartyCallBlk *tpcb = (ThridPartyCallBlk *)sbb;
        ThridPartyCallBlkParam *tpcbp = tpcb->ThridPartyInputParamGet();
        if(tpcbp != 0){
            rui->SetParamValue(tpcbp);
        }
        }
        break;

        case StepBlockBase::COLORREG_UCM_PROCESS:
        {
        ColorBlkParamCreate();
        ColorRecognitionStep *crs = (ColorRecognitionStep *)sbb;
        ColorRecogParam *crp = crs->ColorRecognitionParamGet();
        if(crp !=0 ){
            rui->SetParamValue(crp);
        }
        }
        break;

        case StepBlockBase::IMG_SCANCODE:
        {
        ScanCodeParamCreate();
        ScanCodeStep *scs = (ScanCodeStep *)sbb;
        ScanCodeParam *scp = scs->ScanCodeInputParamGet();
        if(scp !=0 ){
            rui->SetParamValue(scp);
        }
        }
        break;

        case StepBlockBase::MOTION_MOV:
        {
            MotionMoveParamCreate();
            MotionMovStep *mms = (MotionMovStep *)sbb;
            MotionMovParam *mmp = mms->MotionMovParamParamGet();
            if(mmp != 0){
                rui->SetParamValue(mmp);
            }
        }
        break;
        case StepBlockBase::CONVERT_DEPIMG_PC:
        {
            FormatConvertParamCreate();
            FormatConvertStep *fcs = (FormatConvertStep *)sbb;
            FormatConvertParam *fcp = fcs->FormatConvertParamGet();
            if(fcp != 0){
                rui->SetParamValue(fcp);
            }
        }
        break;
        case StepBlockBase::IMG_UCUSTOM_PROCESS:
        {
            UseCustomParamCreate();
            UserCustomStep *ucs = (UserCustomStep *)sbb;
            UseCustomParam *ucp = ucs->UserCustomParamGet();
            if(ucp != 0){
                rui->SetParamValue(ucp);
            }
        }
        break;

        case StepBlockBase::PC_IN_SOURCE:
        {
            PointCouldSourceParamCreate();
            PointCouldSourceStep *pcss = (PointCouldSourceStep *)sbb;
            PointCouldSourceParam *pcsp = pcss->PointCouldSourceParamGet();
            if(pcsp != 0){
                rui->SetParamValue(pcsp);
            }
        }
        break;

        case StepBlockBase::PC_OUT:
        {
            PointCouldOutParamCreate();
            PointCouldOutStep *pcos = (PointCouldOutStep *)sbb;
            PointCouldOutParam *pcop = pcos->PointCouldOutParamGet();
            if(pcop != 0){
                rui->SetParamValue(pcop);
            }
        }
        break;

        case StepBlockBase::PC_FILTER:
        {
            PointCouldFilterParamCreate();
            PointCouldFilterStep *pcfs = (PointCouldFilterStep *)sbb;
            PointCouldFilterParam *ucp = pcfs->PointCouldFilterParamGet();
            if(ucp != 0){
                rui->SetParamValue(ucp);
            }
        }
        break;
        case StepBlockBase::ROIGRPMANGER:
        {
            roiMangerParamCreate();
            roiGrpMangerStep *rgms = (roiGrpMangerStep *)sbb;
            roiMangeParam *rmp = rgms->roiGrpParamGet();
            if(rmp != 0){
                rui->SetParamValue(rmp);
            }
        }
        break;
        case StepBlockBase::PC_ARC:
        {
            PointCouldArclenParamCreate();
            PointCouldArclenStep *pcas = (PointCouldArclenStep *)sbb;
            PointCouldArclenParam *pcap = pcas->PointCouldArclenParamGet();
            if(pcap != 0){
                rui->SetParamValue(pcap);
            }
        }
        break;
        case StepBlockBase::PC_POA_SL:
        {
            PointCouldPoaslParamCreate();
            PointCouldPoaslStep *pcps = (PointCouldPoaslStep *)sbb;
            PointCouldPoaslParam *pcpp = pcps->PointCouldPoaslParamGet();
            if(pcpp != 0){
                rui->SetParamValue(pcpp);
            }
        }
        break;

        default:
        break;
        }
}

void Form::DataOutParamCreate(){
    DataOutSet *dos = new DataOutSet(itfpr,sbb->ID(),ui->inParamPanel);
    rui = dos;
    Vboxlayout->addWidget(dos);
}

void Form::ConditionsBlkParamCreate(){
    ConditionsComparison *cc = new ConditionsComparison(itfpr,sbb->ID(),ui->inParamPanel);
    rui = cc;
    Vboxlayout->addWidget(cc);
}

void Form::ColorBlkParamCreate(){
    ColorIdentification *ci = new ColorIdentification(itfpr,sbb->ID(),ui->inParamPanel);
    rui = ci;
    Vboxlayout->addWidget(ci);
}

void Form::HdrBlkParamCreate(){
    hdrParam *hdr = new hdrParam(itfpr,sbb->ID(),ui->inParamPanel);
    rui = hdr;
    Vboxlayout->addWidget(hdr);
}

void Form::LambertianPhotometricStereoBlkParamCreate(){
    LambertianPhotometricStereo *lps = new LambertianPhotometricStereo(itfpr,sbb->ID(),ui->inParamPanel);
    rui = lps;
    Vboxlayout->addWidget(lps);
}

void Form::MotionMoveParamCreate(){
    motionmovparamform *mmpf = new motionmovparamform(itfpr,sbb->ID(),ui->inParamPanel);
    rui = mmpf;
    Vboxlayout->addWidget(mmpf);
}

void Form::UseCustomParamCreate(){
    UseCustomForm *ucf = new UseCustomForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = ucf;
    Vboxlayout->addWidget(ucf);
}

void Form::ImgSourceBlkParamCreate(){
    ImgSrcInForm *isinp = new ImgSrcInForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = isinp;
    Vboxlayout->addWidget(isinp);
}

void Form::ThridPartBlkParamCreate(){
    Thirdparty *td = new Thirdparty(itfpr,sbb->ID(),ui->inParamPanel);
    rui = td;
    Vboxlayout->addWidget(td);
}

void Form::ScanCodeParamCreate(){
    ScanCodeForm *scf = new ScanCodeForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = scf;
    Vboxlayout->addWidget(scf);
}

void Form::ImgOutBlkParamCreate()
{
    ImgOutParam *iop = new ImgOutParam(itfpr,sbb->ID(),ui->inParamPanel);
    rui = iop;
    //iop->setParent(ui->inParamPanel);
    Vboxlayout->addWidget(iop);
}

void Form::FormatConvertParamCreate(){
    ConvertFormatForm *cff = new ConvertFormatForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = cff;
    Vboxlayout->addWidget(cff);
}

void Form::PointCouldSourceParamCreate(){
    PCSrcInForm *pcsif = new PCSrcInForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = pcsif;
    Vboxlayout->addWidget(pcsif);
}

void Form::PointCouldOutParamCreate(){
    pcOutForm *pcof = new pcOutForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = pcof;
    Vboxlayout->addWidget(pcof);
}

void Form::PointCouldFilterParamCreate(){
    pcFilterForm *pcff = new pcFilterForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = pcff;
    Vboxlayout->addWidget(pcff);
}

void Form::roiMangerParamCreate(){
    roiGrpMangForm *rgmf = new roiGrpMangForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = rgmf;
    Vboxlayout->addWidget(rgmf);
}

void Form::PointCouldArclenParamCreate(){
    pcArclenForm *pcaf = new pcArclenForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = pcaf;
    Vboxlayout->addWidget(pcaf);
}

void Form::PointCouldPoaslParamCreate(){
    pcPoaslForm *pcpf = new pcPoaslForm(itfpr,sbb->ID(),ui->inParamPanel);
    rui = pcpf;
    Vboxlayout->addWidget(pcpf);
}

void Form::on_tester_clicked()
{
    //单块测试


    sbb->Run();
}

void Form::on_confim_clicked()
{
    //保存参数设定
    void *p = sbb->InParamsGet();
    if(p==0){return ;}
    rui->GetParamValue(p);
//    unsigned sbt = sbb->blkType();
//    switch(sbt){
//    case StepBlockBase::IMG_IN_SOURCE:
//        rui->GetParamValue(((ImgSourceStep *)sbb)->ImgSourceInputParamGet());
//        break;

//    case StepBlockBase::IMG_OUT:
//        rui->GetParamValue(((ImgOutStep *)sbb)->ImgOutInputParamGet());
//        break;

//    case StepBlockBase::THRD_PROCESS:
//        rui->GetParamValue(((ThridPartyCallBlk *)sbb)->ThridPartyInputParamGet());
//        break;

//    case StepBlockBase::IMG_SCANCODE:
//        rui->GetParamValue(((ScanCodeStep *)sbb)->ScanCodeInputParamGet());
//        break;

//    case StepBlockBase::COLORREG_UCM_PROCESS:
//        rui->GetParamValue(((ColorRecognitionStep *)sbb)->ColorRecognitionParamGet());
//        break;

//    case StepBlockBase::MOTION_MOV:
//        rui->GetParamValue(((MotionMovStep *)sbb)->MotionMovParamParamGet());
//        break;

//    case StepBlockBase::CONVERT_DEPIMG_PC:
//        rui->GetParamValue(((FormatConvertStep *)sbb)->FormatConvertParamGet());
//        break;

//    case StepBlockBase::IMG_UCUSTOM_PROCESS:
//        rui->GetParamValue(((UserCustomStep *)sbb)->UserCustomParamGet());
//        break;

//    case StepBlockBase::PC_IN_SOURCE:
//        rui->GetParamValue(((PointCouldSourceStep *)sbb)->PointCouldSourceParamGet());
//        break;

//    case StepBlockBase::PC_OUT:
//        rui->GetParamValue(((PointCouldOutStep *)sbb)->PointCouldOutParamGet());
//        break;

//    case StepBlockBase::PC_FILTER:
//        rui->GetParamValue(((PointCouldFilterStep *)sbb)->PointCouldFilterParamGet());
//        break;
//    default:
//        break;
//    }
}


void Form::on_nameSet_clicked()
{
    QString bn = ui->blkName->text();
    QByteArray qba = bn.toLocal8Bit();
    const char *newName = qba.data();
    sbb->Name(newName);
    FlowChart *fc = (FlowChart *)this->parent();
    fc->SenceUpdate();
}

