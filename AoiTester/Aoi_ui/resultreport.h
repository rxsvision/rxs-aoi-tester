#ifndef RESULTREPORT_H
#define RESULTREPORT_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "rxsFixdal.h"
#include "ccConsole.h"


namespace Ui {
class ResultReport;
}

class ResultReport : public QWidget
{
    Q_OBJECT

public:
    explicit ResultReport(QWidget *parent = nullptr);
    ~ResultReport();
    void UpdNextResult();
    void ResultDataClr();
    void rrRepListSelectedSet();
    void UpdateRRTemp();
    short GetCurrentReportStatus(int rps);

private slots:
    void on_expclr_clicked();
    void on_chkboxRunSaveRp_stateChanged(int arg1);
    void on_rgAssemable_clicked();
    void on_ReportGrps_currentIndexChanged(int index);
    void on_addRow_clicked();

    void on_editMode_clicked();

    void on_meResultReport_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::ResultReport *ui;

    void on_clrData_clicked();
    void on_expData_clicked();
    void ShowRRList_Head(const char*,bool clrContent = true);
    void ShowRRLIst_Val(const char*);
    void ShowRRLIst_ValNext(const char*);
    QString curRRName;
    bool inEdit;
};

#endif // RESULTREPORT_H
