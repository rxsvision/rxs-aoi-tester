#ifndef PCDEFORMATIONFORM_H
#define PCDEFORMATIONFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class PcDeformationForm;
}

class PcDeformationForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit PcDeformationForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~PcDeformationForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_modefileBtn_clicked();

private:
    Ui::PcDeformationForm *ui;
};

#endif // PCDEFORMATIONFORM_H
