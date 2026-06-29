#ifndef PCSRCINFORM_H
#define PCSRCINFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class PCSrcInForm;
}

class PCSrcInForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit PCSrcInForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~PCSrcInForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_selectFile_clicked();

    void on_selecteDir_clicked();

    void on_axAdd_clicked();

    void on_sensorParams_clicked();

    void on_axDel_clicked();

private:
    Ui::PCSrcInForm *ui;
};

#endif // PCSRCINFORM_H
