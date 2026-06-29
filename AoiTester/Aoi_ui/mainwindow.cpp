#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QFileDialog"
#include <QSettings>
#include <QMessageBox>
#include <QList>
#include <Qimage>
#include <QBuffer>

//CCLib Includes
#include <CloudSamplingTools.h>
#include <ParallelSort.h>

#include <ccPickingHub.h>
#include "ccPointPropertiesDlg.h"

#include <myCCLabel.h>
//qCC_db
#include <cc2DLabel.h>
#include <cc2DViewportObject.h>
#include <ccCameraSensor.h>
#include <ccColorScalesManager.h>
#include <ccFacet.h>
#include <ccFileUtils.h>
#include <ccGBLSensor.h>
#include <ccImage.h>
#include <ccKdTree.h>
#include <ccPlane.h>
#include <ccProgressDialog.h>
#include <ccQuadric.h>
#include <ccSphere.h>
#include <ccCylinder.h>
#include <ccSubMesh.h>
#include <ccGenericPointCloud.h>
#include <ccPointCloud.h>

//qCC_io
#include <ccShiftAndScaleCloudDlg.h>
#include <BinFilter.h>
#include <DepthMapFileFilter.h>

//QCC_glWindow
#include <ccGLWidget.h>
#include <ccRenderingTools.h>

//local includes
#include "ccConsole.h"
#include "pluginManager/ccPluginUIManager.h"
#include "ccCommon.h"
#include "ccReservedIDs.h"

//other
#include "ccClippingBoxTool.h"
//#include "ccCropTool.h"
#include "ccPersistentSettings.h"
#include "ccOptions.h"

#include "ccComparisonDlg.h"
#include "ccOrderChoiceDlg.h"

#include "flowchart.h"
#include <iostream>

//-----3D点拾取相关的变量----------------------------
enum PickingOperation {	NO_PICKING_OPERATION,
                        PICKING_ROTATION_CENTER,
                        PICKING_LEVEL_POINTS,
                        };
static ccGLWindow* s_pickingWindow = nullptr;
static PickingOperation s_currentPickingOperation = NO_PICKING_OPERATION;
static std::vector<cc2DLabel*> s_levelLabels;
static ccPointCloud* s_levelMarkersCloud = nullptr;
//static ccHObject* s_levelEntity = nullptr;
//----------3D点拾取相关的变量------------

static FlowChart* _flowchart_windows = nullptr;


static QFileDialog::Options CCFileDialogOptions()
{
    QFileDialog::Options dialogOptions = QFileDialog::Options();
    if (!ccOptions::Instance().useNativeDialogs)
    {
        dialogOptions |= QFileDialog::DontUseNativeDialog;
    }
    return dialogOptions;
}

int MainWindow::BackMessageProcess(void* pc,int code,void *self){
    MainWindow *Self = (MainWindow *)self;
    switch(code){
    case CBC_UPDWRKD:
        Self->FillWrkPointGrps();
        Self->updateUI();
        break;

    case CBC_WRKSTOP:
        Self->mroTotalCnt=0;
        Self->sysstatus->UpdateCycleTime(1);
    case CBC_VISUAL_UPD:
        Self->rerp->rrRepListSelectedSet();
        break;

    case CBC_RELOADPAGE:
        Self->rerp->rrRepListSelectedSet();
        break;
    case CBC_PCUSHOW:
        if(pc != 0){
            CloudEntity *ce = (CloudEntity *)pc;
            QString name = QString::fromLocal8Bit("test_") +QString::number(Self->mroTotalCnt);
            Self->myPointCouldDataShow(name,ce->Entiy,ce->pNums);
        }
        break;

    case 12://单一测量完毕
        if(Self->rerp !=0){
            Self->rerp->UpdNextResult();
        }
        Self->mroTotalCnt+=1;
        break;
    case 13://重复测试的每一次开始测试
        Self->ToggleWorkProAction();
        break;

    case CBC_IMGSHOW://结果图片显示
    {
        rxsImg *in = (rxsImg *)pc;
        Self->lsrScaner->mainReadyOpter();
        Self->photoLabel->setPhoto(QString::fromLocal8Bit("result"),in);
    }
        break;

    case CBC_STR_N_MSG:
        Self->ui->RecordList->addItem(QString::fromLocal8Bit((const char *)pc));
        break;
    case CBC_STR_W_MSG:
        ccLog::Warning(QString::fromLocal8Bit((const char *)pc));
        break;
    case CBC_STR_E_MSG:
        ccLog::Error(QString::fromLocal8Bit((const char *)pc));
        break;
    }
    return 0;
}

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->wndFixm = 0;
    this->userLogin = 0;
    this->myHelp = 0;
    this->facParamSet = 0;
    this->usem = 0;
    this->SCopter = 0;
    this->rerp = 0;
    mainFrame = 0;
    m_compDlg = 0;
    mroTotalCnt = 0;
    m_pickingHub = 0;
    m_ppDlg = 0;
    roiCpset = 0;
    m_gsTool=0;
    ComTester = 0;
    isCtrKeyPress = false;
    ismyKeyPress = false;
    this->modoOpter = 0;
    sysRunTime = new QLabel;
    curUser = new QLabel;
    msgText = new QLabel;
    mpActionMap = new QSignalMapper;
    QSize sss = statusBar()->size();
    sss.setWidth(200);
    msgText->setMinimumSize(sss);
    msgText->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(msgText);//左侧
    statusBar()->addPermanentWidget(curUser);//添加现实永久信息。//最右侧
    statusBar()->setSizeGripEnabled(false); //设置是否显示右边的大小控制点
    statusBar()->addPermanentWidget(sysRunTime);//添加现实永久信息。//最右侧
    statusBar()->setSizeGripEnabled(false);

    QStringList mpGrpheader;
    mpGrpheader<<"ID"<<QString::fromLocal8Bit("速度")<<QString::fromLocal8Bit("模式");
    mpGrpheader<<"SX"<<"SY"<<"SZ"<<"EX"<<"EY"<<"EZ"<<QString::fromLocal8Bit("加速度");
    mpGrpheader<<QString::fromLocal8Bit("减速度")<<QString::fromLocal8Bit("流程选定");
    ui->MpsDT->setColumnCount(12);
    ui->MpsDT->setHorizontalHeaderLabels(mpGrpheader);
    ui->MpsDT->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->MpsDT->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->MpsDT->setColumnWidth(0, 30);
    ui->MpsDT->setColumnWidth(1, 50);
    ui->MpsDT->setColumnWidth(2, 50);
    ui->MpsDT->setColumnWidth(3, 60);
    ui->MpsDT->setColumnWidth(4, 60);
    ui->MpsDT->setColumnWidth(5, 60);
    ui->MpsDT->setColumnWidth(6, 60);
    ui->MpsDT->setColumnWidth(7, 60);
    ui->MpsDT->setColumnWidth(8, 60);
    ui->MpsDT->setColumnWidth(9, 50);
    ui->MpsDT->setColumnWidth(10, 50);
    ui->MpsDT->setColumnWidth(11, 340);

    mpActionMap->setMapping(ui->AddRowMp,QString("Add"));
    mpActionMap->setMapping(ui->DelRowMp,QString("Del"));
    mpActionMap->setMapping(ui->moveToMp,QString("Mov"));
    mpActionMap->setMapping(ui->moveToMpEnd,QString("MovEnd"));
    mpActionMap->setMapping(ui->pickMp,QString("Pick"));
    mpActionMap->setMapping(ui->pickEndMp,QString("PickEnd"));
    connect(ui->AddRowMp,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(ui->DelRowMp,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(ui->moveToMp,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(ui->moveToMpEnd,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(ui->pickMp,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(ui->pickEndMp,SIGNAL(clicked()),mpActionMap,SLOT(map()));
    connect(mpActionMap,SIGNAL(mapped(QString)),this,SLOT(MpOpter(QString)));

    ccConsole::Init(ui->RecordList, this, this);
    //ui->actionEnableQtWarnings->setChecked(ccConsole::QtMessagesEnabled());
    ccConsole::QtMessagesEnabled();
    connect(ui->actionFront,&QAction::triggered, this, [=]() { setView( CC_ISO_VIEW_1 ); });
    connect(ui->actionBack,&QAction::triggered, this, [=]() { setView( CC_ISO_VIEW_2 ); });
    connect(ui->actionXVLpos,&QAction::triggered, this, [=]() { setView( CC_LEFT_VIEW ); });
    connect(ui->actionXVFpos,&QAction::triggered, this, [=]() { setView( CC_FRONT_VIEW ); });
    connect(ui->actionXVDpos,&QAction::triggered, this, [=]() { setView( CC_TOP_VIEW ); });

    ui->PointCouldOpter->setWindowTitle(QString::fromLocal8Bit("点云操作栏"));

    connect(ui->actionRoiSelected,&QAction::triggered, this, [=](){activateClippingBoxMode();});

    ui->thrdPluger->setEnabled(true);
    ui->thrdPluger->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->thrdPluger->setVisible(true);

    curUser->setText(tr("<font style = 'font-size:16px; color:red;'> %1 </font>").arg(QString::fromLocal8Bit("未登入")));
    ui->RecordList->addItem(QString::fromLocal8Bit("UI初始化完成！"));
    if (!(mainFrame = rxsSystemReady())) {
        ui->RecordList->addItem(QString::fromLocal8Bit("运控系统加载失败"));
    }else{
        ui->RecordList->addItem(QString::fromLocal8Bit("运控系统加载完成"));
    }

    //mainFrame->userManger->CurUser.Auth = UserManger::USER_AUTH::ADMIN;
    mainFrame->FontMessageProcessCB(BackMessageProcess,this);
    sysstatus = new sysStatus(mainFrame);
    sysstatus->setParent(ui->sysStatusPanel);
    ui->sysStatusPanel->layout()->addWidget(sysstatus);

    rrpBox = new QGridLayout(ui->resultReport);
    rrpBox->setMargin(0);
    rrpBox->setSpacing(1);
    rerp = new ResultReport(this);
    rrpBox->addWidget(rerp);
    ui->resultReport->setLayout(rrpBox);

    m_pluginUIManager = new ccPluginUIManager( this, this );
    m_pluginUIManager->init(ui->thrdPluger);

    m_ccRoot = new ccDBRoot(nullptr,nullptr, this);
    new3DView(false);

    m_pickingHub = new ccPickingHub(this, this);
    m_clipTool = new ccClippingBoxTool(this);
    //目前可以暂时不用，主要是做了一些本界面上相关控件关联内容的更新操作
    //connect(m_clipTool, &ccOverlayDialog::processFinished, this, &MainWindow::deactivateClippingBoxMode);
    updateUI();

    tabifyDockWidget(ui->picShowDock,ui->dockWidget_CCViewer);
    photoLabel = new PhotoLabel(ui->picArea);
    QGridLayout *qgrdl = new QGridLayout();
    qgrdl->addWidget(photoLabel);
    ui->picArea->setLayout(qgrdl);

    QGridLayout *qgrdlsu = new QGridLayout();
    this->lsrScaner = new LaseScaner(this);
    this->lsrScaner->setParent(ui->imgSensorOpter);
    qgrdlsu->addWidget(lsrScaner);
    ui->imgSensorOpter->setLayout(qgrdlsu);

    QVBoxLayout *qvldmou = new QVBoxLayout();
    unsigned anums=0;
    AxisInfo *ais = mainFrame->AxInfo(anums);
    for(unsigned x=0;x< anums;x++){
        AxisInfoCtr *aic = new AxisInfoCtr(&ais[x],ui->motionCtrOpter);
        AicGrps.push_back(aic);
        qvldmou->addWidget(aic);
    }
    ui->motionCtrOpter->setLayout(qvldmou);

    disableButtonFunction(mainFrame->userManger->CurUser.Auth);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(MainUi_ref()));
    timer->start(100);
}

MainWindow::~MainWindow()
{
    on_actionClrView_triggered();
    if(modoOpter!=0){delete modoOpter;modoOpter=0; }
    if(wndFixm!=0){delete wndFixm;wndFixm=0; }
    if(facParamSet != 0){delete facParamSet;facParamSet = 0;}
    if(myHelp != 0){delete myHelp; myHelp = 0;}
    if(ComTester != 0){delete ComTester; ComTester = 0;}
    if(userLogin !=0){delete userLogin; userLogin = 0;}
    if(usem != 0){delete usem;usem=0;}
    if(SCopter != 0){delete SCopter;SCopter =0;}
    if (view3D){view3D->setParent(nullptr);delete view3D;view3D=0;}
    delete lsrScaner;lsrScaner=0;
    delete m_pluginUIManager; m_pluginUIManager=0;
    delete m_clipTool; m_clipTool = 0;
    delete m_ccRoot;m_ccRoot = 0;
    delete mpActionMap;
    delete msgText;
    delete curUser;
    delete sysRunTime;
    if(rerp !=0){delete rerp;rerp=0;}
    if(ccvBox != 0){delete ccvBox;ccvBox=0;}
    if(rrpBox != 0){delete rrpBox;rrpBox=0;}
    if(sysstatus !=0){delete sysstatus;sysstatus=0;}
    if(m_ppDlg != 0){delete m_ppDlg;m_ppDlg = 0;}
    if(m_pickingHub != 0){delete m_pickingHub;m_pickingHub = 0;}
    delete ui;
    rxsSystemRelease();
}

ccGLWindow* MainWindow::new3DView(bool allowEntitySelection)
{
    bool stereoMode = QSurfaceFormat::defaultFormat().stereo();
    CreateGLWindow(view3D, viewWidget, stereoMode);
    if (!viewWidget || !view3D)
    {
        ccLog::Error(QString::fromLocal8Bit("3D view 创建失败!"));
        assert(false);
        return nullptr;
    }
    //restore options
    {
        QSettings settings;
        bool autoPickRotationCenter = settings.value(ccPS::AutoPickRotationCenter(), true).toBool();
        view3D->setAutoPickPivotAtCenter(autoPickRotationCenter);
    }
    viewWidget->setParent(ui->ccDataView);
    //ccGui::Parameters().backgroundCol = ccColor::Rgbub(0,0,0) ;
    ccvBox = new QGridLayout(ui->ccDataView);
    ccvBox->setMargin(0);
    ccvBox->setSpacing(0);
    ccvBox->addWidget(viewWidget);
    ui->ccDataView->setLayout(ccvBox);
    viewWidget->setMinimumSize(400, 300);
    connect(view3D,	&ccGLWindow::mouseWheelRotated, this, &MainWindow::echoMouseWheelRotate);
    connect(view3D,	&ccGLWindow::cameraDisplaced, this, &MainWindow::echoCameraDisplaced);
    connect(view3D,	&ccGLWindow::viewMatRotated, this, &MainWindow::echoBaseViewMatRotation);
    connect(view3D,	&ccGLWindow::cameraPosChanged, this, &MainWindow::echoCameraPosChanged);
    connect(view3D,	&ccGLWindow::pivotPointChanged, this, &MainWindow::echoPivotPointChanged);
    connect(view3D,	&ccGLWindow::pixelSizeChanged, this, &MainWindow::echoPixelSizeChanged);
    connect(view3D,	&QObject::destroyed, this, &MainWindow::prepareWindowDeletion);
    connect(view3D,	&ccGLWindow::filesDropped, this, &MainWindow::addToDBAuto, Qt::QueuedConnection); //DGM: we don't want to block the 'dropEvent' method of ccGLWindow instances!
    connect(view3D,	&ccGLWindow::newLabel, this, &MainWindow::handleNewLabel);
    connect(view3D,	&ccGLWindow::exclusiveFullScreenToggled, this, &MainWindow::onExclusiveFullScreenToggled);

    if (m_pickingHub)
    {
        connect(view3D, &QObject::destroyed, m_pickingHub, &ccPickingHub::onActiveWindowDeleted);
    }

    view3D->setSceneDB(m_ccRoot->getRootEntity());
    viewWidget->setAttribute(Qt::WA_DeleteOnClose);
    viewWidget->showMaximized();
    viewWidget->update();
    return view3D;
}

void MainWindow::MainUi_ref(){
    QDateTime dateTime= QDateTime::currentDateTime();
    QString str = dateTime .toString("yyyy-MM-dd hh:mm:ss");
    sysRunTime->setText(str);
    sysstatus->uiCtr_ref();
    if(wndFixm!=0){
        wndFixm->uiref_();
    }

    QLayout * mcol = ui->motionCtrOpter->layout();
    if(!mcol->isEmpty()){
        for(int x=0;x<AicGrps.size();x++){
            AicGrps[x]->ui_refush_dataupdata();
        }
    }
}

ccDBRoot* MainWindow::db(){
    return m_ccRoot;
}

void MainWindow::on_action_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();
        if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN)
            return;
    }
    if(facParamSet == 0){
        facParamSet = new FacParamSet(mainFrame,this);
        facParamSet->setWindowFlags(Qt::WindowCloseButtonHint|Qt::Dialog);
    }else{
        facParamSet->ref_ui_data();
    }
    facParamSet->show();
}

