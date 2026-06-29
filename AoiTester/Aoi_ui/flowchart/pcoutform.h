#ifndef PCOUTFORM_H
#define PCOUTFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcOutForm;
}

class pcOutForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcOutForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcOutForm();


    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private:
    Ui::pcOutForm *ui;
};

#endif // PCOUTFORM_H
