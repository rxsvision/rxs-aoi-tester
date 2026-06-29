#ifndef FIXMAINTENANCE_H
#define FIXMAINTENANCE_H

#include <QDialog>

namespace Ui {
class FixMaintenance;
}

class FixMaintenance : public QDialog
{
    Q_OBJECT

public:
    explicit FixMaintenance(void *mainFrame,QWidget *parent = nullptr);
    ~FixMaintenance();
    void uiref_();
private slots:
    void on_spRecord_clicked();
    void on_epRecord_clicked();
    void on_StartScan_clicked();
    void on_ReadCurVal_clicked();
    void on_SetCurVal_clicked();
    void on_dgScaner_clicked();

    void on_allSave_clicked();
    void on_addVal_clicked();
    void on_delVal_clicked();
    void on_fullClr_clicked();

    void on_OutPosSet_clicked();

    void on_InPosSet_clicked();

    void on_AddNew_clicked();

    void on_DelSel_clicked();

    void on_ClrRemap_clicked();

private:
    Ui::FixMaintenance *ui;
    void *mainFramePtr;
    unsigned AxNum;


    void pageValSetRef();
};

#endif // FIXMAINTENANCE_H