void MainWindow::on_actionUserLogin_triggered()
{
    if(userLogin ==0){
        userLogin = new Login(mainFrame,this);
    }
    userLogin->setWindowFlags(Qt::WindowCloseButtonHint|Qt::Dialog);
    userLogin->exec();
    if(mainFrame->userManger->CurUser.Uname==0 && mainFrame->userManger->CurUser.Auth ==UserManger::USER_AUTH::ADMIN ){
        curUser->setText(QString::fromLocal8Bit("SYSTEM"));
    }else{
        curUser->setText(QString::fromLocal8Bit(mainFrame->userManger->CurUser.Uname));
    }
    disableButtonFunction(mainFrame->userManger->CurUser.Auth);
}

void MainWindow::on_actionEntMange_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::TECH){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();

        if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::TECH)
            return;
    }
    if(roiCpset==0){
        roiCpset = new roiComputerSet(this);
        roiCpset->setWindowFlags(Qt::WindowCloseButtonHint |Qt::Dialog);
    }else{
        roiCpset->ref_ui_data();
    }
    roiCpset->show();
}

void MainWindow::on_actionHelp_triggered()
{
    if(myHelp == 0){
        myHelp = new help(this);
    }
    myHelp->show();
}

void MainWindow::ToggleWorkProAction(){
    on_actionClrView_triggered();
    //切换默认视图视角
    Kvpdchr val;
    int cvv=0;
    mainFrame->WrkProparam->Selected(1);
    mainFrame->WrkProparam->DataReaderKvpd((unsigned)0,val);
    if(val.val != 0){
        cvv = atoi(val.val);
    }
    setView((CC_VIEW_ORIENTATION)cvv);
}

void MainWindow::on_actionWrkStart_triggered()
{
    QString cmdline=sysstatus->GetRepCnt();
    QByteArray qba = cmdline.toLocal8Bit();
    const char *cl =qba.data();
    rerp->ResultDataClr();
    ToggleWorkProAction();
    sysstatus->UpdateCycleTime();
    WorkProcessDal * wrkpd = WorkProcessDalGet();
    QString pno = sysstatus->GetProductInfo();
    const char *d = pno.toLocal8Bit().data();
    wrkpd->product.Name = d;
    int cvv = mainFrame->WorkStart(cl);
    if(cvv!=0){
        ui->RecordList->addItem(QString::fromLocal8Bit("加工开始失败!"));
    }
    ui->RecordList->clear();
}

void MainWindow::on_actionWrkStop_triggered()
{
    mainFrame->WorkStop(false);
    sysstatus->UpdateCycleTime(1);
}


void MainWindow::on_actionWrkPause_triggered()
{
    mainFrame->WorkPause();
}


void MainWindow::on_actionSysReset_triggered()
{
    mroTotalCnt = 0;
    ui->RecordList->clear();
    mainFrame->SysReset();
}

void MainWindow::on_actionUserManger_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();
        if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN)
            return;
    }
    if(usem != 0){
        delete usem;
        usem = 0;
    }
    usem = new usrManger(mainFrame);
    usem->setWindowFlags(Qt::WindowCloseButtonHint |Qt::Dialog);
    usem->show();
}

void MainWindow::on_actionLoadWrkfile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("加工程式选择"),"",tr("Text files(*.txt);;cfg files (*.ipro);;all files (*.*)"));
    if(fileName.isNull()){
        return;
    }
    QByteArray qba = fileName.toLatin1();
    const char *fn1 =qba.constData();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(wrkpd->WorkProReloader(fn1)){
        msgText->setText(QString(mainFrame->WrkProparam->CurFilePath()));
        ui->RecordList->addItem(QString::fromLocal8Bit("加工程式加载完成!"));
    }else{
        ui->RecordList->addItem(QString::fromLocal8Bit("加工程式加载失败!"));
        FillWrkPointGrps();
        updateUI();
    }
}


void MainWindow::on_actionSaveWrkfile_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("另存加工程式选择"),"",tr("Text files(*.txt);;cfg files (*.ipro);;all files (*.*)"));
    if(fileName.isNull()){
        return;
    }
    GetWrkPoingGrps();
    QByteArray qba = fileName.toLatin1();
    const char *fn1 =qba.constData();
    mainFrame->WrkProparam->Save(fn1);
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->WorkProReloader(fn1);
}

void MainWindow::on_actionAssociationCfg_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();
        return;
    }
    if(SCopter == 0){
        SCopter = new scOpter(mainFrame,this);
    }
    SCopter->setWindowFlags(Qt::WindowCloseButtonHint |Qt::Dialog);
    SCopter->show();
}

void MainWindow::on_actionCalb_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::ADMIN){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();
        return;
    }
    if(wndFixm == 0){
        wndFixm = new FixMaintenance(mainFrame,this);
    }
    wndFixm->setWindowFlags(Qt::WindowCloseButtonHint |Qt::Dialog);
    wndFixm->show();
}

void MainWindow::MpOpter(QString oid){
    int colNum = ui->MpsDT->columnCount();
    if(oid=="Add"){
        int rowNum = ui->MpsDT->rowCount();
        ui->MpsDT->insertRow(rowNum);
        QTableWidgetItem *itemStatus = new QTableWidgetItem(QString::number(rowNum));
        itemStatus->setFlags(itemStatus->flags() & (~Qt::ItemIsEditable));
        ui->MpsDT->setItem(rowNum, colNum-1, itemStatus);

        QTableWidgetItem *ID = new QTableWidgetItem(QString::number(rowNum+1));
        QTableWidgetItem *Speed = new QTableWidgetItem("20.0");
        QTableWidgetItem *Mt = new QTableWidgetItem("1");
        ui->MpsDT->setItem(rowNum, 0, ID);
        ui->MpsDT->setItem(rowNum, 1, Speed);
        ui->MpsDT->setItem(rowNum, 2, Mt);
        ui->MpsDT->setItem(rowNum, 3, Mt);
        ui->MpsDT->setItem(rowNum, 4, Mt);
        ui->MpsDT->setItem(rowNum, 5, Mt);
        ui->MpsDT->setItem(rowNum, 6, Mt);
        ui->MpsDT->setItem(rowNum, 7, Mt);
        ui->MpsDT->setItem(rowNum, 8, Mt);
        ui->MpsDT->setItem(rowNum, 9, Mt);
        ui->MpsDT->setItem(rowNum, 10, Mt);
        ui->MpsDT->setItem(rowNum, 11, Mt);
        return;
    }
    if(oid=="Del"){
        int CurSelectedRowIndex = ui->MpsDT->currentRow();
        if(CurSelectedRowIndex < 0){
            ccConsole::Warning(QString::fromLocal8Bit("请先选中要删除的行!"));
            return;
        }
        ui->MpsDT->removeRow(CurSelectedRowIndex);
        return;
    }
    if(oid=="Mov"){
        int CurSelectedRowIndex = ui->MpsDT->currentRow();
        if(CurSelectedRowIndex < 0){
            ccConsole::Warning(QString::fromLocal8Bit("请先选中要移动的点位!"));
            return;
        }
        QString x = ui->MpsDT->item(CurSelectedRowIndex,3)->text();
        QString y = ui->MpsDT->item(CurSelectedRowIndex,4)->text();
        QString z = ui->MpsDT->item(CurSelectedRowIndex,5)->text();
        unsigned AxNum=0;
        AxisInfo *axGrps =  mainFrame->AxInfo(AxNum);
        axGrps[0].TagePos =x.toFloat();
        axGrps[1].TagePos =y.toFloat();
        axGrps[2].TagePos =z.toFloat();
        mainFrame->AxMove(axGrps[0].AxName,'A');
        mainFrame->AxMove(axGrps[1].AxName,'A');
        mainFrame->AxMove(axGrps[2].AxName,'A');
        return;
    }
    if(oid=="MovEnd"){
        int CurSelectedRowIndex = ui->MpsDT->currentRow();
        if(CurSelectedRowIndex < 0){
            ccConsole::Warning(QString::fromLocal8Bit("请先选中要移动的点位!"));
            return;
        }
        QString x = ui->MpsDT->item(CurSelectedRowIndex,6)->text();
        QString y = ui->MpsDT->item(CurSelectedRowIndex,7)->text();
        QString z = ui->MpsDT->item(CurSelectedRowIndex,8)->text();
        unsigned AxNum=0;
        AxisInfo *axGrps =  mainFrame->AxInfo(AxNum);
        axGrps[0].TagePos =x.toFloat();
        axGrps[1].TagePos =y.toFloat();
        axGrps[2].TagePos =z.toFloat();
        mainFrame->AxMove(axGrps[0].AxName,'A');
        mainFrame->AxMove(axGrps[1].AxName,'A');
        mainFrame->AxMove(axGrps[2].AxName,'A');
        return;
    }
    if(oid=="Pick"){
        int CurSelectedRowIndex = ui->MpsDT->currentRow();
        if(CurSelectedRowIndex < 0){
            ccConsole::Warning(QString::fromLocal8Bit("请先选中要拾取到的点位!"));
            return;
        }
        unsigned AxNum=0;
        AxisInfo *axGrps =  mainFrame->AxInfo(AxNum);
        QTableWidgetItem *x = new QTableWidgetItem(QString::number(axGrps[0].Status.EncPos));
        QTableWidgetItem *y = new QTableWidgetItem(QString::number(axGrps[1].Status.EncPos));
        QTableWidgetItem *z = new QTableWidgetItem(QString::number(axGrps[2].Status.EncPos));
        ui->MpsDT->setItem(CurSelectedRowIndex, 3, x);
        ui->MpsDT->setItem(CurSelectedRowIndex, 4, y);
        ui->MpsDT->setItem(CurSelectedRowIndex, 5, z);
        return;
    }
    if(oid=="PickEnd"){
        int CurSelectedRowIndex = ui->MpsDT->currentRow();
        if(CurSelectedRowIndex < 0){
            ccConsole::Warning(QString::fromLocal8Bit("请先选中要拾取到的点位!"));
            return;
        }
        unsigned AxNum=0;
        AxisInfo *axGrps =  mainFrame->AxInfo(AxNum);
        QTableWidgetItem *x = new QTableWidgetItem(QString::number(axGrps[0].Status.EncPos));
        QTableWidgetItem *y = new QTableWidgetItem(QString::number(axGrps[1].Status.EncPos));
        QTableWidgetItem *z = new QTableWidgetItem(QString::number(axGrps[2].Status.EncPos));
        ui->MpsDT->setItem(CurSelectedRowIndex, 6, x);
        ui->MpsDT->setItem(CurSelectedRowIndex, 7, y);
        ui->MpsDT->setItem(CurSelectedRowIndex, 8, z);
        return;
    }
}

void MainWindow::GetWrkPoingGrps(){
    int mLen = ui->MpsDT->rowCount();
    Movepath movePath;
    Scanpoint scanPoint;
    memset(&scanPoint,0,sizeof(Scanpoint));
    mainFrame->WrkProparam->Selected(3,true);
    QString val ;
    for(unsigned x =0;x<mLen;x++){
        memset(&movePath,0,sizeof(Movepath));
        val = ui->MpsDT->item(x,0)->text();
        movePath.id=val.toUShort();
        val = ui->MpsDT->item(x,1)->text();
        movePath.speed=val.toFloat();
        val = ui->MpsDT->item(x,2)->text();
        movePath.Type=val.toUShort();
        val = ui->MpsDT->item(x,3)->text();
        movePath.Sx=val.toFloat();
        val = ui->MpsDT->item(x,4)->text();
        movePath.Sy=val.toFloat();
        val = ui->MpsDT->item(x,5)->text();
        movePath.Sz=val.toFloat();
        val = ui->MpsDT->item(x,6)->text();
        movePath.Ex=val.toFloat();;
        val = ui->MpsDT->item(x,7)->text();
        movePath.Ey=val.toFloat();
        val = ui->MpsDT->item(x,8)->text();
        movePath.Ez=val.toFloat();
        val = ui->MpsDT->item(x,9)->text();
        movePath.acc=val.toFloat();
        val = ui->MpsDT->item(x,10)->text();
        movePath.dec=val.toFloat();
        val = ui->MpsDT->item(x,11)->text();
        if(val.length()>50){
            //val.
        }
        QByteArray qba = val.toLocal8Bit();
        memcpy( movePath.Flower,qba.data(),qba.length());
        mainFrame->WrkProparam->DataEditer(x,&movePath);
    }
    mainFrame->WrkProparam->Selected(4,true);

    QByteArray qba = ui->tskSensorSelected->currentText().toLocal8Bit();
    Kvpdchr Val={0,qba.data(),0};
    mainFrame->WrkProparam->Selected(2);
    mainFrame->WrkProparam->DataEditer(2,&Val);
}

void MainWindow::FillWrkPointGrps(){
    int ri = 0;
    Movepath* mp = 0;
    while(ui->MpsDT->rowCount()){
        ui->MpsDT->removeRow(0);
    }
    mainFrame->WrkProparam->Selected(3);
    while ((mp = (Movepath*)mainFrame->WrkProparam->DataReaderMsp(ri)) != 0)
    {
        QTableWidgetItem *sx = new QTableWidgetItem(QString::number((*mp).Sx));
        QTableWidgetItem *sy = new QTableWidgetItem(QString::number((*mp).Sy));
        QTableWidgetItem *sz = new QTableWidgetItem(QString::number((*mp).Sz));
        QTableWidgetItem *speed = new QTableWidgetItem(QString::number((*mp).speed));
        QTableWidgetItem *type = new QTableWidgetItem(QString::number((*mp).Type));
        QTableWidgetItem *ex = new QTableWidgetItem(QString::number((*mp).Ex));
        QTableWidgetItem *ey = new QTableWidgetItem(QString::number((*mp).Ey));
        QTableWidgetItem *ez = new QTableWidgetItem(QString::number((*mp).Ez));
        QTableWidgetItem *ID = new QTableWidgetItem(QString::number((*mp).id));
        QTableWidgetItem *acc = new QTableWidgetItem(QString::number((*mp).acc));
        QTableWidgetItem *dec = new QTableWidgetItem(QString::number((*mp).dec));
        QTableWidgetItem *Flower = new QTableWidgetItem(QString::fromLocal8Bit((*mp).Flower));
        ui->MpsDT->insertRow(ri);
        ui->MpsDT->setItem(ri, 0, ID);
        ui->MpsDT->setItem(ri, 1, speed);
        ui->MpsDT->setItem(ri, 2, type);
        ui->MpsDT->setItem(ri, 3, sx);
        ui->MpsDT->setItem(ri, 4, sy);
        ui->MpsDT->setItem(ri, 5, sz);
        ui->MpsDT->setItem(ri, 6, ex);
        ui->MpsDT->setItem(ri, 7, ey);
        ui->MpsDT->setItem(ri, 8, ez);
        ui->MpsDT->setItem(ri, 9, acc);
        ui->MpsDT->setItem(ri, 10, dec);
        ui->MpsDT->setItem(ri, 11, Flower);
        ri += 1;
    }
}

