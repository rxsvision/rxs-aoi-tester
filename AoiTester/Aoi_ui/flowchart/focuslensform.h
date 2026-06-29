#ifndef FOCUSLENSFORM_H
#define FOCUSLENSFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class FocusLensForm;
}

class FocusLensForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit FocusLensForm(imgTaskFlowerProcesser *,int curID,QWidget *parent = nullptr);
    ~FocusLensForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_adjVal_valueChanged(int value);

private:
    Ui::FocusLensForm *ui;
};

#endif // FOCUSLENSFORM_H
