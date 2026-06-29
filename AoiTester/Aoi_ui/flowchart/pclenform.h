#ifndef PCLENFORM_H
#define PCLENFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcLenForm;
}

class pcLenForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcLenForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcLenForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_boundSrc_clicked(bool checked);

private:
    Ui::pcLenForm *ui;
};

#endif // PCLENFORM_H