void MainWindow::on_actionClrView_triggered()
{
    m_ccRoot->clrAllEntities();
    GLWindowFromWidget(viewWidget)->refresh(false);
    //WorkProcessDal *wrkpd = WorkProcessDalGet();
    ui->RecordList->addItem(QString::fromLocal8Bit("视图清空完成!"));
}

bool MainWindow::SelectedEntitiesSet(int wich){
    if(m_ccRoot->changeSelection_my(wich)!=0){
        ccConsole::Warning(QString::fromLocal8Bit("点云实体无法选中,请先确认已经加载!"));
        return false;
    }
    dbTreeSelectionInfo selInfo;
    m_selectedEntities.clear();
    if (m_ccRoot)
    {
        m_ccRoot->getSelectedEntities(m_selectedEntities, CC_TYPES::OBJECT, &selInfo);
    }
    if(m_selectedEntities.size() == 0){
        ccConsole::Warning(QString::fromLocal8Bit("点云实体无法选中异常，请确认已存在实体选中!"));
        return false;
    }
    return true;
}

void MainWindow::unSelectedEntities(int wich){
    m_ccRoot->unselectAllEntities();
}

void MainWindow::setView( CC_VIEW_ORIENTATION view )
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->setView(view);
    }
}

ccGLWindow* MainWindow::getActiveGLWindow(){
    return view3D;
}

void MainWindow::repositionOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos){

    int dx = 0;
    int dy = 0;
    static const int margin = 5;
    switch (pos)
    {
    case Qt::TopLeftCorner:
        dx = margin;
        dy = margin;
        break;
    case Qt::TopRightCorner:
        dx = std::max(margin, ui->ccDataView->width() - dlg->width() - margin);
        dy = margin;
        break;
    case Qt::BottomLeftCorner:
        dx = margin;
        dy = std::max(margin, ui->ccDataView->height() - dlg->height() - margin);
        break;
    case Qt::BottomRightCorner:
        dx = std::max(margin, ui->ccDataView->width() - dlg->width() - margin);
        dy = std::max(margin, ui->ccDataView->height() - dlg->height() - margin);
        break;
    }

    dlg->move(ui->ccDataView->mapToGlobal(QPoint(dx, dy)));
    dlg->raise();
}

void MainWindow::on_actionHeightRamp_triggered()
{
    //先进行选中
    if(!SelectedEntitiesSet()){return ;}
    unsigned char dim = 2;//0-X,1-Y,2-Z哪一个维度
    //ccColorGradientDlg::GradientType ramp = dlg.getType();

    //默认
    ccColorScale::Shared colorScale(nullptr);
    colorScale = ccColorScalesManager::GetDefaultScale();

//     自定义两色过度
//    colorScale = ccColorScale::Create("Temp scale");
//    QColor first;
//    QColor second;
//    dlg.getColors(first,second);
//    colorScale->insert(ccColorScaleElement(0.0, first), false);
//    colorScale->insert(ccColorScaleElement(1.0, second), true);

    const double frequency = 1.000;
    for (ccHObject* ent : getSelectedEntities())
    {
        bool lockedVertices = false;
        //cloud 返回一直为 0x00; 因为选中的类型实体一直不对。
        ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(ent,&lockedVertices);
        if (lockedVertices)
        {
            //ccUtils::DisplayLockedVerticesWarning(ent->getName(), selectedEntities.size() == 1);
            ui->RecordList->addItem(ent->getName()+QString::fromLocal8Bit(":点云实体被锁定!"));
            continue;
        }

        if (cloud && cloud->isA(CC_TYPES::POINT_CLOUD)) // TODO
        {
            ccPointCloud* pc = static_cast<ccPointCloud*>(cloud);

            bool success = false;
            //if (ramp == ccColorGradientDlg::Banding)
                //success = pc->setRGBColorByBanding(dim, frequency);
            //else
                success = pc->setRGBColorByHeight(dim, colorScale);

            if (success)
            {
                ent->showColors(true);
                ent->showSF(false); //just in case
                ent->prepareDisplayForRefresh();
            }
        }
    }
    GLWindowFromWidget(viewWidget)->refresh(false);
    //需要刷新相关窗口
    //updateUIWithSelection();
}


void MainWindow::activateClippingBoxMode()
{
    if(!SelectedEntitiesSet()){return ;}
    ccGLWindow* win = getActiveGLWindow();
    if (!win)
    {
        return;
    }
    m_clipTool->linkWith(win);

    ccHObject::Container selectedEntities = getSelectedEntities(); //we have to use a local copy: 'unselectEntity' will change the set of currently selected entities!
    for (ccHObject *entity : selectedEntities)
    {
        if (m_clipTool->addAssociatedEntity(entity))
        {
            //automatically deselect the entity (to avoid seeing its bounding box ;)
            //m_ccRoot->unselectEntity(entity);
        }
    }

    if (m_clipTool->getNumberOfAssociatedEntity() == 0)
    {
        m_clipTool->close();
        return;
    }

    if (m_clipTool->start())
    {
        repositionOverlayDialog(m_clipTool, Qt::TopRightCorner);
    }
    else
    {
        ccConsole::Error(QString::fromLocal8Bit("意外的错误!"));
    }
}


void MainWindow::echoMouseWheelRotate(float wheelDelta_deg)
{

    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;

}

void MainWindow::echoCameraDisplaced(float ddx, float ddy)
{

    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;

}

void MainWindow::echoBaseViewMatRotation(const ccGLMatrixd& rotMat)
{
    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;

}

void MainWindow::echoCameraPosChanged(const CCVector3d& P)
{
    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;

}

void MainWindow::echoPivotPointChanged(const CCVector3d& P)
{
    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;
}

void MainWindow::echoPixelSizeChanged(float pixelSize)
{
    ccGLWindow* sendingWindow = dynamic_cast<ccGLWindow*>(sender());
    if (!sendingWindow)
        return;
}


void MainWindow::prepareWindowDeletion(QObject* glWindow)
{
//    if (!m_ccRoot)
//        return;

    //we assume only ccGLWindow can be connected to this slot!
    ccGLWindow* win = qobject_cast<ccGLWindow*>(glWindow);

//    m_ccRoot->hidePropertiesView();
//    m_ccRoot->getRootEntity()->removeFromDisplay_recursive(win);
//    m_ccRoot->updatePropertiesView();
}

void MainWindow::onExclusiveFullScreenToggled(bool state)
{
    //we simply update the fullscreen action method icon (whatever the window)
    ccGLWindow* win = getActiveGLWindow();

    if ( win == nullptr )
        return;

//    m_UI->actionExclusiveFullScreen->blockSignals(true);
//    m_UI->actionExclusiveFullScreen->setChecked(win ? win->exclusiveFullScreen() : false);
//    m_UI->actionExclusiveFullScreen->blockSignals(false);

//    if (	!state
//        &&	win->stereoModeIsEnabled()
//        &&	(	win->getStereoParams().glassType == ccGLWindow::StereoParams::NVIDIA_VISION
//            ||	win->getStereoParams().glassType == ccGLWindow::StereoParams::GENERIC_STEREO_DISPLAY ))
//    {
//        //auto disable stereo mode as NVidia Vision only works in full screen mode!
//        m_UI->actionEnableStereo->setChecked(false);
//    }
}

void MainWindow::handleNewLabel(ccHObject* entity)
{
    if (entity)
    {
        addToDB(entity);
        //需要做个分支判断，拾取的是点才做下面的处理
        //此处将对应的标签保存到自己的文件格式中
        cc2DLabel *label = (cc2DLabel *)entity;
        ccLog::Print(QString::fromLocal8Bit("[点域加入]\t- ")+label->getName());
    }
    else
    {
        assert(false);
    }
}

void MainWindow::addToDBAuto(const QStringList& filenames)
{
    ccGLWindow* win = qobject_cast<ccGLWindow*>(QObject::sender());
    addToDB(filenames, QString(), win);
}

ccHObject *MainWindow::LoadFileTo_ccobject(QString filepath){
    if(filepath.size()<2){
        ccConsole::Error(QString::fromLocal8Bit("加载文件到CC对象失败:")+filepath);
        return 0;
    }
    QSettings settings;
    settings.beginGroup(ccPS::LoadFile());
    QString currentPath = settings.value(ccPS::CurrentPath(), ccFileUtils::defaultDocPath()).toString();
    QString currentOpenDlgFilter = settings.value(ccPS::SelectedInputFilter(), BinFilter::GetFileFilter()).toString();

    const QStringList filterStrings = FileIOFilter::ImportFilterList();
    const QString &allFilter = filterStrings.at( 0 );

    if ( !filterStrings.contains( currentOpenDlgFilter ) )
    {
        currentOpenDlgFilter = allFilter;
    }

    currentPath = QFileInfo(filepath).absolutePath();
    settings.setValue(ccPS::CurrentPath(),currentPath);
    settings.setValue(ccPS::SelectedInputFilter(),currentOpenDlgFilter);
    settings.endGroup();

    if (currentOpenDlgFilter == allFilter)
    {
        currentOpenDlgFilter.clear(); //this way FileIOFilter will try to guess the file type automatically!
    }

    CCVector3d loadCoordinatesShift(0,0,0);
    bool loadCoordinatesTransEnabled = false;

    FileIOFilter::LoadParameters parameters;
    {
        parameters.alwaysDisplayLoadDialog = true;
        parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
        parameters.coordinatesShift = &loadCoordinatesShift;
        parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;
        parameters.parentWidget = this;
    }

    //the same for 'addToDB' (if the first one is not supported, or if the scale remains too big)
    CCVector3d addCoordinatesShift(0, 0, 0);

    const ccOptions& options = ccOptions::Instance();
    FileIOFilter::ResetSesionCounter();
    CC_FILE_ERROR result = CC_FERR_NO_ERROR;
    ccHObject* newGroup = FileIOFilter::LoadFromFile(filepath, parameters, result, currentOpenDlgFilter);
    if (newGroup)
    {
        if (!options.normalsDisplayedByDefault)
        {
            //disable the normals on all loaded clouds!
            ccHObject::Container clouds;
            newGroup->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);
            for (ccHObject* cloud : clouds)
            {
                if (cloud)
                {
                    static_cast<ccGenericPointCloud*>(cloud)->showNormals(false);
                }
            }
            return newGroup;
        }
        ccConsole::Error(QString::fromLocal8Bit("从文件加载到CC对象失败，默认法向量显示未开启!"));
        return 0;
    }
    return 0;
}

void MainWindow::on_actionCCfile_triggered()
{
    //persistent settings
    QSettings settings;
    settings.beginGroup(ccPS::LoadFile());
    QString currentPath = settings.value(ccPS::CurrentPath(), ccFileUtils::defaultDocPath()).toString();
    QString currentOpenDlgFilter = settings.value(ccPS::SelectedInputFilter(), BinFilter::GetFileFilter()).toString();

    // Add all available file I/O filters (with import capabilities)
    const QStringList filterStrings = FileIOFilter::ImportFilterList();
    const QString &allFilter = filterStrings.at( 0 );

    if ( !filterStrings.contains( currentOpenDlgFilter ) )
    {
        currentOpenDlgFilter = allFilter;
    }
    //default file filter separator
    const QString s_fileFilterSeparator(";;");
    //file choosing dialog
    QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
                                                              QString::fromLocal8Bit("打开点云文件"),
                                                              currentPath,
                                                              filterStrings.join(s_fileFilterSeparator),
                                                              &currentOpenDlgFilter,
                                                              CCFileDialogOptions());
    if (selectedFiles.isEmpty())
        return;

    //save last loading parameters
    currentPath = QFileInfo(selectedFiles[0]).absolutePath();
    settings.setValue(ccPS::CurrentPath(),currentPath);
    settings.setValue(ccPS::SelectedInputFilter(),currentOpenDlgFilter);
    settings.endGroup();

    if (currentOpenDlgFilter == allFilter)
    {
        currentOpenDlgFilter.clear(); //this way FileIOFilter will try to guess the file type automatically!
    }

    //load files
    addToDB(selectedFiles, currentOpenDlgFilter);

    //------选中----
    //if(!SelectedEntitiesSet()){return;}
    m_pluginUIManager->handleSelectionChanged();
    m_pluginUIManager->updateMenus();
    QList<QAction*> als = ui->thrdPluger->actions();
    if(!als.isEmpty()){
        for(unsigned x =0;x< als.size();x++)
        als.at(x)->setEnabled(true);
    }
}

void MainWindow::addToDB(const QStringList& filenames,QString fileFilter,ccGLWindow* destWin)
{
    //to use the same 'global shift' for multiple files
    CCVector3d loadCoordinatesShift(0,0,0);
    bool loadCoordinatesTransEnabled = false;

    FileIOFilter::LoadParameters parameters;
    {
        parameters.alwaysDisplayLoadDialog = true;
        parameters.shiftHandlingMode = ccGlobalShiftManager::DIALOG_IF_NECESSARY;
        parameters.coordinatesShift = &loadCoordinatesShift;
        parameters.coordinatesShiftEnabled = &loadCoordinatesTransEnabled;
        parameters.parentWidget = this;
    }

    //the same for 'addToDB' (if the first one is not supported, or if the scale remains too big)
    CCVector3d addCoordinatesShift(0, 0, 0);

    const ccOptions& options = ccOptions::Instance();
    FileIOFilter::ResetSesionCounter();

    for ( const QString &filename : filenames )
    {
        CC_FILE_ERROR result = CC_FERR_NO_ERROR;
        ccHObject* newGroup = FileIOFilter::LoadFromFile(filename, parameters, result, fileFilter);

        if (newGroup)
        {
            if (!options.normalsDisplayedByDefault)
            {
                //disable the normals on all loaded clouds!
                ccHObject::Container clouds;
                newGroup->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);
                for (ccHObject* cloud : clouds)
                {
                    if (cloud)
                    {
                        static_cast<ccGenericPointCloud*>(cloud)->showNormals(false);
                    }
                }
            }

            if (destWin)
            {
                newGroup->setDisplay_recursive(destWin);
            }
            addToDB(newGroup, true, true, false);
        }

        if (result == CC_FERR_CANCELED_BY_USER)
        {
            //stop importing the file if the user has cancelled the current process!
            break;
        }
    }
}

