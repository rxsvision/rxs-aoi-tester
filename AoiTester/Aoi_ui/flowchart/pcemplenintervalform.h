#ifndef PCEMPLENINTERVALFORM_H
#define PCEMPLENINTERVALFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcEmplenIntervalForm;
}

class pcEmplenIntervalForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcEmplenIntervalForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcEmplenIntervalForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private:
    Ui::pcEmplenIntervalForm *ui;
};

#endif // PCEMPLENINTERVALFORM_H
