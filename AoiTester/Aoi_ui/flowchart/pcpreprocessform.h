#ifndef PCPREPROCESSFORM_H
#define PCPREPROCESSFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcPreprocessForm;
}

class pcPreprocessForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcPreprocessForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcPreprocessForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_AddFNPS_clicked();

    void on_DelFNPS_clicked();

private:
    Ui::pcPreprocessForm *ui;
};

#endif // PCPREPROCESSFORM_H