void MainWindow::addToDB(ccHObject* obj,bool updateZoom/*=true*/,
                         bool autoExpandDBTree/*=true*/,
                         bool checkDimensions/*=true*/,
                         bool autoRedraw/*=true*/)
{
    if (checkDimensions)
    {
        //get entity bounding box
        ccBBox bBox = obj->getBB_recursive();

        CCVector3 center = bBox.getCenter();
        PointCoordinateType diag = bBox.getDiagNorm();

        CCVector3d P = CCVector3d::fromArray(center.u);
        CCVector3d Pshift(0, 0, 0);
        double scale = 1.0;
        bool preserveCoordinateShift = true;
        //here we must test that coordinates are not too big whatever the case because OpenGL
        //really doesn't like big ones (even if we work with GLdoubles :( ).
        if (ccGlobalShiftManager::Handle(P, diag, ccGlobalShiftManager::DIALOG_IF_NECESSARY, false, Pshift, &preserveCoordinateShift, &scale))
        {
            bool needRescale = (scale != 1.0);
            bool needShift = (Pshift.norm2() > 0);

            if (needRescale || needShift)
            {
                ccGLMatrix mat;
                mat.toIdentity();
                mat.data()[0] = mat.data()[5] = mat.data()[10] = static_cast<float>(scale);
                mat.setTranslation(Pshift);
                obj->applyGLTransformation_recursive(&mat);
                ccConsole::Warning(QString("Entity '%1' has been translated: (%2,%3,%4) and rescaled of a factor %5 [original position will be restored when saving]").arg(obj->getName()).arg(Pshift.x,0,'f',2).arg(Pshift.y,0,'f',2).arg(Pshift.z,0,'f',2).arg(scale,0,'f',6));
            }

            //update 'global shift' and 'global scale' for ALL clouds recursively
            if (preserveCoordinateShift)
            {
                //FIXME: why don't we do that all the time by the way?!
                ccHObject::Container children;
                children.push_back(obj);
                while (!children.empty())
                {
                    ccHObject* child = children.back();
                    children.pop_back();

                    if (child->isKindOf(CC_TYPES::POINT_CLOUD))
                    {
                        ccGenericPointCloud* pc = ccHObjectCaster::ToGenericPointCloud(child);
                        pc->setGlobalShift(pc->getGlobalShift() + Pshift);
                        pc->setGlobalScale(pc->getGlobalScale() * scale);
                    }
                    for (unsigned i = 0; i < child->getChildrenNumber(); ++i)
                    {
                        children.push_back(child->getChild(i));
                    }
                }
            }
        }
    }
    if (m_ccRoot)
    {
        //force a 'global zoom' if the DB was emtpy!
        if (!m_ccRoot->getRootEntity() || m_ccRoot->getRootEntity()->getChildrenNumber() == 0)
        {
            updateZoom = true;
        }
        m_ccRoot->addElement(obj, autoExpandDBTree);
    }
    else
    {
        ccLog::Warning(QString::fromLocal8Bit("[addToDB]内部错误db没有被关联?!"));
        assert(false);
    }

    //we can now set destination display (if none already)
    if (!obj->getDisplay())
    {
        //ccGLWindow* activeWin = getActiveGLWindow();
        ccGLWindow* activeWin = view3D;
        if (!activeWin)
        {
            //no active GL window?!
            return;
        }
        obj->setDisplay_recursive(activeWin);
    }

    //eventually we update the corresponding display
    assert(obj->getDisplay());
    if (updateZoom)
    {
        static_cast<ccGLWindow*>(obj->getDisplay())->zoomGlobal(); //automatically calls ccGLWindow::redraw
    }
    else if (autoRedraw)
    {
        obj->redrawDisplay();
    }
}


void MainWindow::forceConsoleDisplay()
{
    if (ui->dockWidget_Log && ui->dockWidget_Log->isHidden())
    {
        ui->dockWidget_Log->show();
        QApplication::processEvents();
    }
}

ccUniqueIDGenerator::Shared MainWindow::getUniqueIDGenerator()
{
    return ccObject::GetUniqueIDGenerator();
}

void MainWindow::removeFromDB(ccHObject* obj, bool autoDelete/*=true*/)
{
    if (!obj)
        return;
    if (!autoDelete && obj->getParent())
        obj->getParent()->removeDependencyWith(obj);
    if (m_ccRoot)
        m_ccRoot->removeElement(obj);
}

void MainWindow::setSelectedInDB(ccHObject* obj, bool selected)
{
    if (obj && m_ccRoot)
    {
        if (selected)
            m_ccRoot->selectEntity(obj);
        else
            m_ccRoot->unselectEntity(obj);
    }
}


void MainWindow::dispToConsole(QString message, ConsoleMessageLevel level/*=STD_CONSOLE_MESSAGE*/)
{
    switch(level)
    {
    case STD_CONSOLE_MESSAGE:
        ccConsole::Print(message);
        break;
    case WRN_CONSOLE_MESSAGE:
        ccConsole::Warning(message);
        break;
    case ERR_CONSOLE_MESSAGE:
        ccConsole::Error(message);
        break;
    }
}


ccHObject* MainWindow::dbRootObject()
{
    return (m_ccRoot ? m_ccRoot->getRootEntity() : nullptr);
}


void MainWindow::redrawAll(bool only2D/*=false*/)
{
    GLWindowFromWidget(viewWidget)->redraw(only2D);
}

void MainWindow::refreshAll(bool only2D/*=false*/)
{
    GLWindowFromWidget(viewWidget)->refresh(only2D);
}





ccColorScalesManager* MainWindow::getColorScalesManager()
{
    return ccColorScalesManager::GetUniqueInstance();
}

void MainWindow::spawnHistogramDialog(const std::vector<unsigned>& histoValues, double minVal, double maxVal, QString title, QString xAxisLabel)
{
}

void MainWindow::toggleActiveWindowCenteredPerspective()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        const ccViewportParameters& params = win->getViewportParameters();
//        if (params.perspectiveView && params.objectCenteredView && !checkStereoMode(win)) //we need to check this only if we are already in object-centered perspective mode
//        {
//            return;
//        }
//        win->togglePerspective(true);
//        win->redraw(false);
//        updateViewModePopUpMenu(win);
//        updatePivotVisibilityPopUpMenu(win);
    }
}

void MainWindow::toggleActiveWindowCustomLight()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->toggleCustomLight();
        win->redraw(false);
    }
}

void MainWindow::toggleActiveWindowSunLight()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->toggleSunLight();
        win->redraw(false);
    }
}

void MainWindow::toggleActiveWindowViewerBasedPerspective()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        const ccViewportParameters& params = win->getViewportParameters();
        //if (params.perspectiveView && !params.objectCenteredView && !checkStereoMode(win)) //we need to check this only if we are already in viewer-based perspective mode
        if (params.perspectiveView && !params.objectCenteredView )//本项目中不支持StereoMode
        {
            return;
        }
        win->togglePerspective(false);
        win->redraw(false);
    }
}

void MainWindow::zoomOnSelectedEntities()
{
    ccGLWindow* win = getActiveGLWindow();

    ccHObject tempGroup("TempGroup");
    size_t selNum = m_selectedEntities.size();
    for (size_t i = 0; i < selNum; ++i)
    {
        ccHObject *entity = m_selectedEntities[i];

        if (i == 0 || !win)
        {
            //take the first valid window as reference
            win = static_cast<ccGLWindow*>(entity->getDisplay());
        }

        if (win)
        {
            if (entity->getDisplay() == win)
            {
                tempGroup.addChild(entity,ccHObject::DP_NONE);
            }
            else if (entity->getDisplay() != nullptr)
            {
                ccLog::Error("All selected entities must be displayed in the same 3D view!");
                return;
            }
        }
    }

    if (tempGroup.getChildrenNumber() != 0)
    {
        ccBBox box = tempGroup.getDisplayBB_recursive(false, win);
        if (!box.isValid())
        {
            ccLog::Warning("Selected entities have no valid bounding-box!");
        }
        else
        {
            if ( win != nullptr )
            {
                win->updateConstellationCenterAndZoom(&box);
            }
        }
    }
}


void MainWindow::setGlobalZoom()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
        win->zoomGlobal();
}

void MainWindow::increasePointSize()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->setPointSize(win->getViewportParameters().defaultPointSize + 1);
        win->redraw();
    }
}

void MainWindow::decreasePointSize()
{
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->setPointSize(win->getViewportParameters().defaultPointSize - 1);
        win->redraw();
    }
}

