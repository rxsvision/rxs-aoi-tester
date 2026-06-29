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

private:
    Ui::roiGrpMangForm *ui;
};

#endif // ROIGRPMANGFORM_H
