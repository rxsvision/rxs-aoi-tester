#ifndef THIRDPARTY_H
#define THIRDPARTY_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class Thirdparty;
}

class Thirdparty : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit Thirdparty(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~Thirdparty();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;
private slots:
    void on_LoadFileAction_clicked();

private:
    Ui::Thirdparty *ui;
};

#endif // THIRDPARTY_H
