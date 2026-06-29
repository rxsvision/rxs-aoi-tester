#ifndef IMGSRCINFORM_H
#define IMGSRCINFORM_H

#include <QWidget>
#include "showparambaseform.h"


namespace Ui {
class ImgSrcInForm;
}

class ImgSrcInForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ImgSrcInForm(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~ImgSrcInForm();

    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_pushButton_clicked();
    void on_lightSource_cellDoubleClicked(int row, int column);

    void on_selectFile_clicked();

    void on_selecteDir_clicked();

private:
    Ui::ImgSrcInForm *ui;
};

#endif // IMGSRCINFORM_H
