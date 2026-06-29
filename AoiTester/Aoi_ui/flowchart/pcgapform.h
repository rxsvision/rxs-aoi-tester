#ifndef PCGAPFORM_H
#define PCGAPFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcGapForm;
}

class pcGapForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcGapForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcGapForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;


private:
    Ui::pcGapForm *ui;
};

#endif // PCGAPFORM_H
