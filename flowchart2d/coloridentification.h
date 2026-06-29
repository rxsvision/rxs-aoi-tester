#ifndef COLORIDENTIFICATION_H
#define COLORIDENTIFICATION_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class ColorIdentification;
}

class ColorIdentification : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ColorIdentification(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~ColorIdentification();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:

    void on_rootDirSelected_clicked();
    void on_pushButton_clicked();

    void on_gtr_clicked();

private:
    Ui::ColorIdentification *ui;
};

#endif // COLORIDENTIFICATION_H
