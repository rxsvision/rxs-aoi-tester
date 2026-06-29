#include "flowchart.h"
#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "Form.h"
#include <QtWidgets>
#include <iostream>
#include "ccConsole.h"



int FlowChart::BackMessageProcess(void* pc,int code,void *self){
    FlowChart *Self = (FlowChart *)self;
    switch(code){
    case CBC_IMGSHOW://结果图片显示
    {
        rxsImg *in = (rxsImg *)pc;
        Self->photoLabel->setPhoto(QString::fromLocal8Bit("result"),in);
    }
    break;

    case CBC_VISUAL_ERR:
    {
        int errID = (*(int *)pc);
        Self->SenceDitemColorUpdate(errID);
    }
    break;

    case CBC_VISUAL_UPDSSR:
    {
        Form *src = (Form*) Self->blkAttributePage->widget();
        if(src != 0){
            int SID = (*(int *)pc);
            src->ResultRefush(SID);
        }
    }
    break;

    default:
        MainWindow::BackMessageProcess(pc,code,((FlowChart*)self)->mw);
        break;
    }
    std::cout << "BackMessageProcess ok!" << std::endl;
    return 0;
}


FlowChart::FlowChart(MainWindow * parent)
{
    createActions();
    createToolBox();
    createMenus();
    mw = parent;
    sysLoadIndex = -1;
    itfp = 0;
    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 1000, 1000));
    connect(scene, &DiagramScene::itemInserted,
            this, &FlowChart::itemInserted);
    connect(scene, &DiagramScene::itemSelected,
            this, &FlowChart::itemSelected);
    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    layout->setStretch(0, 10);
    setCentralWidget(widget);
    setWindowTitle(QString::fromLocal8Bit("可视化操作"));
    setUnifiedTitleAndToolBarOnMac(true);

    blkAttributePage = new QDockWidget(QString::fromLocal8Bit("属性页"),this);
    blkResultImgShow = new QDockWidget(QString::fromLocal8Bit("结果图"),this);
    addDockWidget(Qt::RightDockWidgetArea ,blkAttributePage);
    addDockWidget(Qt::RightDockWidgetArea,blkResultImgShow);
    QList<QDockWidget*> temp_docklist;
    temp_docklist << blkAttributePage<<blkResultImgShow;
    QList<int> temp_sizelist;
    temp_sizelist<< static_cast<int>(this->geometry().width() * 0.5);
    resizeDocks(temp_docklist,temp_sizelist,Qt::Horizontal);

    photoLabel = new PhotoLabel(blkResultImgShow);
    blkResultImgShow->setWidget(photoLabel);

    blkAttributePage->show();
    blkResultImgShow->show();

    Roitoolf = new roiToolForm(blkResultImgShow);
    Roitoolf->hide();
    mfPtr = rxsMainFrameGet();
    mfPtr->FontMessageProcessCB(BackMessageProcess,this);
}

FlowChart::~FlowChart(){
    delete blkAttributePage;
    blkAttributePage = 0;
}

void FlowChart::SenceDitemUpdate(QString oldName,QString newName){
    if(scene ==0){return ;}
    QList<QGraphicsItem *> qlqis = scene->selectedItems();
    foreach (QGraphicsItem *i, qlqis) {
        if(((DiagramItem *)i)->Name() == oldName){
            ((DiagramItem *)i)->NameSet(newName);
            break;
        }
    }
    scene->update();
    view->update();
}

void FlowChart::SenceDitemColorUpdate(int ID){
    if(scene ==0){return ;}
    if(itfp==0){return ;}
    StepBlockBase *sbb = (*itfp)[ID];
    QList<QGraphicsItem *> alli = scene->items();
    if(alli.empty()){return;}
    if(ID>0){
        const char *tName = sbb->Name();
        QString cmpName = QString::fromLocal8Bit(tName);
        foreach (QGraphicsItem *i, alli) {
            if(((DiagramItem *)i)->Name() == cmpName && ID== sbb->ID()){
                ((DiagramItem *)i)->setTextColor(Qt::red);
                break;
            }
        }
    }else{
        foreach (QGraphicsItem *i, alli) {
            ((DiagramItem *)i)->setTextColor(Qt::black);
        }
    }
    scene->update();
    view->update();
}