void MainWindow::on_actionLCC_triggered()
{
    if(!SelectedEntitiesSet()){return ;}
    std::vector<ccGenericPointCloud*> clouds;
    {
        for ( ccHObject *entity : getSelectedEntities() )
        {
            if (entity->isKindOf(CC_TYPES::POINT_CLOUD))
                clouds.push_back(ccHObjectCaster::ToGenericPointCloud(entity));
        }
    }

    size_t count = clouds.size();
    if (count == 0)
        return;

//    ccLabelingDlg dlg(this);
//    if (count == 1)
//        dlg.octreeLevelSpinBox->setCloud(clouds.front());
//    if (!dlg.exec())
//        return;

    int octreeLevel = 8;//dlg.getOctreeLevel();
    unsigned minComponentSize = static_cast<unsigned>(std::max(0, 10/*dlg.getMinPointsNb()*/));
    bool randColors = false;//dlg.randomColors();

    ccProgressDialog pDlg(false, this);
    pDlg.setAutoClose(false);

    //we unselect all entities as we are going to automatically select the created components
    //(otherwise the user won't perceive the change!)
//    if (m_ccRoot)
//    {
//        m_ccRoot->unselectAllEntities();
//    }

    for ( ccGenericPointCloud *cloud : clouds )
    {
        if (cloud && cloud->isA(CC_TYPES::POINT_CLOUD))
        {
            ccPointCloud* pc = static_cast<ccPointCloud*>(cloud);

            ccOctree::Shared theOctree = cloud->getOctree();
            if (!theOctree)
            {
                ccProgressDialog pOctreeDlg(true, this);
                theOctree = cloud->computeOctree(&pOctreeDlg);
                if (!theOctree)
                {
                    ccConsole::Error(QString("Couldn't compute octree for cloud '%1'!").arg(cloud->getName()));
                    break;
                }
            }
            //we create/activate CCs label scalar field
            int sfIdx = pc->getScalarFieldIndexByName(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            if (sfIdx < 0)
            {
                sfIdx = pc->addScalarField(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            }
            if (sfIdx < 0)
            {
                ccConsole::Error("Couldn't allocate a new scalar field for computing CC labels! Try to free some memory ...");
                break;
            }
            pc->setCurrentScalarField(sfIdx);

            //we try to label all CCs
            CCLib::ReferenceCloudContainer components;
            int componentCount = CCLib::AutoSegmentationTools::labelConnectedComponents(cloud,
                                                                                        static_cast<unsigned char>(octreeLevel),
                                                                                        false,
                                                                                        &pDlg,
                                                                                        theOctree.data());

            if (componentCount >= 0)
            {
                //safety test
                int realComponentCount = 0;
                {
                    for (size_t i = 0; i < components.size(); ++i)
                    {
                        if (components[i]->size() >= minComponentSize)
                        {
                            ++realComponentCount;
                        }
                    }
                }
                if (realComponentCount > 500)
                {
                    if (QMessageBox::warning(this, QString::fromLocal8Bit("连通域太多"), QString("Do you really expect up to %1 components?\n(this may take a lot of time to process and display)").arg(realComponentCount), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
                    {
                        //cancel
                        pc->deleteScalarField(sfIdx);
                        if (pc->getNumberOfScalarFields() != 0)
                        {
                            pc->setCurrentDisplayedScalarField(static_cast<int>(pc->getNumberOfScalarFields()) - 1);
                        }
                        else
                        {
                            pc->showSF(false);
                        }
                        pc->prepareDisplayForRefresh();
                        continue;
                    }
                }
                pc->getCurrentInScalarField()->computeMinAndMax();
                if (!CCLib::AutoSegmentationTools::extractConnectedComponents(cloud, components))
                {
                    ccConsole::Warning(QString::fromLocal8Bit("[LCC]从%1释放cloud到CCs出现了一些错误!").arg(cloud->getName()));
                }
            }
            else
            {
                ccConsole::Warning(QString::fromLocal8Bit("[LCC]从%1释放cloud到CCs出现了一些错误!").arg(cloud->getName()));
            }

            //we delete the CCs label scalar field (we don't need it anymore)
            pc->deleteScalarField(sfIdx);
            sfIdx = -1;

            if (!components.empty())
            {
                int maxPoint = -1;
                int maxVindex = -1;
                //此时还是无序的， 过虑一下 components 不需要的删除掉,只需要取最大的一个就可以,但是不能区分出两个相同一样的最大
                int slen = components.size();
                int s=0;
                for(unsigned m =0;m< slen;m++){
                    s=components[m]->size();
                    if(s > maxPoint){
                        maxPoint = s;
                        maxVindex = m;
                    }
                }
                CCLib::ReferenceCloudContainer Ccs;
                if(maxPoint >0 && maxVindex > -1){
                    Ccs.push_back(components[maxVindex]);
                }else{
                    Ccs = components;
                }
                ccConsole::Print(QString::fromLocal8Bit("[LCC]已经分割出%1个连通域,将仅保留最大的一个！").arg(slen));
                //createComponentsClouds(cloud, components, minComponentSize, randColors, true);
                createComponentsClouds(cloud, Ccs, minComponentSize, randColors, true);
            }
        }
    }
}

struct ComponentIndexAndSize
{
    unsigned index;
    unsigned size;

    ComponentIndexAndSize(unsigned i, unsigned s) : index(i), size(s) {}

    static bool DescendingCompOperator(const ComponentIndexAndSize& a, const ComponentIndexAndSize& b)
    {
        return a.size > b.size;
    }
};

void MainWindow::createComponentsClouds(ccGenericPointCloud* cloud,
                                        CCLib::ReferenceCloudContainer& components,
                                        unsigned minPointsPerComponent,
                                        bool randomColors,
                                        bool selectComponents,
                                        bool sortBysize/*=true*/)
{
    if (!cloud || components.empty())
        return;
//      //本项目中不需要，已经做了前置过滤，传入的时候只有一个了；
//    std::vector<ComponentIndexAndSize> sortedIndexes;
//    std::vector<ComponentIndexAndSize>* _sortedIndexes = nullptr;
//    if (sortBysize)
//    {
//        try
//        {
//            sortedIndexes.reserve(components.size());
//        }
//        catch (const std::bad_alloc&)
//        {
//            ccLog::Warning("[LCC]创建点云实体群没有足够内存按尺寸排序!");
//            sortBysize = false;
//        }

//        if (sortBysize) //still ok?
//        {
//            unsigned compCount = static_cast<unsigned>(components.size());
//            for (unsigned i = 0; i < compCount; ++i)
//            {
//                sortedIndexes.emplace_back(i, components[i]->size());
//            }

//            ParallelSort(sortedIndexes.begin(), sortedIndexes.end(), ComponentIndexAndSize::DescendingCompOperator);
//            _sortedIndexes = &sortedIndexes;
//        }
//    }
    //we create "real" point clouds for all input components
    {
        ccPointCloud* pc = cloud->isA(CC_TYPES::POINT_CLOUD) ? static_cast<ccPointCloud*>(cloud) : nullptr;
        //we create a new group to store all CCs
        ccHObject* ccGroup = new ccHObject(cloud->getName() + QString(" [CCs]"));
        for (size_t i = 0; i < components.size(); ++i)
        {
            //CCLib::ReferenceCloud* compIndexes = _sortedIndexes ? components[_sortedIndexes->at(i).index] : components[i];
            CCLib::ReferenceCloud* compIndexes = components[i];
            //if it has enough points
            if (compIndexes->size() >= minPointsPerComponent)
            {
                //we create a new entity
                ccPointCloud* compCloud = (pc ? pc->partialClone(compIndexes) : ccPointCloud::From(compIndexes));
                if (compCloud)
                {
                    //shall we colorize it with random color?
                    if (randomColors)
                    {
                        ccColor::Rgb col = ccColor::Generator::Random();
                        compCloud->setColor(col);
                        compCloud->showColors(true);
                        compCloud->showSF(false);
                    }

                    //'shift on load' information
                    if (pc)
                    {
                        compCloud->setGlobalShift(pc->getGlobalShift());
                        compCloud->setGlobalScale(pc->getGlobalScale());
                    }
                    compCloud->setVisible(true);
                    compCloud->setName(QString("CC#%1").arg(ccGroup->getChildrenNumber()));

                    ccGroup->addChild(compCloud);

//                    if (selectComponents && m_ccRoot)
//                        m_ccRoot->selectEntity(compCloud, true);
                    if(selectComponents){
                        compCloud->setSelected(true);
                    }
                }
                else
                {
                    ccConsole::Warning(
                        QString::fromLocal8Bit("[LCC]创建领域部件失败 #%i! (没有足够的内存)",ccGroup->getChildrenNumber()+1));
                }
            }

            delete compIndexes;
            compIndexes = nullptr;
        }

        components.clear();

        if (ccGroup->getChildrenNumber() == 0)
        {
            ccConsole::Error(QString::fromLocal8Bit("[LCC]没有连通域可创建,请检查:minimum size..."));
            delete ccGroup;
            ccGroup = nullptr;
        }
        else
        {
            ccGroup->setDisplay(cloud->getDisplay());
            addToDB(ccGroup);
            ccConsole::Print(QString::fromLocal8Bit("[LCC]从'%1'创建了%2个连通域!").arg(cloud->getName()).arg(ccGroup->getChildrenNumber()));
        }
        cloud->prepareDisplayForRefresh();
        if (ccGroup)
        {
            //cloud->setEnabled(false);
            ccConsole::Print(QString::fromLocal8Bit("[LCC]原像已经删除!"));
            m_ccRoot->removeElement(cloud);
            //m_ccRoot->deleteSelectedEntities();
        }
    }
}

static bool IsValidFileName(QString filename)
{
#ifdef CC_WINDOWS
    QString sPattern("^(?!^(PRN|AUX|CLOCK\\$|NUL|CON|COM\\d|LPT\\d|\\..*)(\\..+)?$)[^\\x00-\\x1f\\\\?*:\\"";|/]+$");
#else
    QString sPattern("^(([a-zA-Z]:|\\\\)\\\\)?(((\\.)|(\\.\\.)|([^\\\\/:\\*\\?""\\|<>\\. ](([^\\\\/:\\*\\?""\\|<>\\. ])|([^\\\\/:\\*\\?""\\|<>]*[^\\\\/:\\*\\?""\\|<>\\. ]))?))\\\\)*[^\\\\/:\\*\\?""\\|<>\\. ](([^\\\\/:\\*\\?""\\|<>\\. ])|([^\\\\/:\\*\\?""\\|<>]*[^\\\\/:\\*\\?""\\|<>\\. ]))?$");
#endif
    return QRegExp(sPattern).exactMatch(filename);
}

void MainWindow::on_actionPCSave_triggered()
{
    if(!SelectedEntitiesSet()){return ;}
    ccHObject clouds("clouds");
    ccHObject meshes("meshes");
    ccHObject images("images");
    ccHObject polylines("polylines");
    ccHObject other("other");
    ccHObject otherSerializable("serializable");
    ccHObject::Container entitiesToDispatch;
    entitiesToDispatch.insert(entitiesToDispatch.begin(), m_selectedEntities.begin(), m_selectedEntities.end());
    ccHObject entitiesToSave;
    while (!entitiesToDispatch.empty())
    {
        ccHObject* child = entitiesToDispatch.back();
        entitiesToDispatch.pop_back();

        if (child->isA(CC_TYPES::HIERARCHY_OBJECT))
        {
            for (unsigned j = 0; j < child->getChildrenNumber(); ++j)
                entitiesToDispatch.push_back(child->getChild(j));
        }
        else
        {
            ccHObject* dest = nullptr;
            if (child->isA(CC_TYPES::POINT_CLOUD))
                dest = &clouds;
            else if (child->isKindOf(CC_TYPES::MESH))
                dest = &meshes;
            else if (child->isKindOf(CC_TYPES::IMAGE))
                dest = &images;
            else if (child->isKindOf(CC_TYPES::POLY_LINE))
                dest = &polylines;
            else if (child->isSerializable())
                dest = &otherSerializable;
            else
                dest = &other;

            assert(dest);

            //we don't want double insertions if the user has clicked both the father and child
            if (!dest->find(child->getUniqueID()))
            {
                dest->addChild(child, ccHObject::DP_NONE);
                entitiesToSave.addChild(child, ccHObject::DP_NONE);
            }
        }
    }

    bool hasCloud = (clouds.getChildrenNumber() != 0);
    bool hasMesh = (meshes.getChildrenNumber() != 0);
    bool hasImages = (images.getChildrenNumber() != 0);
    bool hasPolylines = (polylines.getChildrenNumber() != 0);
    bool hasSerializable = (otherSerializable.getChildrenNumber() != 0);
    bool hasOther = (other.getChildrenNumber() != 0);

    int stdSaveTypes =		static_cast<int>(hasCloud)
                       +	static_cast<int>(hasMesh)
                       +	static_cast<int>(hasImages)
                       +	static_cast<int>(hasPolylines)
                       +	static_cast<int>(hasSerializable);
    if (stdSaveTypes == 0)
    {
        ccConsole::Error("无法通过这种方法保存选中的实体!");
        return;
    }

    QStringList fileFilters;
    {
        for ( const FileIOFilter::Shared &filter : FileIOFilter::GetFilters() )
        {
            bool atLeastOneExclusive = false;

            //can this filter export one or several clouds?
            bool canExportClouds = true;
            if (hasCloud)
            {
                bool isExclusive = true;
                bool multiple = false;
                canExportClouds = (		filter->canSave(CC_TYPES::POINT_CLOUD, multiple, isExclusive)
                                   &&	(multiple || clouds.getChildrenNumber() == 1) );
                atLeastOneExclusive |= isExclusive;
            }

            //can this filter export one or several meshes?
            bool canExportMeshes = true;
            if (hasMesh)
            {
                bool isExclusive = true;
                bool multiple = false;
                canExportMeshes = (		filter->canSave(CC_TYPES::MESH, multiple, isExclusive)
                                   &&	(multiple || meshes.getChildrenNumber() == 1) );
                atLeastOneExclusive |= isExclusive;
            }

            //can this filter export one or several polylines?
            bool canExportPolylines = true;
            if (hasPolylines)
            {
                bool isExclusive = true;
                bool multiple = false;
                canExportPolylines = (	filter->canSave(CC_TYPES::POLY_LINE, multiple, isExclusive)
                                      &&	(multiple || polylines.getChildrenNumber() == 1) );
                atLeastOneExclusive |= isExclusive;
            }

            //can this filter export one or several images?
            bool canExportImages = true;
            if (hasImages)
            {
                bool isExclusive = true;
                bool multiple = false;
                canExportImages = (		filter->canSave(CC_TYPES::IMAGE, multiple, isExclusive)
                                   &&	(multiple || images.getChildrenNumber() == 1) );
                atLeastOneExclusive |= isExclusive;
            }

            //can this filter export one or several other serializable entities?
            bool canExportSerializables = true;
            if (hasSerializable)
            {
                //check if all entities have the same type
                {
                    CC_CLASS_ENUM firstClassID = otherSerializable.getChild(0)->getUniqueID();
                    for (unsigned j = 1; j < otherSerializable.getChildrenNumber(); ++j)
                    {
                        if (otherSerializable.getChild(j)->getUniqueID() != firstClassID)
                        {
                            //we add a virtual second 'stdSaveType' so as to properly handle exlusivity
                            ++stdSaveTypes;
                            break;
                        }
                    }
                }

                for (unsigned j = 0; j < otherSerializable.getChildrenNumber(); ++j)
                {
                    ccHObject* child = otherSerializable.getChild(j);
                    bool isExclusive = true;
                    bool multiple = false;
                    canExportSerializables &= (		filter->canSave(child->getClassID(), multiple, isExclusive)
                                               &&	(multiple || otherSerializable.getChildrenNumber() == 1) );
                    atLeastOneExclusive |= isExclusive;
                }
            }

            bool useThisFilter =	canExportClouds
                                 &&	canExportMeshes
                                 &&	canExportImages
                                 &&	canExportPolylines
                                 &&	canExportSerializables
                                 &&	(!atLeastOneExclusive || stdSaveTypes == 1);

            if (useThisFilter)
            {
                QStringList ff = filter->getFileFilters(false);
                for (int j = 0; j < ff.size(); ++j)
                    fileFilters.append(ff[j]);
            }
        }
    }


    //persistent settings
    QSettings settings;
    settings.beginGroup(ccPS::SaveFile());

    //default filter
    QString selectedFilter = fileFilters.first();
    if (hasCloud)
        selectedFilter = settings.value(ccPS::SelectedOutputFilterCloud(),selectedFilter).toString();
    else if (hasMesh)
        selectedFilter = settings.value(ccPS::SelectedOutputFilterMesh(), selectedFilter).toString();
    else if (hasImages)
        selectedFilter = settings.value(ccPS::SelectedOutputFilterImage(), selectedFilter).toString();
    else if (hasPolylines)
        selectedFilter = settings.value(ccPS::SelectedOutputFilterPoly(), selectedFilter).toString();

    //default output path (+ filename)
    QString currentPath = settings.value(ccPS::CurrentPath(), ccFileUtils::defaultDocPath()).toString();
    QString fullPathName = currentPath;

    if (haveOneSelection())
    {
        //hierarchy objects have generally as name: 'filename.ext (fullpath)'
        //so we must only take the first part! (otherwise this type of name
        //with a path inside perturbs the QFileDialog a lot ;))
        QString defaultFileName(m_selectedEntities.front()->getName());
        if (m_selectedEntities.front()->isA(CC_TYPES::HIERARCHY_OBJECT))
        {
            QStringList parts = defaultFileName.split(' ', QString::SkipEmptyParts);
            if (!parts.empty())
            {
                defaultFileName = parts[0];
            }
        }

        //we remove the extension
        defaultFileName = QFileInfo(defaultFileName).baseName();

        if (!IsValidFileName(defaultFileName))
        {
            ccLog::Warning("[I/O] First entity's name would make an invalid filename! Can't use it...");
            defaultFileName = "project";
        }

        fullPathName += QString("/") + defaultFileName;
    }

    const QString s_fileFilterSeparator(";;");
    QString selectedFilename = QFileDialog::getSaveFileName(this,
                                                            QString::fromLocal8Bit("文件另存为"),
                                                            fullPathName,
                                                            fileFilters.join(s_fileFilterSeparator),
                                                            &selectedFilter,
                                                            CCFileDialogOptions());

    if (selectedFilename.isEmpty())
    {
        return;
    }

    if (hasOther)
    {
        ccConsole::Warning("[I/O] The following selected entities won't be saved:");
        for (unsigned i = 0; i < other.getChildrenNumber(); ++i)
        {
            ccConsole::Warning(QString("\t- %1s").arg(other.getChild(i)->getName()));
        }
    }

    CC_FILE_ERROR result = CC_FERR_NO_ERROR;
    FileIOFilter::SaveParameters parameters;
    {
        parameters.alwaysDisplaySaveDialog = true;
        parameters.parentWidget = this;
    }

    //specific case: BIN format
    if (selectedFilter == BinFilter::GetFileFilter())
    {
        if ( haveOneSelection() )
        {
            result = FileIOFilter::SaveToFile(m_selectedEntities.front(), selectedFilename, parameters, selectedFilter);
        }
        else
        {
            //we'll regroup all selected entities in a temporary group
            ccHObject tempContainer;
            ConvertToGroup(m_selectedEntities, tempContainer, ccHObject::DP_NONE);
            if (tempContainer.getChildrenNumber())
            {
                result = FileIOFilter::SaveToFile(&tempContainer, selectedFilename, parameters, selectedFilter);
            }
            else
            {
                ccLog::Warning("[I/O] None of the selected entities can be saved this way...");
                result = CC_FERR_NO_SAVE;
            }
        }
    }
    else if (entitiesToSave.getChildrenNumber() != 0)
    {
        //ignored items
        //if (hasSerializable)
        //{
        //	if (!hasOther)
        //		ccConsole::Warning("[I/O] The following selected entites won't be saved:"); //display this warning only if not already done
        //	for (unsigned i = 0; i < otherSerializable.getChildrenNumber(); ++i)
        //		ccConsole::Warning(QString("\t- %1").arg(otherSerializable.getChild(i)->getName()));
        //}

        result = FileIOFilter::SaveToFile(	entitiesToSave.getChildrenNumber() > 1 ? &entitiesToSave : entitiesToSave.getChild(0),
                                          selectedFilename,parameters,selectedFilter);

        if (result == CC_FERR_NO_ERROR && m_ccRoot)
        {
            m_ccRoot->unselectAllEntities();
        }
    }

    //update default filters
    if (hasCloud)
        settings.setValue(ccPS::SelectedOutputFilterCloud(),selectedFilter);
    if (hasMesh)
        settings.setValue(ccPS::SelectedOutputFilterMesh(), selectedFilter);
    if (hasImages)
        settings.setValue(ccPS::SelectedOutputFilterImage(),selectedFilter);
    if (hasPolylines)
        settings.setValue(ccPS::SelectedOutputFilterPoly(), selectedFilter);

    //we update current file path
    currentPath = QFileInfo(selectedFilename).absolutePath();
    settings.setValue(ccPS::CurrentPath(),currentPath);
    settings.endGroup();
}

void MainWindow::on_actionSORFilter_triggered()
{
    ccHObject::Container selectedEntities = getSelectedEntities();
    if(selectedEntities.size()<1){return;}
    ccHObject *entity = selectedEntities[0];
    bool lockedVertices;
    ccPointCloud* cloud = ccHObjectCaster::ToPointCloud(entity, &lockedVertices);
    if(cloud == 0){
        ccConsole::Warning(QString::fromLocal8Bit("[SOR]为NULL的云尝试申请噪声过滤!" ));
        return;
    }
    static int s_sorFilterKnn = 6;
    static double s_sorFilterNSigma = 1.0;

    ccProgressDialog pDlg(true, this);
    pDlg.setAutoClose(false);

    CCLib::ReferenceCloud* selection =
        CCLib::CloudSamplingTools::sorFilter(cloud,s_sorFilterKnn,s_sorFilterNSigma,nullptr,&pDlg);

    if (selection == 0){return ;}
    if (selection->size() == cloud->size())
    {
        ccLog::Warning(QString::fromLocal8Bit("[SOR]'%1'云图中没有点被删除!").arg(cloud->getName()));
    }else{
        ccPointCloud* cleanCloud = cloud->partialClone(selection);
        if (cleanCloud)
        {
            cleanCloud->setName(cloud->getName() + QString(".clean"));
            cleanCloud->setDisplay(cloud->getDisplay());
            if (cloud->getParent())
                cloud->getParent()->addChild(cleanCloud);
            addToDB(cleanCloud);
            cloud->setEnabled(false);

            ccConsole::Warning("Previously selected entities (sources) have been hidden!");
            m_ccRoot->selectEntity(cleanCloud, true);
        }
        else
        {
            ccConsole::Warning(QString::fromLocal8Bit("[SOR] 没有足够的内存去创建降噪后的云:'%1'!").arg(cloud->getName()));
        }
    }
    delete selection;
    selection = nullptr;
}

void MainWindow::on_actionCpCpDist_triggered()
{
    if (m_selectedEntities.size() < 1)
    {
        ccConsole::Error(QString::fromLocal8Bit("请先加载入一副点云图!"));
        return;
    }
    //ccHObject *firstPc=m_selectedEntities[0];

    //*记得加入-先得判断一下有没有载入标准点云图
    QString fp = QString::fromLocal8Bit("C:\\Users\\Administrator\\Desktop\\cc_file_test\\0.txt");
    ccHObject *secondSrc = LoadFileTo_ccobject(fp);
    if(secondSrc==0){return;}
    ccHObject *secondPc = secondSrc->getChild(0);
    addToDB(secondSrc);
    //------上面这一段记得做判断-----

    //firstPc->setSelected(true);
    m_selectedEntities[0]->setSelected(true);
    secondPc->setSelected(true);

    if (!m_selectedEntities[0]->isKindOf(CC_TYPES::POINT_CLOUD) || !secondPc->isKindOf(CC_TYPES::POINT_CLOUD))
    {
        ccConsole::Error(QString::fromLocal8Bit("选中的两幅点云类型不正确!"));
        return;
    }

    ccHObject* m_firstEnt = 0;
    ccHObject* m_secondEnt = 0;
    //....ccOrderChoiceDlg.h
    ccOrderChoiceDlg dlg(	m_selectedEntities[0], "Compared",
                         /*m_selectedEntities[1]*/secondPc, "Reference",
                         this );
    if (!dlg.exec())
        return;

//    firstPc->setEnabled(true);
//    firstPc->setVisible(true);
//    firstPc->setTempColor(ccColor::red);
//    firstPc->prepareDisplayForRefresh_recursive();
//    secondPc->setEnabled(true);
//    secondPc->setVisible(true);
//    secondPc->setTempColor(ccColor::yellow);
//    secondPc->prepareDisplayForRefresh_recursive();
//    refreshAll();

    ccGenericPointCloud* compCloud = ccHObjectCaster::ToGenericPointCloud(/*firstPc*/dlg.getFirstEntity());
    ccGenericPointCloud* refCloud = ccHObjectCaster::ToGenericPointCloud(/*secondPc*/dlg.getSecondEntity());

    if (m_compDlg)
        delete m_compDlg;

    m_compDlg = new ccComparisonDlg(compCloud, refCloud, ccComparisonDlg::CLOUDCLOUD_DIST, this);
    if (!m_compDlg->initDialog())
    {
        ccConsole::Error("初始化比较设置失败!");
        delete m_compDlg;
        m_compDlg = nullptr;
        return;
    }
    connect(m_compDlg, &QDialog::finished, this, &MainWindow::deactivateComparisonMode);
    m_compDlg->show();
}

void MainWindow::deactivateComparisonMode(int result)
{
    if (m_compDlg && result == QDialog::Accepted && m_ccRoot)
    {
        ccHObject* compEntity = m_compDlg->getComparedEntity();
        if (compEntity)
        {
            m_ccRoot->selectEntity(compEntity);
//            addToDB(compEntity);
//            ccHObject*  curs = m_ccRoot->getRootEntity()->getChild(0);
//            curs->setVisible(false);
//            curs->setSelected(false);
//            curs->setEnabled(false);
            m_ccRoot->getRootEntity()->removeChild(1);
//            refreshAll();
        }
    }
}

short MainWindow::myPointCouldDataShow(QString name,rxsPointCouldp *rpc,unsigned pointNums){
    ccHObject* ceObj = new ccHObject();
    ccPointCloud* loadedCloud = new ccPointCloud(name);
    if (!loadedCloud)
        return 10;//CC_FERR_NOT_ENOUGH_MEMORY;

    //loadedCloud->showColors(true);
    for(unsigned x=0;x< pointNums;x++){
        double *Pf = (double *)&rpc[x];
        loadedCloud->addPoint(CCVector3::fromArray(Pf));
        //        ccColor::Rgb C;
        //        loadedCloud->addColor(C);
    }
    ceObj->addChild(loadedCloud);
    addToDB(ceObj);
    kvpdChr *kvpv = mainFrame->FacParam->KvpRead("自动高程");
    if(kvpv != 0){
        if(kvpv->val != 0){
            if(atoi(kvpv->val)==1){
                on_actionHeightRamp_triggered();
            }
        }
    }
    return 0;
}


short MainWindow::myImageDataShow(QString name,QImage *img){
    ccImage *ccimg = new ccImage(*img);
    ccHObject* ceObj = new ccHObject();
    ceObj->addChild(ccimg);
    addToDB(ceObj);
    refreshAll(true);
    redrawAll(true);
    return 0;
}

short MainWindow::myLabelDataShow(const char *name,rxsPoint pos,unsigned itemIndex){
    cc2DLabel *m_label = new cc2DLabel();
    m_label->setSelected(true);
    m_label->setName(QString::fromLocal8Bit(name));
    m_label->addPickedPoint(static_cast<ccGenericPointCloud*>(getSelectedEntities()[0]), itemIndex, false/*picked.entityCenter*/);
    m_label->setVisible(true);
    m_label->displayPointLegend(m_label->size() == 3);
    m_label->setPosition(static_cast<float>(pos.x) / view3D->glWidth(), static_cast<float>(pos.y) / view3D->glHeight());
    addToDB(m_label);
    return 0;
}

short MainWindow::myDefineLabelDataShow(const char *name,QStringList content,rxsPoint pos,unsigned index){
    gpp2DLabel *m_label = new gpp2DLabel();
    m_label->setSelected(true);
    m_label->setName(QString::fromLocal8Bit(name));
    m_label->bodyContentSet(content);
    m_label->addPickedPoint(static_cast<ccGenericPointCloud*>(getSelectedEntities()[0]), index, false/*picked.entityCenter*/);
    m_label->setVisible(true);
    m_label->setPosition(static_cast<float>(pos.x) / view3D->glWidth(), static_cast<float>(pos.y) / view3D->glHeight());
    addToDB(m_label);
    return 0;
}

short MainWindow::myClipBoxShow(){
    ccHObject::Container selectedEntities = getSelectedEntities();
    if(selectedEntities.size()<1){
        ccLog::Warning(QString::fromLocal8Bit("没有可选中的内容，请先选择!"));
        return -1;
    }
    ccClipBox* m_clipBox = new ccClipBox(QString(), static_cast<unsigned>(ReservedIDs::CLIPPING_BOX));
    m_clipBox->setVisible(true);
    m_clipBox->setEnabled(true);
    m_clipBox->setSelected(true);
    ccHObject *entity = selectedEntities[0];

    ccGLWindow* m_associatedWin = view3D;
    m_associatedWin->addToOwnDB(m_clipBox);
    if (!m_clipBox->addAssociatedEntity(entity))
    {
        ccLog::Error(QString::fromLocal8Bit("发生了一个错误，请查看运行日志!"));
        return false;
    }

    CCVector3 diag = m_clipBox->getBox().getDiagVec();
    QString xWidth = QString::number(diag.x / 100.0);
    QString yWidth = QString::number(diag.y / 100.0);
    QString zWidth = QString::number(diag.z / 100.0);

//    CCVector3 min={0.0f,0.0f,0.0f};
//    CCVector3 max={100.0f,100.0f,100.0f};
//    ccBBox box(min,max);
//    m_clipBox->setBox(box);

    m_clipBox->reset();
    m_associatedWin->setUnclosable(true);
    m_associatedWin->redraw();
    vccb.push_back(m_clipBox);
    return 0;
}

void MainWindow::myClipBoxDestory(){
    vccb[vccb.size()-1]->releaseAssociatedEntities();
    redrawAll();
    refreshAll();
    delete vccb[vccb.size()-1];
    vccb.pop_back();
}

short MainWindow::myBoundBoxShow(){
//      get entity bounding box
//      ccBBox bBox = obj->getBB_recursive();
//      CCVector3 center = bBox.getCenter();

//        CCVector3 min={0.0f,0.0f,0.0f};
//        CCVector3 max={100.0f,100.0f,100.0f};
//        ccBBox *box = new ccBBox(min,max);
//        ccGLWindow* m_associatedWin = view3D;
//        m_associatedWin->addToOwnDB(box);
    return 0;
}

void MainWindow::myRoiAreaSet(ccHObject* obj,CCVector3 maxCorner,CCVector3 minCorner){
    CCVector3 dd = maxCorner - minCorner;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    QByteArray qbaName = obj->getName().toLocal8Bit();
    rxsPointCouldp roip[3]={{maxCorner.x,maxCorner.y,maxCorner.z},
        {minCorner.x,minCorner.y,minCorner.z}, {dd.x,dd.y,dd.z}};
    const char *name =qbaName.data();
    u8 type = 0;
    wrkpd->RoiAreaSet(name,type,roip,0,0);
}

bool MainWindow::myRoiAreaSet(QString name){
    if(vccb.size()<1){
        return false;
    }
    ccBBox  box = vccb[vccb.size()-1]->getBox();
    CCVector3 maxCorner = box.maxCorner();
    CCVector3 minCorner = box.minCorner();
    CCVector3 dd = maxCorner - minCorner;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    rxsPointCouldp roip[3]={{maxCorner.x,maxCorner.y,maxCorner.z},
                              {minCorner.x,minCorner.y,minCorner.z}, {dd.x,dd.y,dd.z}};
    u8 type = 0;
    QByteArray qba = name.toLocal8Bit();
    wrkpd->RoiAreaSet(qba.data(),type,roip,0,0);
    myClipBoxDestory();
//    vccb[vccb.size()-1]->releaseAssociatedEntities();
//    redrawAll();
//    refreshAll();
//    delete vccb[vccb.size()-1];
//    vccb.pop_back();
    return true;
}

void MainWindow::on_action3DPointGet_triggered()
{
    ccGLWindow* win = getActiveGLWindow();
    if (!win)
    {
        return;
    }
    if (m_ccRoot)
    {
        m_ccRoot->unselectAllEntities();
    }
    if (!m_ppDlg)
    {
        m_ppDlg = new ccPointPropertiesDlg(m_pickingHub, this);
        connect(m_ppDlg, &ccOverlayDialog::processFinished,	this, &MainWindow::deactivatePointPickingMode);
        connect(m_ppDlg, &ccPointPropertiesDlg::newLabel,	this, &MainWindow::handleNewLabel);
        repositionOverlayDialog(m_ppDlg, Qt::TopRightCorner);
    }
    m_ppDlg->linkWith(win);

    if (!m_ppDlg->start())
        deactivatePointPickingMode(false);
    else
        updateOverlayDialogsPlacement();

}

void MainWindow::deactivatePointPickingMode(bool state)
{
    //we enable all GL windows
    //enableAll();
    freezeUI(false);
    updateUI();
}

ccHObject *MainWindow::myGetCCObjFromDB(QString pcName)
{
    ccHObject* TopObj = dbRootObject();
    if(TopObj->getParent() == 0){
        unsigned m =0;
        ccHObject* entObj= nullptr;
        ccHObject* cldEnt = nullptr;
        while((entObj= TopObj->getChild(m))!= nullptr){
            unsigned X=0;
            while((cldEnt = entObj->getChild(X))!=nullptr){
                if(cldEnt->getName()==pcName){
                    return cldEnt;
                }
                X+=1;
            }
            m+=1;
        }
    }
    return 0;
}

void MainWindow::cancelPreviousPickingOperation(bool aborted)
{
    if (!s_pickingWindow)
        return;

    switch(s_currentPickingOperation)
    {
    case PICKING_ROTATION_CENTER:
        //nothing to do
        break;
    case PICKING_LEVEL_POINTS:
        if (s_levelMarkersCloud)
        {
            s_pickingWindow->removeFromOwnDB(s_levelMarkersCloud);
            delete s_levelMarkersCloud;
            s_levelMarkersCloud = nullptr;
        }
        break;
    default:
        assert(false);
        break;
    }

    if (aborted)
    {
        s_pickingWindow->displayNewMessage(QString(), ccGLWindow::LOWER_LEFT_MESSAGE); //clear previous messages
        s_pickingWindow->displayNewMessage("Picking operation aborted", ccGLWindow::LOWER_LEFT_MESSAGE);
    }
    s_pickingWindow->redraw(false);
    freezeUI(false);

    m_pickingHub->removeListener(this);

    s_pickingWindow = nullptr;
    s_currentPickingOperation = NO_PICKING_OPERATION;
}

void MainWindow::onItemPicked(const PickedItem& pi)
{
    if (!s_pickingWindow || !m_pickingHub)
    {
        return;
    }

    if (!pi.entity)
    {
        return;
    }

    if (m_pickingHub->activeWindow() != s_pickingWindow)
    {
        ccLog::Warning("The point picked was picked in the wrong window");
        return;
    }

    CCVector3 pickedPoint = pi.P3D;
    switch(s_currentPickingOperation)
    {
    case PICKING_LEVEL_POINTS:
    {
        if (!s_levelMarkersCloud)
        {
            assert(false);
            cancelPreviousPickingOperation(true);
        }

        for (unsigned i = 0; i < s_levelMarkersCloud->size(); ++i)
        {
            const CCVector3* P = s_levelMarkersCloud->getPoint(i);
            if ((pickedPoint - *P).norm() < 1.0e-6)
            {
                ccLog::Warning("[Level] Point is too close from the others!");
                return;
            }
        }

        //add the corresponding marker
        s_levelMarkersCloud->addPoint(pickedPoint);
        unsigned markerCount = s_levelMarkersCloud->size();
        cc2DLabel* label = new cc2DLabel();
        label->addPickedPoint(s_levelMarkersCloud, markerCount - 1);
        label->setName(QString("P#%1").arg(markerCount));
        label->setDisplayedIn2D(false);
        label->setDisplay(s_pickingWindow);
        label->setVisible(true);
        s_levelMarkersCloud->addChild(label);
        s_pickingWindow->redraw();

        if (markerCount == 3)
        {
            //we have enough points!
            const CCVector3* A = s_levelMarkersCloud->getPoint(0);
            const CCVector3* B = s_levelMarkersCloud->getPoint(1);
            const CCVector3* C = s_levelMarkersCloud->getPoint(2);
            CCVector3 X = *B - *A;
            CCVector3 Y = *C - *A;
            CCVector3 Z = X.cross(Y);
            //we choose 'Z' so that it points 'upward' relatively to the camera (assuming the user will be looking from the top)
            CCVector3d viewDir = s_pickingWindow->getCurrentViewDir();
            if (CCVector3d::fromArray(Z.u).dot(viewDir) > 0)
            {
                Z = -Z;
            }
            Y = Z.cross(X);
            X.normalize();
            Y.normalize();
            Z.normalize();

            ccGLMatrixd trans;
            double* mat = trans.data();
            mat[0] = X.x; mat[4] = X.y; mat[8]  = X.z; mat[12] = 0;
            mat[1] = Y.x; mat[5] = Y.y; mat[9]  = Y.z; mat[13] = 0;
            mat[2] = Z.x; mat[6] = Z.y; mat[10] = Z.z; mat[14] = 0;
            mat[3] = 0  ; mat[7] = 0  ; mat[11] = 0  ; mat[15] = 1;

            CCVector3d T = -CCVector3d::fromArray(A->u);
            trans.apply(T);
            T += CCVector3d::fromArray(A->u);
            trans.setTranslation(T);

            assert(haveOneSelection() && m_selectedEntities.front() == s_levelEntity);
            //applyTransformation(trans);

            //clear message
            s_pickingWindow->displayNewMessage(QString(), ccGLWindow::LOWER_LEFT_MESSAGE, false); //clear previous message
            s_pickingWindow->setView(CC_TOP_VIEW);
        }
        else
        {
            //we need more points!
            return;
        }
    }
        //we use the next 'case' entry (PICKING_ROTATION_CENTER) to redefine the rotation center as well!
        assert(s_levelMarkersCloud && s_levelMarkersCloud->size() != 0);
        pickedPoint = *s_levelMarkersCloud->getPoint(0);
        //break;

    case PICKING_ROTATION_CENTER:
    {
        CCVector3d newPivot = CCVector3d::fromArray(pickedPoint.u);
        //specific case: transformation tool is enabled
//        if (m_transTool && m_transTool->started())
//        {
//            m_transTool->setRotationCenter(newPivot);
//            const unsigned& precision = s_pickingWindow->getDisplayParameters().displayedNumPrecision;
//            s_pickingWindow->displayNewMessage(QString(), ccGLWindow::LOWER_LEFT_MESSAGE, false); //clear previous message
//            s_pickingWindow->displayNewMessage(QString("Point (%1 ; %2 ; %3) set as rotation center for interactive transformation")
//                                                   .arg(pickedPoint.x, 0, 'f', precision)
//                                                   .arg(pickedPoint.y, 0, 'f', precision)
//                                                   .arg(pickedPoint.z, 0, 'f', precision),
//                                               ccGLWindow::LOWER_LEFT_MESSAGE, true);
//        }
//        else
//        {
//            const ccViewportParameters& params = s_pickingWindow->getViewportParameters();
//            if (!params.perspectiveView || params.objectCenteredView)
//            {
//                //apply current GL transformation (if any)
//                pi.entity->getGLTransformation().apply(newPivot);
//                s_pickingWindow->setPivotPoint(newPivot, true, true);
//            }
//        }
        //s_pickingWindow->redraw(); //already called by 'cancelPreviousPickingOperation' (see below)
    }
    break;

    default:
        assert(false);
        break;
    }

    cancelPreviousPickingOperation(false);
}

MainWindow::ccHObjectContext MainWindow::removeObjectTemporarilyFromDBTree(ccHObject* obj)
{
    ccHObjectContext context;

    assert(obj);
    //    if (!m_ccRoot || !obj)
    //        return context;

    //    //mandatory (to call putObjectBackIntoDBTree)
    //    context.parent = obj->getParent();

    //    //remove the object's dependency to its father (in case it undergoes "severe" modifications)
    //    if (context.parent)
    //    {
    //        context.parentFlags = context.parent->getDependencyFlagsWith(obj);
    //        context.childFlags = obj->getDependencyFlagsWith(context.parent);

    //        context.parent->removeDependencyWith(obj);
    //        obj->removeDependencyWith(context.parent);
    //    }

    //    m_ccRoot->removeElement(obj);

    return context;
}

void MainWindow::putObjectBackIntoDBTree(ccHObject* obj, const ccHObjectContext& context)
{
    //    assert(obj);
    //    if (!obj || !m_ccRoot)
    //        return;

    //    if (context.parent)
    //    {
    //        context.parent->addChild(obj,context.parentFlags);
    //        obj->addDependency(context.parent,context.childFlags);
    //    }

    //    //DGM: we must call 'notifyGeometryUpdate' as any call to this method
    //    //while the object was temporarily 'cut' from the DB tree were
    //    //ineffective!
    //    obj->notifyGeometryUpdate();

    //    m_ccRoot->addElement(obj,false);
}


void MainWindow::registerOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos)
{
}

void MainWindow::unregisterOverlayDialog(ccOverlayDialog* dialog)
{
}

void MainWindow::updateOverlayDialogsPlacement()
{

}
void MainWindow::createGLWindow(ccGLWindow*& window, QWidget*& widget) const
{
}

void MainWindow::destroyGLWindow(ccGLWindow*) const
{

}

void MainWindow::updateUI()
{
    if(rerp != 0){
        rerp->rrRepListSelectedSet();
    }
    char *wppp = mainFrame->WrkProparam->CurFilePath();
    if(wppp !=0){
        QString p = QString::fromLocal8Bit(wppp);
        if(p.length()>0){
            msgText->setText(p);
            FillWrkPointGrps();
            ui->RecordList->addItem(QString::fromLocal8Bit("扫描路径点位信息获取成功"));

            Kvpdchr Val;
            mainFrame->WrkProparam->Selected(2);
            mainFrame->WrkProparam->DataReaderKvpd(2,Val);
            QString sv = QString::fromLocal8Bit(Val.val);
            this->lsrScaner->FillImageableNames(this->ui->tskSensorSelected);
            int i= ui->tskSensorSelected->findText(sv);
            ui->tskSensorSelected->setCurrentIndex(i);
            return;
        }
    }else{
        msgText->setText(QString::fromLocal8Bit("当前未能加载加工程式！"));
    }
}

void MainWindow::freezeUI(bool state)
{
}

short MainWindow::myComputerFlowTester(const char* cname){
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    SelectedEntitiesSet();
    if(m_selectedEntities.size()<1){return -1;}
    ccHObject* ceObj =m_selectedEntities[0];
    if(!ceObj->isKindOf(CC_TYPES::POINT_CLOUD)){
        return -2;
    }
    bool isLooled= false;
    //ccPointCloud* cpc = ccHObjectCaster::ToPointCloud(ceObj,&isLooled);
    ccGenericPointCloud *cgpc = ccHObjectCaster::ToGenericPointCloud(ceObj,&isLooled);
    if(cgpc ==0){return -3;}
    unsigned int psize =  cgpc->size();
    if(psize<1){return -4;}
//    rxsPointCouldp *pdent = new rxsPointCouldp[psize];

//    for(unsigned m=0;m<psize;m++){
//        const CCVector3 *pgs = cgpc->getPoint(m);
//        pdent[m].x = pgs->x;
//        pdent[m].y = pgs->y;
//        pdent[m].z = pgs->z;
//    }
//    if(wrkpd->CompTranTester(cname,pdent,psize)!=0){
//        ui->RecordList->addItem(QString::fromLocal8Bit("请确认当前计算流程配置已经正确!"));
//    }
//    delete[] pdent;
//    pdent =0;
    return 0;
}

rxsImg MainWindow::myImgGetFromCCWnd(){
    rxsImg ri ={0,0,0,0,0};
        SelectedEntitiesSet();
        if(m_selectedEntities.size()<1){
            ccConsole::Error(QString::fromLocal8Bit("请先加载资源!"));
            return ri;
        }
        ccHObject* ceObj =m_selectedEntities[0];
        if(!ceObj->isKindOf(CC_TYPES::IMAGE)){
            ccConsole::Error(QString::fromLocal8Bit("本测试功能需要加载图片处理!"));
            return ri;
        }
        ccImage *ccimg = ccHObjectCaster::ToImage(ceObj);
        QImage image = ccimg->data();
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "JPG");
        buffer.close();
        unsigned char *data = reinterpret_cast<unsigned char *>(bytes.data());
        ri.Data =(void*) image.constBits();
        ri.cols = image.width();
        ri.rows = image.height();
        ri.Len = image.sizeInBytes();
        ri.Channels = ri.Len/(ri.cols * ri.rows);
        return ri;
}

void MainWindow::on_AxDioOpter_triggered()
{
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::TECH){
        ui->RecordList->addItem(QString::fromLocal8Bit("当前用户无权限，请登入!"));
        on_actionUserLogin_triggered();
        if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::TECH)
            return;
    }
    if(modoOpter == 0){
        modoOpter = new MotionOpter(this);
    }
    modoOpter->setWindowFlags(Qt::WindowCloseButtonHint|Qt::Dialog);
    modoOpter->show();
}


