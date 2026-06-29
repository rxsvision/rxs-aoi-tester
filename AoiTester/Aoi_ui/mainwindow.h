#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QTime>
#include <QKeyEvent>
#include <rxsFixdal.h>
#include "roicomputerset.h"
#include "scopter.h"
#include "fixmaintenance.h"
#include "login.h"
#include "lasescaner.h"
#include "facparamset.h"
#include "help.h"
#include "usrmanger.h"
#include "sysstatus.h"
#include "resultreport.h"
#include "motionopter.h"

#include <QtWidgets/QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

//qCC_db
#include <ccHObject.h>
#include <ccHObjectCaster.h>
//qCC_gl
#include <ccGLUtils.h>

//Local
#include "ccMainAppInterface.h"
#include "ccPickingListener.h"
#include "ccDBRoot.h"

//CCLib
#include <AutoSegmentationTools.h>
#include "ccGraphicalSegmentationTool.h"
#include <ccClipBox.h>
#include "commtestdialog.h"


class ccGLWindow;
class ccPluginUIManager;
class ccClippingBoxTool;
class ccComparisonDlg;
class ccPointPropertiesDlg;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow,public ccMainAppInterface, public ccPickingListener
{
    Q_OBJECT
protected:
    MotionOpter *modoOpter;
    LaseScaner  *lsrScaner;
    FixMaintenance  *wndFixm;
    Login   *userLogin;
    help    *myHelp;
    FacParamSet *facParamSet;
    usrManger   *usem;
    sysStatus *sysstatus;
    scOpter* SCopter;
    ResultReport *rerp;
    roiComputerSet *roiCpset;

    QLabel  *sysRunTime;
    QLabel  *curUser;
    QLabel  *msgText;

    QSignalMapper *mpActionMap;

    ccPluginUIManager	*m_pluginUIManager;
    ccHObject::Container m_selectedEntities;
//    void wheelEvent(QWheelEvent *event) override;
//    void keyPressEvent(QKeyEvent *event) override;
//    void keyReleaseEvent(QKeyEvent *event) override;
    bool isCtrKeyPress;
    bool ismyKeyPress;
    std::vector<AxisInfoCtr*> AicGrps;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    PhotoLabel *photoLabel;
    static int BackMessageProcess(void* pc,int code,void *self);

    ccHObject *myGetCCObjFromDB(QString pcName);
    short myPointCouldDataShow(QString name,rxsPointCouldp *,unsigned pointNums);
    short myImageDataShow(QString name,QImage *img);
    short myLabelDataShow(const char *name,rxsPoint pos,unsigned index);
    short myDefineLabelDataShow(const char *name,QStringList content,rxsPoint pos,unsigned index);
    short myClipBoxShow();
    void myClipBoxDestory();
    short myBoundBoxShow();
    void myRoiAreaSet(ccHObject*,CCVector3 maxCorner,CCVector3 minCorner);
    bool myRoiAreaSet(QString name);
    short myComputerFlowTester(const char*);
    rxsImg myImgGetFromCCWnd();
    bool SelectedEntitiesSet(int wich =0);
    void unSelectedEntities(int wich=0);
    void disableButtonFunction(UserManger::USER_AUTH ua);

    /*---以下----目前仅在 CorssSection 功能中使用 后期可优化掉*/
    ccDBRoot* db();
    ccGLWindow* new3DView( bool allowEntitySelection );
    void addToDB( const QStringList& filenames,QString fileFilter = QString(),ccGLWindow* destWin = nullptr );
    void addToDB( ccHObject* obj,bool updateZoom = false,bool autoExpandDBTree = true,bool checkDimensions = false,bool autoRedraw = true ) override;

    //! Forces display of console widget
    void forceConsoleDisplay() override;

    void onItemPicked(const PickedItem& pi) override;
    ccGLWindow* getActiveGLWindow() override;
    void createGLWindow(ccGLWindow*& window, QWidget*& widget) const override;
    void destroyGLWindow(ccGLWindow*) const override;

    //! Registers a MDI area 'overlay' dialog
    /** Overlay dialogs are displayed in the central MDI area, above the 3D views.
    The position (pos) is defined relatively to the MDI area (as one of its 4 corners).
    And it is automatically updated when the main window is moved or resized.
    Registered dialogs are automatically released when CloudCompare stops.

    Notes:
    - it may be necessary to call 'updateOverlayDialogsPlacement' after calling this method
    - it's a good idea to freeez the UI when the tool starts to avoid other overlay dialogs
    to appear (don't forget to unfreeze the UI afterwards)
    **/
    void registerOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos) override;

    //! Unregisters a MDI area 'overlay' dialog
    /** \warning Original overlay dialog object will be deleted (see QObject::deleteLater)
    **/
    void unregisterOverlayDialog(ccOverlayDialog* dlg) override;

    //! Forces the update of all registered MDI 'overlay' dialogs
    void updateOverlayDialogsPlacement() override;

    ccUniqueIDGenerator::Shared getUniqueIDGenerator() override;

    void removeFromDB(ccHObject* obj, bool autoDelete = true) override;


    //! Removes object temporarily from DB tree
    /** This method must be called before any modification to the db tree
        \warning May change the set of currently selected entities
    **/
    ccHObjectContext removeObjectTemporarilyFromDBTree(ccHObject* obj) override;

    //! Adds back object to DB tree
    /** This method should be called once modifications to the db tree are finished
        (see removeObjectTemporarilyFromDBTree).
    **/
    void putObjectBackIntoDBTree(ccHObject* obj, const ccHObjectContext& context) override;

    //! Selects or unselects an entity (in db tree)
    /** \param obj entity
        \param selected whether entity should be selected or not
    **/
    void setSelectedInDB(ccHObject* obj, bool selected) override;

    inline  QMainWindow* getMainWindow() override { return this; }
    inline  const ccHObject::Container& getSelectedEntities() const override { return m_selectedEntities; }

    void dispToConsole(QString message, ConsoleMessageLevel level = STD_CONSOLE_MESSAGE) override;
    ccHObject* dbRootObject() override;

    //! Forces redraw of all GL windows
    /** \param only2D whether to redraw everything (false) or only the 2D layer (true)
    **/
    void redrawAll(bool only2D = false) override;
    void refreshAll(bool only2D = false) override;
    void updateUI() override;
    void freezeUI(bool state) override;

    //! Returns color scale manager (unique instance)
    ccColorScalesManager* getColorScalesManager() override;

    //! Spawns an histogram dialog
    void spawnHistogramDialog(	const std::vector<unsigned>& histoValues,
                              double minVal,
                              double maxVal,
                              QString title,
                              QString xAxisLabel) override;

    ccPickingHub* pickingHub() override { return nullptr; }
    void setView( CC_VIEW_ORIENTATION view ) override;