void FlowChart::RefushResetSence(){
    itfp = 0;
    clrIfwer();
}

void FlowChart::runTester(){
    photoLabel->clearShow();
    if(itfp != 0){
        //SenceDitemColorUpdate(-1);
        itfp->RunAnsy();
        //itfp->Run();
    }
    std::cout << "runTester ok!" << std::endl;
}

void FlowChart::newCreater(){
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    if(wrkpd->rxsImgTPC == 0){
        return;
    }
    int newNo = wrkpd->rxsImgTPC->Count()+1;
    QString nnn = QString::fromLocal8Bit("new_%1").arg(newNo);
    QByteArray qbannnn=  nnn.toLocal8Bit();
    wrkpd->rxsImgTPC->TaskFlowerProcessPush(qbannnn.data());
    itfp = (*wrkpd->rxsImgTPC)[newNo-1];
    if(itfp != 0){
        clrIfwer();
    }
}

void FlowChart::clrIfwer(){
    photoLabel->clearShow();
    if(itfp){
        itfp->Clear();
    }
    scene->clear();
    view->update();
}

void FlowChart::backgroundButtonGroupClicked(QAbstractButton *button)
{
    const QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background4.png"));

    scene->update();
    view->update();
}

void FlowChart::buttonGroupClicked(QAbstractButton *button)
{
    if(itfp==0){
        ccConsole::Print(QString::fromLocal8Bit("请先进行流程创建!"));
        return;
    }
    const QList<QAbstractButton *> buttons = buttonGroup->buttons();
    for (QAbstractButton *myButton : buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    const int id = buttonGroup->id(button);
    scene->setItemType(DiagramItem::DiagramType(id));
    scene->setMode(DiagramScene::InsertItem);
    QString nt = button->text();
    QStringList nn= nt.split('_');
    scene->setItemInfo(nn[0],(StepBlockBase::SBTYPE)nn[1].toInt(),itfp->CanUseID());
}

void FlowChart::deleteItem()
{
    if(scene ==0){return ;}
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            (arrow->startItem())->removeArrow(arrow);
            (arrow->endItem())->removeArrow(arrow);
            delete item;
        }
    }

    selectedItems = scene->selectedItems();
    for (QGraphicsItem *item : qAsConst(selectedItems)) {

        if (item->type() == DiagramItem::Type){
            DiagramItem *tage = qgraphicsitem_cast<DiagramItem *>(item);
            tage->removeArrows();
            QByteArray qba = tage->Name().toLocal8Bit();
            const char *name = qba.data();
            itfp->StepBlockRemove(name,tage->sbbID());
        }
        scene->removeItem(item);
        delete item;
     }
}

void FlowChart::ArrowRelationshipMapSave(){
     if(scene ==0){return ;}
     QRectF rectf = scene->sceneRect();
     QList<QGraphicsItem *> selectedItems = scene->items(rectf);
     for (QGraphicsItem *item : qAsConst(selectedItems)) {
        if (item->type() == Arrow::Type) {
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            DiagramItem* from = arrow->startItem();
            DiagramItem* to = arrow->endItem();
            std::string fn = from->Name().toStdString();
            std::string tn = to->Name().toStdString();
            itfp->Linker(from->sbbID(),to->sbbID());
        }
     }
}

