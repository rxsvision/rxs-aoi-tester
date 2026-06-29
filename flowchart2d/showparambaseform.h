#ifndef SHOWPARAMBASEFORM_H
#define SHOWPARAMBASEFORM_H

#include <QWidget>
#include <QtWidgets/QComboBox>
#include "imgProcess.h"


namespace Ui {
class ShowParamBaseForm;
}

class ShowParamBaseForm : public QWidget
{
    Q_OBJECT

public:
    explicit ShowParamBaseForm(imgTaskFlowerProcesser *,int curID,QWidget *parent = nullptr);
    ~ShowParamBaseForm();

    //设置参数内容到UI上显示
    virtual bool SetParamValue(void *);
    //获取ui上参数同步到块
    virtual bool GetParamValue(void *);

protected:
    int Curid;
    imgTaskFlowerProcesser *itfper;
    void ShowFromSrcLinker(QComboBox *selectLinker, int filterID);
    bool SelectLinkerSetToUi(QComboBox *selectLinker,FromLinker *flr);
    bool SelectLinkerGetFromUi(QComboBox *selectLinker,FromLinker *flr);

private:
    Ui::ShowParamBaseForm *ui;
};

#endif // SHOWPARAMBASEFORM_H
