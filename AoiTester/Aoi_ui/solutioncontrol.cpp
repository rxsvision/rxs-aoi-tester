#include "solutioncontrol.h"
#include "ui_solutioncontrol.h"
#include <QFileDialog>

void SolutionControl::slots_exception(int code, QString source, QString desc, QString help)
{
    throw code;
}

SolutionControl::SolutionControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolutionControl)
{
    ui->setupUi(this);
    connect(ui->axWidgetProc, SIGNAL(exception(int, QString, QString, QString)), this, SLOT(slots_exception(int, QString, QString, QString)));
    try
    {
        m_pVmSol = CreateSolutionInstance();
        ui->axWidgetProc->dynamicCall("GetObjectPointer()");
        m_bIsChineseLanguage = true;
    }
    catch (CVmException e)
    {
        QString strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == CreateSolutionInstance()";
        ui->textEdit->append(strReMsg);
    }
    catch (...)
    {
        QString strReMsg = "Interface Exception!";
        ui->textEdit->append(strReMsg);
        return;
    }
}

SolutionControl::~SolutionControl()
{
    try
    {
        DisposeResource();
    }
    catch (CVmException ex)
    {
        int nErrorCode = ex.GetErrorCode();
    }

    delete ui;
}

void SolutionControl::on_solPathChooseButton_clicked()
{
    QString strTemp = QStringLiteral("Choose solution path");
    QString strFileName = QFileDialog::getOpenFileName(this, strTemp, "", "SOL(*.sol)");
    if (!strFileName.isEmpty())
    {
        ui->SolutionPathEdit->setText(strFileName);
    }
}

void SolutionControl::on_loadSolButton_clicked()
{
    QString strSolPath = ui->SolutionPathEdit->text();
    QString strPassword = ui->SolPasswordEdit->text();
    QString strReMsg = "";

    if (strSolPath.isEmpty()) strSolPath = "";
    if (strPassword.isEmpty()) strPassword = "";

    try
    {
        m_pVmSol = LoadSolution(strSolPath.toStdString().c_str(), strPassword.toStdString().c_str());
        if (NULL == m_pVmSol)
        {
            return;
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == LoadSolution()";
        ui->textEdit->append(strReMsg);
        return;
    }

    strReMsg = "LoadSolution success.";
    ui->textEdit->append(strReMsg);
}

void SolutionControl::on_saveSolButton_clicked()
{
    QString strSolPath = ui->SolutionPathEdit->text();
    QString strPassword = ui->SolPasswordEdit->text();
    QString strReMsg = "";

    if (strSolPath.isEmpty()) strSolPath = "";
    if (strPassword.isEmpty()) strPassword = "";

    try
    {
        if (NULL != m_pVmSol)
        {
            m_pVmSol->SaveAsSolution(strSolPath.toStdString().c_str(), strPassword.toStdString().c_str());
        }
        else
        {
            strReMsg = "No solution file.";
            ui->textEdit->append(strReMsg);
            return;
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == SaveAsSolution()";
        ui->textEdit->append(strReMsg);
        return;
    }

    strReMsg = "SaveAsSolution success.";
    ui->textEdit->append(strReMsg);
}

void SolutionControl::on_getSolPathButton_clicked()
{
    QString strReMsg = "";
    try
    {
        if (NULL != m_pVmSol)
        {
            const char * strSolPath = m_pVmSol->GetSolutionPath();
            if (NULL != strSolPath)
            {
                strReMsg = QString::fromLocal8Bit(strSolPath);
                strReMsg = "Solution path:" + strReMsg;
                ui->textEdit->append(strReMsg);
            }
        }
        else
        {
            strReMsg = "No solution file.";
            ui->textEdit->append(strReMsg);
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == GetSolutionPath()";
        ui->textEdit->append(strReMsg);
    }
}

void SolutionControl::on_closeSolPathButton_clicked()
{
    QString strReMsg = "";
    try
    {
        if (NULL != m_pVmSol)
        {
            DestroySolutionInstance(m_pVmSol);
            m_pVmSol = NULL;
        }
        else
        {
            strReMsg = "No solution file.";
            ui->textEdit->append(strReMsg);
            return;
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == Closesolution()";
        ui->textEdit->append(strReMsg);
        return;
    }

    strReMsg = "Closesolution success.";
    ui->textEdit->append(strReMsg);
}

void SolutionControl::on_getSolVersionButton_clicked()
{
    QString strReMsg = "";
    try
    {
        if (NULL != m_pVmSol)
        {
            const char * strVersion = m_pVmSol->GetSolutionVersion();
            if (NULL != strVersion)
            {
                strReMsg = QString::fromLocal8Bit(strVersion);
                strReMsg = "Solution Version: " + strReMsg;
                ui->textEdit->append(strReMsg);
            }
        }
        else
        {
            strReMsg = "No solution file.";
            ui->textEdit->append(strReMsg);
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == GetSolutionVersion()";
        ui->textEdit->append(strReMsg);
    }
}

void SolutionControl::on_checkSolPasswordButton_clicked()
{
    QString strReMsg = "";
    try
    {
        if (NULL != m_pVmSol)
        {
            if (m_pVmSol->HasPassword())
            {
                strReMsg = "The solution has password.";
                ui->textEdit->append(strReMsg);
            }
            else
            {
                strReMsg = "The solution has no password.";
                ui->textEdit->append(strReMsg);
            }
        }
        else
        {
            strReMsg = "No solution file.";
            ui->textEdit->append(strReMsg);
        }
    }
    catch (CVmException e)
    {
        strReMsg = QString::number(e.GetErrorCode(), 16);
        strReMsg = "0x" + strReMsg + " == CheckSolutionPassword()";
        ui->textEdit->append(strReMsg);
    }
}

void SolutionControl::on_lockWorkAreaButton_clicked()
{
    ui->axWidgetProc->dynamicCall("LockWorkArea()");
}

void SolutionControl::on_unlockWorkAreaButton_clicked()
{
    ui->axWidgetProc->dynamicCall("UnlockWorkArea()");
}

void SolutionControl::on_clearMsgButton_clicked()
{
    ui->textEdit->clear();
}

void SolutionControl::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void SolutionControl::on_changeLanguageButton_clicked()
{
    // 仅切换Demo界面语言，控件语言通过配置文件切换
    // Only switch the language of demo interface,and switch the language of control through the configuration file
    if(m_bIsChineseLanguage)
    {
        m_qTrans.load(":/english");
        m_bIsChineseLanguage = false;
    }
    else
    {
        m_qTrans.load(":/chinese");
        m_bIsChineseLanguage = true;
    }
    qApp->installTranslator(&m_qTrans);
}
