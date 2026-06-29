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
#include "pcdeformationform.h"
#include "pcemplenintervalform.h"
#include "pcgapform.h"
#include "pclenform.h"
#include "pcpreprocessform.h"
#include "pcboundolform.h"
#include "tcpserverrecvform.h"
#include "lightsourceform.h"
#include "focuslensform.h"
#include <iostream>


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


void Form::ParamPagePlanClr(){
    if(rui != 0){
        unsigned sbt = sbb->blkType();
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
        }else if(sbt ==StepBlockBase::IMG_MERG_PROCESS || sbt == StepBlockBase::IMG_LAMBPHOTOMETRIC){
            delete (LambertianPhotometricStereo*)rui;
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
        }else if(sbt == StepBlockBase::PC_DEFORMATION){
            delete (PcDeformationForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_POA_SL){
            delete (pcPoaslForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::ROIGRPMANGER){
            delete (roiGrpMangForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_EMPLEN){
            delete (pcEmplenIntervalForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_GAP){
            delete (pcGapForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::PC_LENGTH){
            delete (pcLenForm *)rui;
            rui=0;
        }
        else if(sbt == StepBlockBase::PC_BOUNDOL){
            delete (pcBoundolForm *)rui;
            rui=0;
        }
        else if(sbt == StepBlockBase::PC_PREPROCESS){
            delete (pcPreprocessForm *)rui;
            rui=0;
        }
        else if(sbt == StepBlockBase::TCP_SERVERRECV){
            delete (TcpServerRecvForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::HW_LIGHTSOURCE){
            delete (LightSourceForm *)rui;
            rui=0;
        }else if(sbt == StepBlockBase::HW_FOCUSINGLENS){
            delete (FocusLensForm *)rui;
            rui=0;
        }else{
            delete rui;
            rui=0;
        }
    }
}

Form::~Form()
{
    if(Vboxlayout != 0) {
        delete Vboxlayout;
        Vboxlayout = 0;
    }
    ParamPagePlanClr();
    delete ui;
}


void Form::ResultRefush(int sid){
    if(sid != sbb->ID()){return ;}
    if(sbb->blkType()==StepBlockBase::PC_ARC){
        ((pcArclenForm *)rui)->ShowRRLIst_Val();
    }
}

void Form::ShowHasParamPage(){
    unsigned sbt = sbb->blkType();
    switch(sbt){
        case StepBlockBase::IMG_IN_SOURCE:
        {
            ParamPanelCreate<ImgSrcInForm>(ui->inParamPanel);
            ImgSourceStep *ist = (ImgSourceStep *)sbb;
            ImgSourceParam *isp = ist->ParamGet<ImgSourceParam>();
            if(isp != 0){
                rui->SetParamValue(isp);
            }
        }
        break;

        case StepBlockBase::IMG_OUT:
        {
//            ParamPanelCreate<ImgOutParam>(ui->inParamPanel);
//            ImgOutStep *ios =(ImgOutStep *)sbb;
//            ImgOutParamer *iop = ios->ImgOutInputParamGet();
//            if(iop != 0){
//                rui->SetParamValue(iop);
//            }
            ParamCreateAndAppend<ImgOutParam,ImgOutStep,ImgOutParamer>(ui->inParamPanel);
        }
        break;

        case StepBlockBase::THRD_PROCESS:
        {
            ParamCreateAndAppend<Thirdparty,ThridPartyCallBlk,ThridPartyCallBlkParam>(ui->inParamPanel);
        }
        break;

        case StepBlockBase::IMG_MERG_PROCESS:
        {
        ParamPanelCreate<LambertianPhotometricStereo>(ui->inParamPanel);
        ImgMergingStep *ims = (ImgMergingStep *)sbb;
        ImgMergingParam *imp = ims->ParamGet<ImgMergingParam>();
        if(curBlkName==QString::fromLocal8Bit("HDR")){
            ((LambertianPhotometricStereo *)rui)->imTypeSet(1);
        }else if(curBlkName==QString::fromLocal8Bit("焦距融合")){
            ((LambertianPhotometricStereo *)rui)->imTypeSet(2);
        }
        if(imp != 0){
            rui->SetParamValue(imp);
        }
        }
        break;
        case StepBlockBase::IMG_LAMBPHOTOMETRIC:
        {
            ParamCreateAndAppend<LambertianPhotometricStereo,LambertianPhotoMetricstereoStep,LambertianPhotoMetriParam>(ui->inParamPanel);
        }
        break;

        case StepBlockBase::COLORREG_UCM_PROCESS:
        {
            ParamCreateAndAppend<ColorIdentification,ColorRecognitionStep,ColorRecogParam>(ui->inParamPanel);
        }
        break;

        case StepBlockBase::IMG_SCANCODE:
        {
//            ParamPanelCreate<ScanCodeForm>(ui->inParamPanel);
//            ScanCodeStep *scs = (ScanCodeStep *)sbb;
//            ScanCodeParam *scp = scs->ScanCodeInputParamGet();
//            if(scp !=0 ){
//                rui->SetParamValue(scp);
//            }
            ParamCreateAndAppend<ScanCodeForm,ScanCodeStep,ScanCodeParam>(ui->inParamPanel);
        }
        break;

        case StepBlockBase::MOTION_MOV:
        {
            ParamPanelCreate<motionmovparamform>(ui->inParamPanel);
            MotionMovStep *mms = (MotionMovStep *)sbb;
            MotionMovParam *mmp = mms->MotionMovParamParamGet();
            if(mmp != 0){
                rui->SetParamValue(mmp);
            }
        }
        break;
        case StepBlockBase::CONVERT_DEPIMG_PC:
        {
            FormatConvertStep *fcs = (FormatConvertStep *)sbb;
            FormatConvertParam *fcp = fcs->FormatConvertParamGet();
            if(fcp != 0){
                rui->SetParamValue(fcp);
            }
        }
        break;
        case StepBlockBase::IMG_UCUSTOM_PROCESS:
        {
            ParamPanelCreate<UseCustomForm>(ui->inParamPanel);
            UserCustomStep *ucs = (UserCustomStep *)sbb;
            UseCustomParam *ucp = ucs->ParamGet<UseCustomParam>();
            if(ucp != 0){
                rui->SetParamValue(ucp);
            }
        }
        break;

        case StepBlockBase::PC_IN_SOURCE:
        {
            ParamPanelCreate<PCSrcInForm>(ui->inParamPanel);
            PointCouldSourceStep *pcss = (PointCouldSourceStep *)sbb;
            PointCouldSourceParam *pcsp = pcss->PointCouldSourceParamGet();
            if(pcsp != 0){
                rui->SetParamValue(pcsp);
            }
        }
        break;

        case StepBlockBase::PC_OUT:
        {
            ParamPanelCreate<pcOutForm>(ui->inParamPanel);
            PointCouldOutStep *pcos = (PointCouldOutStep *)sbb;
            PointCouldOutParam *pcop = pcos->PointCouldOutParamGet();
            if(pcop != 0){
                rui->SetParamValue(pcop);
            }
        }
        break;

        case StepBlockBase::PC_FILTER:
        {
            ParamPanelCreate<pcFilterForm>(ui->inParamPanel);
            PointCouldFilterStep *pcfs = (PointCouldFilterStep *)sbb;
            PointCouldFilterParam *ucp = pcfs->PointCouldFilterParamGet();
            if(ucp != 0){
                rui->SetParamValue(ucp);
            }
        }
        break;
        case StepBlockBase::ROIGRPMANGER:
            ParamCreateAndAppend<roiGrpMangForm,roiGrpMangerStep,roiMangeParam>(ui->inParamPanel);
        break;
        case StepBlockBase::PC_ARC:
            ParamCreateAndAppend<pcArclenForm,PointCouldArclenStep,PointCouldArclenParam>(ui->inParamPanel);
        break;
        case StepBlockBase::PC_POA_SL:
            ParamCreateAndAppend<pcPoaslForm,PointCouldPoaslStep,PointCouldPoaslParam>(ui->inParamPanel);
        break;
        case StepBlockBase::PC_DEFORMATION:
            ParamCreateAndAppend<PcDeformationForm,PointCouldDeformationStep,PointCouldDeformationParam>(ui->inParamPanel);
        break;
        case StepBlockBase::PC_EMPLEN:
            ParamCreateAndAppend<pcEmplenIntervalForm,PointCouldEmplenStep,PointCouldEmplenParam>(ui->inParamPanel);
            break;
        case StepBlockBase::PC_GAP:
            ParamCreateAndAppend<pcGapForm,PointCouldGapStep,PointCouldGapParam>(ui->inParamPanel);
            break;
        case StepBlockBase::PC_LENGTH:
            ParamCreateAndAppend<pcLenForm,PointCouldLengthStep,PointCouldLenParam>(ui->inParamPanel);
            break;
        case StepBlockBase::PC_PREPROCESS:
            ParamCreateAndAppend<pcPreprocessForm,PointCouldPreProcessStep,PointCouldPreProParam>(ui->inParamPanel);
            break;
        case StepBlockBase::PC_BOUNDOL:
            ParamCreateAndAppend<pcBoundolForm,PointCouldBoundolStep,PointCouldBoundolParam>(ui->inParamPanel);
            break;
        case StepBlockBase::TCP_SERVERRECV:
            ParamCreateAndAppend<TcpServerRecvForm,TcpServerRecvStep>(ui->inParamPanel);
            break;
        case StepBlockBase::HW_FOCUSINGLENS:
            ParamCreateAndAppend<FocusLensForm,FocusingLensStep>(ui->inParamPanel);
            break;
        case StepBlockBase::HW_LIGHTSOURCE:
            ParamCreateAndAppend<LightSourceForm,LightSourceStep,msclspParam>(ui->inParamPanel);
            break;

        default:
            break;
        }
}

void Form::on_tester_clicked()
{
    //单块测试
    sbb->Run();
    if(rui!=0){
        rui->GetTestResult(0);
    }
}

void Form::on_confim_clicked()
{
    //保存参数设定
    void *p = sbb->InParamsGet();
    rui->GetParamValue(p);
}

void Form::on_nameSet_clicked()
{
    QString bn = ui->blkName->text();
    QByteArray qba = bn.toLocal8Bit();
    const char *newName = qba.data();
    QString oname = QString::fromLocal8Bit(sbb->Name());
    sbb->Name(newName);
    mainFcer->SenceDitemUpdate(oname,bn);
}

