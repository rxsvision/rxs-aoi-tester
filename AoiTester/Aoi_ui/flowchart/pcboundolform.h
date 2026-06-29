#ifndef PCBOUNDOLFORM_H
#define PCBOUNDOLFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcBoundolForm;
}

class pcBoundolForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcBoundolForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcBoundolForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_AddParam_clicked();

    void on_DelParam_clicked();

    void on_autoFind_stateChanged(int arg1);

private:
    Ui::pcBoundolForm *ui;

    void tableClrCols();
    void tableClrRows();
};

#endif // PCBOUNDOLFORM_H
