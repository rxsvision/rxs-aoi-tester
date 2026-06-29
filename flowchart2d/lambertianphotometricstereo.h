#ifndef LAMBERTIANPHOTOMETRICSTEREO_H
#define LAMBERTIANPHOTOMETRICSTEREO_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class LambertianPhotometricStereo;
}

class LambertianPhotometricStereo : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit LambertianPhotometricStereo(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~LambertianPhotometricStereo();

private:
    Ui::LambertianPhotometricStereo *ui;
};

#endif // LAMBERTIANPHOTOMETRICSTEREO_H
