#ifndef SOLUTIONCONTROL_H
#define SOLUTIONCONTROL_H

#include <QDialog>
#include "IVmSolution.h"
#include "VMException.h"
#include <QTranslator>
#include <qDebug>
using namespace VisionMasterSDK;
using namespace VisionMasterSDK::VmSolution;

namespace Ui {
class SolutionControl;
}

class SolutionControl : public QDialog
{
    Q_OBJECT

public:
    explicit SolutionControl(QWidget *parent = 0);
    ~SolutionControl();

private slots:
    void on_solPathChooseButton_clicked();

    void on_loadSolButton_clicked();

    void on_saveSolButton_clicked();

    void on_getSolPathButton_clicked();

    void on_closeSolPathButton_clicked();

    void on_getSolVersionButton_clicked();

    void on_checkSolPasswordButton_clicked();

    void on_lockWorkAreaButton_clicked();

    void on_unlockWorkAreaButton_clicked();

    void on_clearMsgButton_clicked();

    void changeEvent(QEvent *e); 

    void on_changeLanguageButton_clicked();

    void slots_exception(int code, QString source, QString desc, QString help);

private:
    QTranslator m_qTrans; 
    Ui::SolutionControl *ui;
    bool m_bIsChineseLanguage;

private:
    IVmSolution * m_pVmSol;
};

#endif // SOLUTIONCONTROL_H
