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

    //对于存在计算结果显示的，重新刷新本界面
    void ResultRefush(int sid);
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
    void ParamPagePlanClr();
    void ShowHasParamPage();

    template <typename T>
    void ParamPanelCreate(QWidget *ui_inppl){
        T *iopform = new T(itfpr,sbb->ID(),ui_inppl);
        rui = iopform;
        Vboxlayout->addWidget(iopform);
    }

    template <typename Tf,typename Ts,typename Tp>
    void ParamCreateAndAppend(QWidget *ui_inppl){
        ParamPanelCreate<Tf>(ui_inppl);
        Ts *stepblk = (Ts *)sbb;
        Tp *sbp = stepblk->ParamGet<Tp>();
        rui->MainUiSet((FlowChart *)this->parent());
        if(sbp !=0 ){
            rui->SetParamValue(sbp);
        }
    }

    template <typename Tf,typename Ts>
    void ParamCreateAndAppend(QWidget *ui_inppl){
        ParamPanelCreate<Tf>(ui_inppl);
        Ts *stepblk = (Ts *)sbb;
        rui->SetParamValue(0);
    }

    FlowChart *mainFcer;
};

#endif // FORM_H
