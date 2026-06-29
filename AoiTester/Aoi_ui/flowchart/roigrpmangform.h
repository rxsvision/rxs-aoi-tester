#ifndef ROIGRPMANGFORM_H
#define ROIGRPMANGFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class roiGrpMangForm;
}

class roiGrpMangForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit roiGrpMangForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~roiGrpMangForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_addNew_clicked();

    void on_delSel_clicked();

    void on_techImp_clicked();

    void on_clrBtn_clicked();

    void on_impTech_clicked();

private:
    Ui::roiGrpMangForm *ui;
    void roiTableClr();
    void roiTableGet(std::vector<roiRect> &data,int rows);
    void roiTableAdd(roiRect *rois,int row);
};

#endif // ROIGRPMANGFORM_H
