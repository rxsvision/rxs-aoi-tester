#ifndef LIGHTSOURCEFORM_H
#define LIGHTSOURCEFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class LightSourceForm;
}

class LightSourceForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit LightSourceForm(imgTaskFlowerProcesser *,int curID,QWidget *parent = nullptr);
    ~LightSourceForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_rdo_channels_clicked(bool checked);

    void on_rdo_Multispectral_clicked(bool checked);

    void on_Add_clicked();

    void on_Remove_clicked();

private:
    Ui::LightSourceForm *ui;
    void tableReSet(int,msclspParam *);
    void tableDataGet(int,msclspParam *);
};

#endif // LIGHTSOURCEFORM_H