void FlowChart::SaveMyFile(){
     QString currentOpenDlgFilter;
     QStringList filterStrings;
     filterStrings << QString::fromLocal8Bit("Flower (*.ifwer)" ) << QString::fromLocal8Bit("All (*.*)" );
     QString currentPath="";
     QString selectedFile = QFileDialog::getSaveFileName(	this,
                                                               QString::fromLocal8Bit("另存为"),
                                                               currentPath,
                                                               filterStrings.join(";;"),
                                                               &currentOpenDlgFilter,
                                                               QFileDialog::Options());
     if (selectedFile.isEmpty())
         return;
     ArrowRelationshipMapSave();
     QByteArray qba = selectedFile.toLocal8Bit();
     const char *fp = qba.data();
     if(itfp){
        itfp->Save(fp);
     }
}

void FlowChart::OpenMyFile(){
     if(itfp==0){
        ccConsole::Print(QString::fromLocal8Bit("如要加载外部流程，请先进行流程创建!"));
        return;
     }
     QString currentOpenDlgFilter;
     QStringList filterStrings;
     filterStrings<< QString::fromLocal8Bit("Flower (*.ifwer)" ) << QString::fromLocal8Bit("All (*.*)" );
     QString currentPath="";
     QStringList selectedFiles = QFileDialog::getOpenFileNames(	this,
                                                               QString::fromLocal8Bit("打开可视化流程文件"),
                                                               currentPath,
                                                               filterStrings.join(";;"),
                                                               &currentOpenDlgFilter,
                                                               QFileDialog::Options());

     if (selectedFiles.isEmpty())
         return;

     QByteArray qba = selectedFiles[0].toLocal8Bit();
     const char *fp = qba.data();
     itfp->Load(fp);
     scene->clear();
     LoadShower();
}

void FlowChart::LoadShower(){
     int num = itfp->StepBlockCount();
     this->setWindowTitle(QString::fromLocal8Bit(itfp->FlowerName()));
     StepBlockBase *sbb;
     QList<QPointF> plst;
     for(int v = 0;v < num;v++){
         sbb = itfp->StepBlockGet(v);
         scene->setItemType(DiagramItem::Step);
         if(sbb->blkType() == StepBlockBase::CONDCMP_PROCESS)
         {
            scene->setItemType(DiagramItem::Conditional);
         }else{
            int id = sbb->ID();
            scene->setItemInfo(QString::fromLocal8Bit(sbb->Name()),(StepBlockBase::SBTYPE)sbb->blkType(),id);
         }
         rxsPointI *pi =sbb->GetRenderPos();
         QPointF cip(pi->x,pi->y);
         scene->CreaterItem(cip);
         plst.push_back(cip);
    }
     for(int nx=0;nx < num;nx++){
        sbb = itfp->StepBlockGet(nx);
        unsigned nums=0;
        int id = sbb->ID();
        int *ids = itfp->Linker(id,nums);
        if(nums>0 && ids !=0){
            QPointF p1=plst[nx];
            for(int jxx=0;jxx<nums;jxx++){
                int nid = ids[jxx];
                int toIndex=itfp->index(nid);
                QPointF p2=plst[toIndex];
                scene->CreaterArrower(p1,p2);
            }
        }
     }
}


void FlowChart::pointerGroupClicked()
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void FlowChart::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}

void FlowChart::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    const QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    for (const QGraphicsItem *item : overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}

