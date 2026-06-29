#ifndef SCANCODEFORM_H
#define SCANCODEFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class ScanCodeForm;
}

class ScanCodeForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ScanCodeForm(imgTaskFlowerProcesser *,int curID,QWidget *parent = nullptr);
    ~ScanCodeForm();


    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_roiAdd_clicked();

private:
    Ui::ScanCodeForm *ui;
};

#endif // SCANCODEFORM_H