private slots:
    void toggleActiveWindowCenteredPerspective() override;
    void toggleActiveWindowCustomLight() override;
    void toggleActiveWindowSunLight() override;
    void toggleActiveWindowViewerBasedPerspective() override;
    void zoomOnSelectedEntities() override;
    void setGlobalZoom() override;

    void echoMouseWheelRotate(float);
    void echoCameraDisplaced(float ddx, float ddy);
    void echoBaseViewMatRotation(const ccGLMatrixd& rotMat);
    void echoCameraPosChanged(const CCVector3d&);
    void echoPivotPointChanged(const CCVector3d&);
    void echoPixelSizeChanged(float);

    void onExclusiveFullScreenToggled(bool);
    void prepareWindowDeletion(QObject* glWindow);
    void handleNewLabel(ccHObject*);
    void deactivatePointPickingMode(bool);
    void addToDBAuto(const QStringList& filenames);

    void increasePointSize() override;
    void decreasePointSize() override;

    //--以上--ccMainAppInterface---接口实现--提供给Plugs，DBtree 的一些控件内部操作 使用--完全可以优化掉---
    void MainUi_ref();
    void on_action_triggered();
    void on_actionUserLogin_triggered();   
    void on_actionHelp_triggered();
    void on_actionWrkStart_triggered();
    void on_actionWrkStop_triggered();
    void on_actionWrkPause_triggered();
    void on_actionSysReset_triggered();
    void on_actionCalb_triggered();
    void on_actionUserManger_triggered();
    void on_actionCCfile_triggered();
    void MpOpter(QString oid);
    //void SpOpter(QString oid);
    //void TskOpter(QString oid);
    void on_actionHeightRamp_triggered();
    void on_actionAssociationCfg_triggered();
    void on_actionLoadWrkfile_triggered();
    void on_actionSaveWrkfile_triggered();
    void on_actionPCSave_triggered();
    void activateClippingBoxMode();
    void on_actionLCC_triggered();
    void createComponentsClouds(ccGenericPointCloud* cloud,
                                            CCLib::ReferenceCloudContainer& components,
                                            unsigned minPointsPerComponent,bool randomColors,
                                            bool selectComponents,bool sortBysize=true);

    void on_actionSORFilter_triggered();
    void on_actionCpCpDist_triggered();
    void deactivateComparisonMode(int);
    void on_actionClrView_triggered();
    void on_action3DPointGet_triggered();
    void cancelPreviousPickingOperation(bool aborted);

    void on_actionEntMange_triggered();
    void on_AxDioOpter_triggered();
    void on_actionRunPicTester_triggered();

    //Graphical segmentation
    void deactivateSegmentationMode(bool);
    void SegmentationModeRoipro(rxsPointCouldp *roiCoorV,int vNum,rxsPointCouldp lwh,int,QString);
    void on_actionVisualFlow_triggered();
    void on_actionCut_triggered();

    void on_actionLocComm_triggered();

    void on_pointOffset_clicked();

    void on_modelTech_clicked();

    void on_pushButton_clicked();

    void on_VisualRunaction_triggered();

private:
    Ui::MainWindow *ui;
    rxsMainFrame *mainFrame;
    commTestDialog *ComTester;

    void GetWrkPoingGrps();
    void FillWrkPointGrps();
    void repositionOverlayDialog(ccOverlayDialog* dlg, Qt::Corner pos);
    ccHObject *LoadFileTo_ccobject(QString filepath);

    QTimer *timer;
    ccClippingBoxTool* m_clipTool;
    ccGraphicalSegmentationTool* m_gsTool;
    ccDBRoot* m_ccRoot;
    QWidget* viewWidget = nullptr;
    ccGLWindow* view3D = nullptr;
    ccComparisonDlg* m_compDlg;
    QGridLayout *ccvBox;
    QGridLayout *rrpBox;

    unsigned mroTotalCnt;//单一路径测量结果计数
    ccPointPropertiesDlg* m_ppDlg;
    ccPickingHub* m_pickingHub;

    std::vector<ccClipBox*> vccb;
    QString tskCollectionStr;
    void ToggleWorkProAction();
};
#endif // MAINWINDOW_H