void MainWindow::disableButtonFunction(UserManger::USER_AUTH ua){
    if(mainFrame->userManger->CurUser.Auth < UserManger::USER_AUTH::TECH)
    {

    }else{

    }
    if(ua == UserManger::USER_AUTH::TECH){
        ui->action->setEnabled(false);
        //ui->action3D->setEnabled(true);
        ui->AxDioOpter->setEnabled(true);
        ui->actionUserManger->setEnabled(false);
        ui->actionWrkPause->setEnabled(true);
        ui->actionAssociationCfg->setEnabled(false);
        ui->actionLCC->setEnabled(false);
        ui->actionRoiSelected->setEnabled(false);
        ui->actionPCSave->setEnabled(false);
        ui->actionSORFilter->setEnabled(false);
        ui->actionCpCpDist->setEnabled(false);
        ui->actionEntMange->setEnabled(false);
        ui->actionVisualFlow->setEnabled(false);
        ui->actionCCfile->setEnabled(false);
        ui->moveToMp->setEnabled(true);
        ui->moveToMpEnd->setEnabled(true);
        ui->pickMp->setEnabled(true);
        ui->pickEndMp->setEnabled(true);
        ui->DelRowMp->setEnabled(true);
        ui->AddRowMp->setEnabled(true);
    }else if(ua == UserManger::USER_AUTH::ADMIN){
        ui->action->setEnabled(true);
        //ui->action3D->setEnabled(true);
        ui->AxDioOpter->setEnabled(true);
        ui->actionUserManger->setEnabled(true);
        ui->actionWrkPause->setEnabled(true);
        ui->actionAssociationCfg->setEnabled(true);
        ui->actionLCC->setEnabled(true);
        ui->actionRoiSelected->setEnabled(true);
        ui->actionPCSave->setEnabled(true);
        ui->actionSORFilter->setEnabled(true);
        ui->actionCpCpDist->setEnabled(true);
        ui->actionEntMange->setEnabled(true);
        ui->actionVisualFlow->setEnabled(true);
        ui->actionCCfile->setEnabled(true);
        ui->moveToMp->setEnabled(true);
        ui->moveToMpEnd->setEnabled(true);
        ui->pickMp->setEnabled(true);
        ui->pickEndMp->setEnabled(true);
        ui->DelRowMp->setEnabled(true);
        ui->AddRowMp->setEnabled(true);
    }else{
        ui->action->setEnabled(false);
        //ui->action3D->setEnabled(false);
        ui->AxDioOpter->setEnabled(false);
        ui->actionUserManger->setEnabled(false);
        ui->actionWrkPause->setEnabled(false);
        ui->actionAssociationCfg->setEnabled(false);
        ui->actionLCC->setEnabled(false);
        ui->actionRoiSelected->setEnabled(false);
        ui->actionPCSave->setEnabled(false);
        ui->actionSORFilter->setEnabled(false);
        ui->actionCpCpDist->setEnabled(false);
        ui->actionEntMange->setEnabled(false);
        ui->actionVisualFlow->setEnabled(false);
        ui->actionCCfile->setEnabled(false);
        ui->moveToMp->setEnabled(false);
        ui->moveToMpEnd->setEnabled(false);
        ui->pickMp->setEnabled(false);
        ui->pickEndMp->setEnabled(false);
        ui->DelRowMp->setEnabled(false);
        ui->AddRowMp->setEnabled(false);
    }
}

