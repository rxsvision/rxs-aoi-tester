#ifndef PCARCLENFORM_H
#define PCARCLENFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcArclenForm;
}

class pcArclenForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcArclenForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcArclenForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

    void ShowRRLIst_Val();
private:
    Ui::pcArclenForm *ui;
};

#endif // PCARCLENFORM_H