void FlowChart::itemInserted(DiagramItem *item)
{
    if(itfp==0){
        ccConsole::Print(QString::fromLocal8Bit("请先进行流程创建!"));
        return;
    }
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
    QByteArray qba = item->Name().toLocal8Bit();
    QPointF p = item->pos();
    int x = p.x();
    int y = p.y();
    rxsPointI pi={x,y};
    const char* name = qba.data();
    if(item->sbbType() == StepBlockBase::IMG_IN_SOURCE){
        imgTaskFlowerStepBlockCreater<ImgSourceStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::THRD_PROCESS){
        imgTaskFlowerStepBlockCreater<ThridPartyCallBlk>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::IMG_OUT){
        imgTaskFlowerStepBlockCreater<ImgOutStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::IMG_SCANCODE){
        imgTaskFlowerStepBlockCreater<ScanCodeStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::COLORREG_UCM_PROCESS){
        imgTaskFlowerStepBlockCreater<ColorRecognitionStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::IMG_UCUSTOM_PROCESS){
        imgTaskFlowerStepBlockCreater<UserCustomStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::IMG_MERG_PROCESS){
        imgTaskFlowerStepBlockCreater<ImgMergingStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::IMG_LAMBPHOTOMETRIC){
        imgTaskFlowerStepBlockCreater<LambertianPhotoMetricstereoStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_IN_SOURCE){
        imgTaskFlowerStepBlockCreater<PointCouldSourceStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_OUT){
        imgTaskFlowerStepBlockCreater<PointCouldOutStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_FILTER){
        imgTaskFlowerStepBlockCreater<PointCouldFilterStep>(itfp,pi,name);
        return;
    }

    if(item->sbbType() == StepBlockBase::MOTION_MOV){
        imgTaskFlowerStepBlockCreater<MotionMovStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::CONVERT_DEPIMG_PC){
        imgTaskFlowerStepBlockCreater<FormatConvertStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_ARC){
        imgTaskFlowerStepBlockCreater<PointCouldArclenStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_POA_SL){
        imgTaskFlowerStepBlockCreater<PointCouldPoaslStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_DEFORMATION){
        imgTaskFlowerStepBlockCreater<PointCouldDeformationStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::ROIGRPMANGER){
        imgTaskFlowerStepBlockCreater<roiGrpMangerStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_EMPLEN){
        imgTaskFlowerStepBlockCreater<PointCouldEmplenStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_LENGTH){
        imgTaskFlowerStepBlockCreater<PointCouldLengthStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_GAP){
        imgTaskFlowerStepBlockCreater<PointCouldGapStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_PREPROCESS){
        imgTaskFlowerStepBlockCreater<PointCouldPreProcessStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::PC_BOUNDOL){
        imgTaskFlowerStepBlockCreater<PointCouldBoundolStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::TCP_SERVERRECV){
        imgTaskFlowerStepBlockCreater<TcpServerRecvStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::HW_FOCUSINGLENS){
        imgTaskFlowerStepBlockCreater<FocusingLensStep>(itfp,pi,name);
        return;
    }
    if(item->sbbType() == StepBlockBase::HW_LIGHTSOURCE){
        imgTaskFlowerStepBlockCreater<LightSourceStep>(itfp,pi,name);
        return;
    }
}

void FlowChart::currentFontChanged(const QFont &)
{
    handleFontChange();
}

void FlowChart::fontSizeChanged(const QString &)
{
    handleFontChange();
}

void FlowChart::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QTransform oldMatrix = view->transform();
    view->resetTransform();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}

void FlowChart::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}

void FlowChart::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}

void FlowChart::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}

void FlowChart::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}

void FlowChart::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}

void FlowChart::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}

void FlowChart::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}

void FlowChart::itemSelected(QGraphicsItem *item)
{
    if(item->type() == DiagramItem::Type){
        DiagramItem *ployItem = qgraphicsitem_cast<DiagramItem *>(item);
        //ployItem = (DiagramItem *)item;
        Form *src = (Form*) blkAttributePage->widget();
        if(src != 0){
            delete src;
            src=0;
        }
        if(itfp != 0){
            QByteArray qba= ployItem->Name().toLocal8Bit();
            QPointF pf = ployItem->pos();
            const char* sbn=qba.data();
            rxsPointI rp={(int)pf.x(),(int)pf.y()};
            StepBlockBase *sbb= itfp->StepBlockGet(sbn,ployItem->sbbID());
            if(sbb==0){
                return;
            }
            sbb->UpdateRenderPos(rp);
            Form *ioParamPage = new Form(sbb,this);
            blkAttributePage->setWidget(ioParamPage);
            if(sbb->blkType() == StepBlockBase::ROIGRPMANGER){
                photoLabel->set_StepModel(sbb);
                Roitoolf->LinkWith(photoLabel,Qt::TopRightCorner);
            }else{
                photoLabel->set_StepModel(0);
                Roitoolf->hide();
                photoLabel->clearShow();
            }
        }
        return;
    }
}