void MainWindow::on_actionRunPicTester_triggered()
{
    WorkProcessDal * wrkpd = WorkProcessDalGet();
    wrkpd->twoDGetDataFromPic();
}


void MainWindow::on_actionCut_triggered()
{
    if(!SelectedEntitiesSet()){return ;}
    ccGLWindow* win = getActiveGLWindow();
    if (!win) {return;}
    if (!m_gsTool)
    {
        m_gsTool = new ccGraphicalSegmentationTool(this);
        connect(m_gsTool, &ccOverlayDialog::processFinished, this, &MainWindow::deactivateSegmentationMode);
        connect(m_gsTool, &ccGraphicalSegmentationTool::roiGetProcess, this, &MainWindow::SegmentationModeRoipro);
        registerOverlayDialog(m_gsTool, Qt::TopRightCorner);
    }
    m_gsTool->linkWith(win);
    for ( ccHObject *entity : getSelectedEntities() )
    {
        m_gsTool->addEntity(entity);
    }
    if (m_gsTool->getNumberOfValidEntities() == 0)
    {
        ccConsole::Error(QString::fromLocal8Bit("当前激活窗口中没有可分段裁剪的点云实体!"));
        return;
    }
    freezeUI(true);
    if (m_gsTool->start())
    {
        repositionOverlayDialog(m_gsTool, Qt::TopRightCorner);
    }else{
        deactivateSegmentationMode(false);
    }
}

void MainWindow::SegmentationModeRoipro(rxsPointCouldp *roiCoorV,int vNum,rxsPointCouldp lwh,int t,QString roiName){
    WorkProcessDal * wrkpd = WorkProcessDalGet();
    if(t==0){
        int cr = ui->MpsDT->currentRow();
        if(cr<0){
            ccConsole::Error(QString::fromLocal8Bit("请先选中对应路径后重新点击本按钮进行一键同步!"));
            return;
        }
        QString curSelFlower = ui->MpsDT->item(cr,11)->text();
        QByteArray qba = curSelFlower.toLocal8Bit();
        wrkpd->SynchronousRoiToFlower(qba.data());
    }
    if(t == 1){
        for(int x=0;x<vNum;x++){
            ccConsole::Print(QString::fromLocal8Bit("选框视图坐标(%1,%2,%3)!")
                                 .arg(roiCoorV[x].x).arg(roiCoorV[x].y).arg(roiCoorV[x].z));
        }
        return;
    }
    QSet<ccHObject*>& segmentedEntities = m_gsTool->entities();
    int x = segmentedEntities.size();
    ccConsole::Print(QString::fromLocal8Bit("点云[%1]选取段数量:%2").arg(roiName).arg(x));
    rxsPointCouldp rp[3]={{0},{0},{0}};
    for (QSet<ccHObject*>::iterator p = segmentedEntities.begin(); p != segmentedEntities.end(); p++){
        ccHObject* entity = (*p);
        if (entity->isKindOf(CC_TYPES::POINT_CLOUD) || entity->isKindOf(CC_TYPES::MESH))
        {
            ccGenericPointCloud* genCloud = ccHObjectCaster::ToGenericPointCloud(entity);
            if(!genCloud){continue;}
            ccGenericPointCloud::VisibilityTableType& visibilityArray = genCloud->getTheVisibilityArray();
            if(visibilityArray.empty()){continue;}
            int couldSize = genCloud->size();
            std::vector<rxsPointCouldp> tageSeg;
            int index=0;
            for(int i=0;i<couldSize;i++){
                if(visibilityArray[i] == POINT_HIDDEN){
                    const CCVector3 *pp = genCloud->getPoint(i);
                    tageSeg.push_back({pp->x,pp->y,pp->z});
                }else{
                    if(tageSeg.size()>0){
                        //roiName = QString::fromLocal8Bit(genCloud->getName().toLocal8Bit())+ QString::number(index);
                        QString nRoin=roiName+ QString::number(index);
                        QByteArray qba = nRoin.toLocal8Bit();
                        const char *name = qba.data();
                        if(t == 2){
                            ccConsole::Print(QString::fromLocal8Bit("框选点云[%1]-%2个点!").arg(nRoin).arg(tageSeg.size()));
                            rp[0] = tageSeg[0];
                            rp[1] = tageSeg[tageSeg.size()-1];
                            rp[2] = lwh;
                            //wrkpd->RoiAreaSet(name,8,rp,&tageSeg[0], tageSeg.size());
                            wrkpd->RoiAreaSet(name,4,rp,0,0);
                        }else if(t == 3 || t == 4){
                            //左右点记录
                            int rsi = i-tageSeg.size();
                            rp[0] = tageSeg[0];
                            rp[1] = tageSeg[tageSeg.size()-1];
                            rp[2] = lwh;
                            wrkpd->RoiAreaSet(name,4,rp,0,0);
                            ccConsole::Print(
                                QString::fromLocal8Bit("框选点云[%1]-%2个点!-%3\n-首点(%4,%5,%6)\n-末端(%7,%8,%9)!")
                                    .arg(nRoin).arg(tageSeg.size()).arg(rsi).arg(tageSeg.size())
                                    .arg(rp[0].x).arg(rp[0].y).arg(rp[0].z).arg(rp[1].x).arg(rp[1].y).arg(rp[1].z));
                            CloudEntity ce={&tageSeg[0],static_cast<unsigned long>(tageSeg.size())};
                            rxsPointCouldp outp;
                            if(!wrkpd->roiPcFindPoint((t==3)?'L':'R',ce,outp)){
                                ccConsole::Print(QString::fromLocal8Bit("[%1]寻找点执行失败!").arg(nRoin));
                            }else
                            {
                                int fi = rsi + wrkpd->indexFindFrom(ce,outp);
                                //rxsPoint p1={static_cast<float>(roiCoorV[0].x),static_cast<float>(roiCoorV[0].y)};
                                QStringList bstr;
                                bstr.append(QString::fromLocal8Bit("X:%1").arg(outp.x));
                                bstr.append(QString::fromLocal8Bit("Y:%1").arg(outp.y));
                                bstr.append(QString::fromLocal8Bit("Z:%1").arg(outp.z));

                                rxsPoint cpc = {
                                    static_cast<float>(roiCoorV[0].x + viewWidget->width()/2),
                                    static_cast<float>(viewWidget->height()/2 - roiCoorV[0].y)};

                                if(t==3){
                                    myDefineLabelDataShow("左点",bstr,cpc,fi);
                                }else{
                                    myDefineLabelDataShow("右点",bstr,cpc,fi);
                                }
                            }
                        }
                        else{
                        }
                        tageSeg.clear();
                        index+=1;
                    }
                }
            }
            genCloud->resetVisibilityArray();
        }
    }
}

