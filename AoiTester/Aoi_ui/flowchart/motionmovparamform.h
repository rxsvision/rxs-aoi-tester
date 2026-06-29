#ifndef MOTIONMOVPARAMFORM_H
#define MOTIONMOVPARAMFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class motionmovparamform;
}

class motionmovparamform : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit motionmovparamform(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~motionmovparamform();

    bool GetParamValue(void *) override;
    bool SetParamValue(void *) override;

private slots:
    void on_addNew_clicked();

    void on_SelDel_clicked();

private:
    Ui::motionmovparamform *ui;
};

#endif // MOTIONMOVPARAMFORM_H