void FlowChart::createToolBox()
{
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &FlowChart::buttonGroupClicked);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("输入源"), DiagramItem::Io,StepBlockBase::IMG_IN_SOURCE), 0, 0);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("图像输出"), DiagramItem::Io,StepBlockBase::IMG_OUT), 0, 1);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("光度立体"), DiagramItem::Step,StepBlockBase::IMG_LAMBPHOTOMETRIC),1, 0);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("HDR"), DiagramItem::Step,StepBlockBase::IMG_MERG_PROCESS),1, 1);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("颜色识别"), DiagramItem::Step,StepBlockBase::COLORREG_UCM_PROCESS),2, 0);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("三方调用"), DiagramItem::Step,StepBlockBase::THRD_PROCESS),2, 1);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("扫码"), DiagramItem::Step,StepBlockBase::IMG_SCANCODE),3, 0);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("图像处理"), DiagramItem::Step,StepBlockBase::IMG_PREV_PROCESS),3, 1);
    layout->addWidget(createCellWidget(QString::fromLocal8Bit("自定义"), DiagramItem::Step,StepBlockBase::IMG_UCUSTOM_PROCESS),4, 0);
    layout->setRowStretch(5, 10);
    layout->setColumnStretch(2, 1);

    QWidget *itemWidget = new QWidget;
    itemWidget->setLayout(layout);

    QGridLayout *layoutD3 = new QGridLayout;
    QWidget *itemWidgetD3 = new QWidget;
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("输入源"), DiagramItem::Io,StepBlockBase::PC_IN_SOURCE), 0, 0);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("输出"), DiagramItem::Io,StepBlockBase::PC_OUT), 0, 1);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("滤波"), DiagramItem::Io,StepBlockBase::PC_FILTER), 1, 0);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("轮廓度"), DiagramItem::Io,StepBlockBase::PC_POA_SL), 1, 1);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("弧长"), DiagramItem::Io,StepBlockBase::PC_ARC), 2, 0);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("变形度"), DiagramItem::Io,StepBlockBase::PC_DEFORMATION), 2, 1);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("长度"), DiagramItem::Io,StepBlockBase::PC_LENGTH), 3, 0);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("间隙"), DiagramItem::Io,StepBlockBase::PC_EMPLEN), 3, 1);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("断差"), DiagramItem::Io,StepBlockBase::PC_GAP), 4, 0);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("预处理"), DiagramItem::Io,StepBlockBase::PC_PREPROCESS), 4, 1);
    layoutD3->addWidget(createCellWidget(QString::fromLocal8Bit("边界点"), DiagramItem::Io,StepBlockBase::PC_BOUNDOL), 5, 0);
    layoutD3->setRowStretch(6, 10);
    layoutD3->setColumnStretch(2, 1);
    itemWidgetD3->setLayout(layoutD3);

    QGridLayout *layoutCOPR = new QGridLayout;
    QWidget *itemWidgetCOPR= new QWidget;
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("运动控制"), DiagramItem::Io,StepBlockBase::MOTION_MOV), 0, 0);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("数字输出"), DiagramItem::Io,StepBlockBase::DSIGNAL_OUT), 0, 1);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("格式转换"), DiagramItem::Io,StepBlockBase::CONVERT_DEPIMG_PC), 1, 0);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("ROI"), DiagramItem::Io,StepBlockBase::ROIGRPMANGER), 1, 1);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("条件比较"), DiagramItem::Conditional,StepBlockBase::CONDCMP_PROCESS), 2, 0);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("调焦镜头"), DiagramItem::Io,StepBlockBase::HW_FOCUSINGLENS), 2, 1);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("可调光源"), DiagramItem::Io,StepBlockBase::HW_LIGHTSOURCE), 3, 0);
    layoutCOPR->addWidget(createCellWidget(QString::fromLocal8Bit("TCP服务"), DiagramItem::Io,StepBlockBase::TCP_SERVERRECV), 3, 1);
    layoutCOPR->setRowStretch(5, 10);
    layoutCOPR->setColumnStretch(2, 1);
    itemWidgetCOPR->setLayout(layoutCOPR);

    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &FlowChart::backgroundButtonGroupClicked);

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           ":/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           ":/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           ":/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           ":/images/background4.png"), 1, 1);
    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 1);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);

    toolBox = new QToolBox;
    //toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidgetCOPR, QString::fromLocal8Bit("功能组件"));
    toolBox->addItem(itemWidget, QString::fromLocal8Bit("2D功能"));
    toolBox->addItem(itemWidgetD3,QString::fromLocal8Bit("3D功能"));
    toolBox->addItem(backgroundWidget, QString::fromLocal8Bit("场景背景"));
}

