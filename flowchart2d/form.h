#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QVBoxLayout>
#include "imgProcess.h"
#include "showparambaseform.h"
#include "flowchart.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(StepBlockBase *,QWidget *parent = nullptr);
    ~Form();
    void *toSaveStruct();
    imgTaskFlowerProcesser* itfpr;
private slots:
    void on_tester_clicked();
    void on_confim_clicked();
    void on_nameSet_clicked();

private:
    Ui::Form *ui;
    QString curBlkName;
    ShowParamBaseForm *rui;
    StepBlockBase *sbb;

    QVBoxLayout *Vboxlayout;
    void ShowHasParamPage();
    void ImgSourceBlkParamCreate();
    void ThridPartBlkParamCreate();
    void ImgOutBlkParamCreate();
    void ScanCodeParamCreate();
    void LambertianPhotometricStereoBlkParamCreate();
    void DataOutParamCreate();
    void ConditionsBlkParamCreate();
    void ColorBlkParamCreate();
    void HdrBlkParamCreate();
    void MotionMoveParamCreate();
    void UseCustomParamCreate();
    void FormatConvertParamCreate();
    void PointCouldSourceParamCreate();
    void PointCouldOutParamCreate();
    void PointCouldFilterParamCreate();
    void roiMangerParamCreate();
    void PointCouldArclenParamCreate();
    void PointCouldPoaslParamCreate();

    FlowChart *mainFcer;
};

#endif // FORM_H
