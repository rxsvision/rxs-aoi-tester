#ifndef CONVERTFORMATFORM_H
#define CONVERTFORMATFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class ConvertFormatForm;
}

class ConvertFormatForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ConvertFormatForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~ConvertFormatForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_efSelected_clicked();

private:
    Ui::ConvertFormatForm *ui;
};

#endif // CONVERTFORMATFORM_H