void FlowChart::createActions()
{
    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, &QAction::triggered, this, &FlowChart::bringToFront);

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+T"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, &QAction::triggered, this, &FlowChart::sendToBack);

    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, &QAction::triggered, this, &FlowChart::deleteItem);

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    openAction = new QAction(QIcon(":/images/open.png"),tr("O&pen"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("open the ifwer file"));
    connect(openAction, &QAction::triggered, this, &FlowChart::OpenMyFile);

    saveAction = new QAction(QIcon(":/images/save.png"),tr("S&ave"), this);
    saveAction->setShortcuts(QKeySequence::SaveAs);
    saveAction->setStatusTip(tr("save the ifwer file"));
    connect(saveAction, &QAction::triggered, this, &FlowChart::SaveMyFile);

    newIfwerCreatAction= new QAction(QIcon(":/images/newcreat.png"),tr("create"), this);
    newIfwerCreatAction->setCheckable(true);
    newIfwerCreatAction->setStatusTip(tr("new ifwer creat"));
    connect(newIfwerCreatAction, &QAction::triggered, this, &FlowChart::newCreater);


    prevFlower = new QAction(QIcon(":/images/left.ico"),QString::fromLocal8Bit("上一个"), this);
    prevFlower->setCheckable(true);
    prevFlower->setStatusTip(QString::fromLocal8Bit("系统加载的流程集中的上一个流程"));
    connect(prevFlower, &QAction::triggered, this, &FlowChart::iscPrevFlowwer);

    nextFlower = new QAction(QIcon(":/images/right.ico"),QString::fromLocal8Bit("下一个"), this);
    nextFlower->setCheckable(true);
    nextFlower->setStatusTip(QString::fromLocal8Bit("系统加载的流程集中的下一个流程"));
    connect(nextFlower, &QAction::triggered, this, &FlowChart::iscNextFlowwer);


    collFlowLoad = new QAction(QIcon(":/images/camera_focus.ico"),QString::fromLocal8Bit("集合加载"), this);
    collFlowLoad->setCheckable(true);
    collFlowLoad->setStatusTip(QString::fromLocal8Bit("文件目录下的所有流程加载"));
    connect(collFlowLoad, &QAction::triggered, this, &FlowChart::collFlowLoader);

    clrIfwerAction = new QAction(QIcon(":/images/clear.png"),tr("clear"), this);
    clrIfwerAction->setCheckable(true);
    clrIfwerAction->setStatusTip(tr("clear the current sence ifwer"));
    connect(clrIfwerAction, &QAction::triggered, this, &FlowChart::clrIfwer);

    runTestAction = new QAction(QIcon(":/images/run.ico"),tr("run"), this);
    runTestAction->setCheckable(true);
    runTestAction->setStatusTip(tr("ifwer file run test"));
    connect(runTestAction, &QAction::triggered, this, &FlowChart::runTester);

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, &QAction::triggered, this, &FlowChart::handleFontChange);

    italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, &QAction::triggered, this, &FlowChart::handleFontChange);

    underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, &QAction::triggered, this, &FlowChart::handleFontChange);
}

