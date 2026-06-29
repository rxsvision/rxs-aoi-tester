#ifndef PCFILTERFORM_H
#define PCFILTERFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcFilterForm;
}

class pcFilterForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcFilterForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcFilterForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private:
    Ui::pcFilterForm *ui;
};

#endif // PCFILTERFORM_H
