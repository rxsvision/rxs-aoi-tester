#ifndef IMGOUTPARAM_H
#define IMGOUTPARAM_H

#include <QWidget>
#include "showparambaseform.h"


namespace Ui {
class ImgOutParam;
}

class ImgOutParam : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ImgOutParam(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~ImgOutParam();


    bool GetParamValue(void *) override;
    bool SetParamValue(void *) override;
private slots:
    void on_savePathSet_clicked();

    void on_srcSelected_clicked();

    void on_Addele_clicked();

    void on_clrSelected_clicked();

private:
    Ui::ImgOutParam *ui;
};

#endif // IMGOUTPARAM_H
