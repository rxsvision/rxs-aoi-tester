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

    void imTypeSet(int t);
    bool GetParamValue(void *) override;
    bool SetParamValue(void *) override;
private slots:
    void on_dirSelected_clicked();
    void on_slAdd_clicked();
    void on_slDel_clicked();


//    void on_Add_clicked();
//    void on_Del_clicked();

private:
    Ui::LambertianPhotometricStereo *ui;
    ///融合类型,1-HDR,2-fuse（多幅不同焦距的图片融合）,3 光度立体法
    int mType;
//    void sparamSetToUI(ImgMergingParam *imp);
//    void sparamFillToBlk(ImgMergingParam *imp,ImgMergingStep *ims);
};

#endif // LAMBERTIANPHOTOMETRICSTEREO_H