void FlowChart::iscPrevFlowwer(){
    WorkProcessDal *wrkpd = WorkProcessDalGet() ;
    if(wrkpd->rxsImgTPC==0){
        return;
    }
    sysLoadIndex-=1;
    if(sysLoadIndex>-1){
        itfp = (*wrkpd->rxsImgTPC)[sysLoadIndex];
        if(itfp==0){
            return;
        }
        scene->clear();
        LoadShower();
    }else{
        sysLoadIndex=wrkpd->rxsImgTPC->Count();
        iscPrevFlowwer();
    }
}

void FlowChart::iscNextFlowwer(){
    WorkProcessDal *wrkpd = WorkProcessDalGet() ;
    if(wrkpd->rxsImgTPC==0){
        return;
    }
    int num =  wrkpd->rxsImgTPC->Count();
    sysLoadIndex+=1;
    if(sysLoadIndex<num){
        itfp = (*wrkpd->rxsImgTPC)[sysLoadIndex];
        if(itfp==0){
            return;
        }
        scene->clear();
        LoadShower();
    }else{
        sysLoadIndex=-1;
        iscNextFlowwer();
    }
}

void FlowChart::collFlowLoader(){
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->exec();
    auto selectDir = fileDialog->selectedFiles();
    if (selectDir.size()>0)
    {
        //qDebug() << "Dir Path:" << selectDir.at(0);
        WorkProcessDal *wrkpd = WorkProcessDalGet();
        QByteArray qba = selectDir.at(0).toLocal8Bit();
        const char *dirp = qba.data();
        wrkpd->ImgProcessFlowerLoad(dirp);
    }
}

void FlowChart::createMenus()
{
    fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("&文件"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(QString::fromLocal8Bit("&块操作"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);
}

void FlowChart::createToolbars()
{
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(openAction);
    editToolBar->addAction(saveAction);
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(collFlowLoad);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);
    editToolBar->addAction(runTestAction);
    editToolBar->addAction(newIfwerCreatAction);
    editToolBar->addAction(clrIfwerAction);
    editToolBar->addAction(prevFlower);
    editToolBar->addAction(nextFlower);

    fontCombo = new QFontComboBox();
    connect(fontCombo, &QFontComboBox::currentFontChanged,this, &FlowChart::currentFontChanged);

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, &QComboBox::currentTextChanged,
            this, &FlowChart::fontSizeChanged);

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, &QAbstractButton::clicked,
            this, &FlowChart::textButtonTriggered);

    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, &QAbstractButton::clicked,
            this, &FlowChart::fillButtonTriggered);

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, &QAbstractButton::clicked,
            this, &FlowChart::lineButtonTriggered);

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, &FlowChart::pointerGroupClicked);

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, &QComboBox::currentTextChanged,
            this, &FlowChart::sceneScaleChanged);

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);

}

QWidget *FlowChart::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QWidget *FlowChart::createCellWidget(const QString &text, DiagramItem::DiagramType type,StepBlockBase::SBTYPE sbt)
{
    DiagramItem item(type, itemMenu,text);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(80, 30));
    button->setCheckable(true);
    button->setText(text+"_"+QString::number(int(sbt)));
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

QMenu *FlowChart::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << QString::fromLocal8Bit("黑色") << QString::fromLocal8Bit("白色") << QString::fromLocal8Bit("红色")
          << QString::fromLocal8Bit("蓝色") << QString::fromLocal8Bit("黄色");
    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}

QIcon FlowChart::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);
    return QIcon(pixmap);
}

QIcon FlowChart::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