void MainWindow::deactivateSegmentationMode(bool state)
{
    bool deleteHiddenParts = false;
    if (state)
    {
        ccHObject* firstResult = nullptr;
        deleteHiddenParts = m_gsTool->deleteHiddenParts();

        //aditional vertices of which visibility array should be manually reset
        std::unordered_set<ccGenericPointCloud*> verticesToReset;

        QSet<ccHObject*>& segmentedEntities = m_gsTool->entities();
        for (QSet<ccHObject*>::iterator p = segmentedEntities.begin(); p != segmentedEntities.end(); )
        {
            ccHObject* entity = (*p);

            if (entity->isKindOf(CC_TYPES::POINT_CLOUD) || entity->isKindOf(CC_TYPES::MESH))
            {
                //first, do the things that must absolutely be done BEFORE removing the entity from DB (even temporarily)
                //bool lockedVertices;
                ccPointCloud* cloud = ccHObjectCaster::ToPointCloud(entity/*,&lockedVertices*/);
                assert(cloud);
                if (cloud)
                {
                    //assert(!lockedVertices); //in some cases we accept to segment meshes with locked vertices!

                    //specific case: labels (do this before temporarily removing 'entity' from DB!)
                    ccHObject::Container labels;
                    if (m_ccRoot)
                    {
                        m_ccRoot->getRootEntity()->filterChildren(labels,true,CC_TYPES::LABEL_2D);
                    }
                    for (ccHObject::Container::iterator it = labels.begin(); it != labels.end(); ++it)
                    {
                        if ((*it)->isA(CC_TYPES::LABEL_2D)) //Warning: cc2DViewportLabel is also a kind of 'CC_TYPES::LABEL_2D'!
                        {
                            //we must search for all dependent labels and remove them!!!
                            //TODO: couldn't we be more clever and update the label instead?
                            cc2DLabel* label = static_cast<cc2DLabel*>(*it);
                            bool removeLabel = false;
                            for (unsigned i = 0; i < label->size(); ++i)
                            {
                                if (label->getPickedPoint(i).entity() == entity)
                                {
                                    removeLabel = true;
                                    break;
                                }
                            }

                            if (removeLabel && label->getParent())
                            {
                                ccLog::Warning(QString("[Segmentation] Label %1 depends on cloud %2 and will be removed").arg(label->getName(), cloud->getName()));
                                ccHObject* labelParent = label->getParent();
                                ccHObjectContext objContext = removeObjectTemporarilyFromDBTree(labelParent);
                                labelParent->removeChild(label);
                                label = nullptr;
                                putObjectBackIntoDBTree(labelParent,objContext);
                            }
                        }
                    } //for each label
                } // if (cloud)

                //we temporarily detach the entity, as it may undergo
                //"severe" modifications (octree deletion, etc.) --> see ccPointCloud::createNewCloudFromVisibilitySelection
                ccHObjectContext objContext = removeObjectTemporarilyFromDBTree(entity);

                //apply segmentation
                ccHObject* segmentationResult = nullptr;
                bool deleteOriginalEntity = deleteHiddenParts;
                if (entity->isKindOf(CC_TYPES::POINT_CLOUD))
                {
                    ccGenericPointCloud* genCloud = ccHObjectCaster::ToGenericPointCloud(entity);
                    ccGenericPointCloud* segmentedCloud = genCloud->createNewCloudFromVisibilitySelection(!deleteHiddenParts);
                    if (segmentedCloud && segmentedCloud->size() == 0)
                    {
                        delete segmentationResult;
                        segmentationResult = nullptr;
                    }
                    else
                    {
                        segmentationResult = segmentedCloud;
                    }

                    deleteOriginalEntity |= (genCloud->size() == 0);
                }
                else if (entity->isKindOf(CC_TYPES::MESH)/*|| entity->isA(CC_TYPES::PRIMITIVE)*/) //TODO
                {
                    if (entity->isA(CC_TYPES::MESH))
                    {
                        segmentationResult = ccHObjectCaster::ToMesh(entity)->createNewMeshFromSelection(!deleteHiddenParts);
                    }
                    else if (entity->isA(CC_TYPES::SUB_MESH))
                    {
                        segmentationResult = ccHObjectCaster::ToSubMesh(entity)->createNewSubMeshFromSelection(!deleteHiddenParts);
                    }

                    deleteOriginalEntity |=  (ccHObjectCaster::ToGenericMesh(entity)->size() == 0);
                }

                if (segmentationResult)
                {
                    assert(cloud);
                    if (cloud)
                    {
                        //another specific case: sensors (on clouds)
                        for (unsigned i = 0; i < entity->getChildrenNumber(); ++i)
                        {
                            ccHObject* child = entity->getChild(i);
                            assert(child);
                            if (child && child->isKindOf(CC_TYPES::SENSOR))
                            {
                                if (child->isA(CC_TYPES::GBL_SENSOR))
                                {
                                    ccGBLSensor* sensor = ccHObjectCaster::ToGBLSensor(entity->getChild(i));
                                    //remove the associated depth buffer of the original sensor (derpecated)
                                    sensor->clearDepthBuffer();
                                    if (deleteOriginalEntity)
                                    {
                                        //either transfer
                                        entity->transferChild(sensor,*segmentationResult);
                                    }
                                    else
                                    {
                                        //or copy
                                        segmentationResult->addChild(new ccGBLSensor(*sensor));
                                    }
                                }
                                else if (child->isA(CC_TYPES::CAMERA_SENSOR))
                                {
                                    ccCameraSensor* sensor = ccHObjectCaster::ToCameraSensor(entity->getChild(i));
                                    if (deleteOriginalEntity)
                                    {
                                        //either transfer
                                        entity->transferChild(sensor,*segmentationResult);
                                    }
                                    else
                                    {
                                        //or copy
                                        segmentationResult->addChild(new ccCameraSensor(*sensor));
                                    }
                                }
                                else
                                {
                                    //unhandled sensor?!
                                    assert(false);
                                }
                            }
                        } //for each child
                    }

                    //we must take care of the remaining part
                    if (!deleteHiddenParts)
                    {
                        //no need to put back the entity in DB if we delete it afterwards!
                        if (!deleteOriginalEntity)
                        {
                            entity->setName(entity->getName() + QString(".remaining"));
                            putObjectBackIntoDBTree(entity, objContext);
                        }
                    }
                    else
                    {
                        //keep original name(s)
                        segmentationResult->setName(entity->getName());
                        if (entity->isKindOf(CC_TYPES::MESH) && segmentationResult->isKindOf(CC_TYPES::MESH))
                        {
                            ccGenericMesh* meshEntity = ccHObjectCaster::ToGenericMesh(entity);
                            ccHObjectCaster::ToGenericMesh(segmentationResult)->getAssociatedCloud()->setName(meshEntity->getAssociatedCloud()->getName());

                            //specific case: if the sub mesh is deleted afterwards (see below)
                            //then its associated vertices won't be 'reset' by the segmentation tool!
                            if (deleteHiddenParts && meshEntity->isA(CC_TYPES::SUB_MESH))
                            {
                                verticesToReset.insert(meshEntity->getAssociatedCloud());
                            }
                        }
                        assert(deleteOriginalEntity);
                        //deleteOriginalEntity = true;
                    }

                    if (segmentationResult->isA(CC_TYPES::SUB_MESH))
                    {
                        //for sub-meshes, we have no choice but to use its parent mesh!
                        objContext.parent = static_cast<ccSubMesh*>(segmentationResult)->getAssociatedMesh();
                    }
                    else
                    {
                        //otherwise we look for first non-mesh or non-cloud parent
                        while (objContext.parent && (objContext.parent->isKindOf(CC_TYPES::MESH) || objContext.parent->isKindOf(CC_TYPES::POINT_CLOUD)))
                        {
                            objContext.parent = objContext.parent->getParent();
                        }
                    }

                    if (objContext.parent)
                    {
                        objContext.parent->addChild(segmentationResult); //FiXME: objContext.parentFlags?
                    }

                    segmentationResult->setDisplay_recursive(entity->getDisplay());
                    segmentationResult->prepareDisplayForRefresh_recursive();

                    addToDB(segmentationResult);

                    if (!firstResult)
                    {
                        firstResult = segmentationResult;
                    }
                }
                else if (!deleteOriginalEntity)
                {
                    //ccConsole::Error("An error occurred! (not enough memory?)");
                    putObjectBackIntoDBTree(entity,objContext);
                }

                if (deleteOriginalEntity)
                {
                    p = segmentedEntities.erase(p);

                    delete entity;
                    entity = nullptr;
                }
                else
                {
                    ++p;
                }
            }
        }

        for ( ccGenericPointCloud *cloud : verticesToReset )
        {
            cloud->resetVisibilityArray();
        }
        if (firstResult && m_ccRoot)
        {
            m_ccRoot->selectEntity(firstResult);
        }
    }

    if (m_gsTool)
    {
        m_gsTool->removeAllEntities(!deleteHiddenParts);
    }
    freezeUI(false);
    updateUI();
    ccGLWindow* win = getActiveGLWindow();
    if (win)
    {
        win->redraw();
    }
}

void MainWindow::on_actionVisualFlow_triggered()
{
    //static FlowChart* _flowchart_window = new FlowChart(this);
    if(_flowchart_windows == nullptr){
        _flowchart_windows = new FlowChart(this);
    }
    _flowchart_windows->setGeometry(100, 100, 800, 500);
    _flowchart_windows->show();
}


void MainWindow::on_actionLocComm_triggered()
{
    if(ComTester == 0){
        ComTester = new commTestDialog(this);
    }
    ComTester->show();
}


//void MainWindow::wheelEvent(QWheelEvent *event)
//{
//    if(isCtrKeyPress){
//        if(mainFrame->sysStatus->CurFixStatus()!= SystemInfo::RUNING){
//            WorkProcessDal * wrkpd = WorkProcessDalGet();
//            AdjustFocal *adjF = (AdjustFocal *)wrkpd->HrdWareObjectGet(0);
//            short v = adjF->CurVal();
//            int dv = event->delta();
//            if(dv > 0){
//                // 当滚轮远离使用者时
//                // 进行放大
//                v += dv;
//                adjF->Set(v);
//            }else{
//                // 当滚轮向使用者方向旋转时
//                // 进行缩小
//                v -= dv;
//                adjF->Set(v);
//            }
//            ccConsole::Print(QString::fromLocal8Bit("快捷键调焦%1").arg(dv));
//        }
//    }
//    QMainWindow::wheelEvent(event);
//}

//void MainWindow::keyReleaseEvent(QKeyEvent *event){
//    if(mainFrame->sysStatus->CurFixStatus()!= SystemInfo::RUNING){
//        Qt::KeyboardModifiers kb_Mod = event->modifiers();
//        int k = event->key();
//        if((k == Qt::Key_Left) || (k == Qt::Key_Right) || (k == Qt::Key_Up) || (k == Qt::Key_Down)
//            ||(k == Qt::Key_PageUp) ||(k == Qt::Key_PageDown) ){
//            ismyKeyPress = false;
//        }
//        if(Qt::ControlModifier == kb_Mod){
//            isCtrKeyPress = false;
//        }
//        if(!ismyKeyPress  || !isCtrKeyPress){
//            rxsMainFrame *psys = rxsMainFrameGet();
//            psys->AxStop(0,true);
//        }
//    }
//    QMainWindow::keyReleaseEvent(event);
//}

//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    if(mainFrame->sysStatus->CurFixStatus()!= SystemInfo::RUNING){
//        Qt::KeyboardModifiers kb_Mod = event->modifiers();
//        if(Qt::ControlModifier==kb_Mod){
//            isCtrKeyPress = true;
//            rxsMainFrame *psys = rxsMainFrameGet();
//            switch (event->key()) {
//            case Qt::Key_Left:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-X",'+',true);
//                break;
//            case Qt::Key_Right:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-X",'-',true);
//                break;
//            case Qt::Key_Up:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-Y",'+',true);
//                break;
//            case Qt::Key_Down:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-Y",'-',true);
//                break;
//            case Qt::Key_PageUp:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-Z",'-',true);
//                ccConsole::Print(QString::fromLocal8Bit("快捷键Z上"));
//                break;
//            case Qt::Key_PageDown:
//                ismyKeyPress = true;
//                psys->AxMove("AOI-Z",'+',true);
//                ccConsole::Print(QString::fromLocal8Bit("快捷键Z下"));
//                break;

//            default:
//                break;
//            }
//        }
//    }
//    QMainWindow::keyPressEvent(event);
//}



void MainWindow::on_pointOffset_clicked()
{
    QStringList xyz = ui->pointCoord->text().split(',');
    if(xyz.length()>2){
        rxsPointCouldp offset={xyz[0].toDouble(),xyz[1].toDouble(),xyz[2].toDouble()};
        mainFrame->WrkProparam->PointOffsetSet(offset);
    }else{
        ccConsole::Warning("点位偏置格式输入错误=> x,y,z => 0,0,0");
    }
}


void MainWindow::on_modelTech_clicked()
{
    int CurSelectedRowIndex = ui->MpsDT->currentRow();
    if(CurSelectedRowIndex <0){
        ccConsole::Warning(QString::fromLocal8Bit("请先在加工路径中选中要操作的对应行!"));
        return;
    }
    QString fln = ui->MpsDT->item(CurSelectedRowIndex,11)->text();
    int id = ui->MpsDT->item(CurSelectedRowIndex,0)->text().toInt();
    int dir =ui->MpsDT->item(CurSelectedRowIndex,2)->text().toInt();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->PointCould_DeformTecher(0,id,dir,fln.toLocal8Bit().data());
}


void MainWindow::on_pushButton_clicked()
{
    int CurSelectedRowIndex = ui->MpsDT->currentRow();
    if(CurSelectedRowIndex <0){
        ccConsole::Warning(QString::fromLocal8Bit("请先在加工路径中选中要操作的对应行!"));
        return;
    }
    QString fln = ui->MpsDT->item(CurSelectedRowIndex,11)->text();
    int id = ui->MpsDT->item(CurSelectedRowIndex,0)->text().toInt();
    int dir =ui->MpsDT->item(CurSelectedRowIndex,2)->text().toInt();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->PointCould_DeformTecher(1,id,dir,fln.toLocal8Bit().data());
}


void MainWindow::on_VisualRunaction_triggered()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->virtualRunTester();
}

