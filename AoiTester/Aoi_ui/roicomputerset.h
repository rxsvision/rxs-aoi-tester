#ifndef ROICOMPUTERSET_H
#define ROICOMPUTERSET_H

#include <QDialog>

namespace Ui {
class roiComputerSet;
}

class roiComputerSet : public QDialog
{
    Q_OBJECT

public:
    explicit roiComputerSet(QWidget *parent = nullptr);
    ~roiComputerSet();

    void ref_ui_data();

private slots:
    void on_LookSelected_clicked();
    //void on_AddRC_clicked();
    //void on_AddRep_clicked();
    //void on_CfgSave_clicked();
    //void on_testCT_clicked();
    void on_roiOK_clicked();
    void on_removeRoi_clicked();

    void on_delCould_clicked();
    void on_SelectedCE_clicked();
    void on_roiUnSelected_clicked();

    void on_algTestButton_clicked();
    void on_pcHidden_clicked();
    void on_pcShow_clicked();
    void on_UnSelected_clicked();

    void on_clrBtn_clicked();

    void on_absRoiConvert_clicked();

    void on_relRoiConvert_clicked();

private:
    Ui::roiComputerSet *ui;
    void UpdateRoiList();
    void ToolKitNamesFill();
    void UpdateCouldList();

//    void UpdateRRTemp();
//    void UpdateRRList();
//    void UpdateCompTx();
};

#endif // ROICOMPUTERSET_H
