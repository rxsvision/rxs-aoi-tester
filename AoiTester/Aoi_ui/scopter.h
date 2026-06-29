#ifndef SCOPTER_H
#define SCOPTER_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class scOpter;
}

class scOpter : public QDialog
{
    Q_OBJECT

public:
    explicit scOpter(void *mainFramer,QWidget *parent = nullptr);
    ~scOpter();

private slots:
    void on_pushButton_clicked();
    void on_FilePathSelecte_clicked();
    void on_AutoGCT_stateChanged(int arg1);
    void on_wrkShow_stateChanged(int arg1);
    void on_WrkDView_currentIndexChanged(int index);

    void on_SoutRtl_clicked();
    void on_AddReport_clicked();
    void on_repSetSave_clicked();
    void on_SinLtr_clicked();

    void on_DelReport_clicked();
    void on_axGrpCreater_clicked();
    void on_axGrpRemover_clicked();

    void on_AxCombiner_itemClicked(QListWidgetItem *item);
    void on_rrItems_itemClicked(QListWidgetItem *item);

private:
    Ui::scOpter *ui;
    void UpdateValue();
    void AllReportFiledFill();
    void ReportTempFill();
    void AxFill();
    void mCombinerFill();
    void mCombinerSalveFill(QString cn);
    void on_ReportDetail_clicked(QString n);
    void on_NeedFiledclr_clicked();
};

#endif // SCOPTER_H
