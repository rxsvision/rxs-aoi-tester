#ifndef USECUSTOMFORM_H
#define USECUSTOMFORM_H

#include <QWidget>
#include <QTableWidgetItem>
#include "showparambaseform.h"

namespace Ui {
class UseCustomForm;
}

class UseCustomForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit UseCustomForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~UseCustomForm();


    bool GetParamValue(void *) override;
    bool SetParamValue(void *) override;

private slots:
    void on_apiGrp_itemClicked(QTableWidgetItem *item);

private:
    Ui::UseCustomForm *ui;
};

#endif // USECUSTOMFORM_H
