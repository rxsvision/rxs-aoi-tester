#ifndef PCPOASLFORM_H
#define PCPOASLFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class pcPoaslForm;
}

class pcPoaslForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit pcPoaslForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~pcPoaslForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private:
    Ui::pcPoaslForm *ui;
};

#endif // PCPOASLFORM_H
