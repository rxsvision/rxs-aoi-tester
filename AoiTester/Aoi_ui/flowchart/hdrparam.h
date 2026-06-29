#ifndef HDRPARAM_H
#define HDRPARAM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class hdrParam;
}

class hdrParam : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit hdrParam(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~hdrParam();

private:
    Ui::hdrParam *ui;
};

#endif // HDRPARAM_H
