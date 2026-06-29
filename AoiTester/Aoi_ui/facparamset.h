#ifndef FACPARAMSET_H
#define FACPARAMSET_H

#include <QDialog>

namespace Ui {
class FacParamSet;
}

class FacParamSet : public QDialog
{
    Q_OBJECT

public:
    explicit FacParamSet(void *,QWidget *parent = nullptr);
    ~FacParamSet();
    void ref_ui_data();

private slots:
    void on_InsertData_clicked();

    void on_DeleteData_clicked();

    void on_save_clicked();

private:
    Ui::FacParamSet *ui;
    void *mainFramePtr;
    void LoadParam();
};

#endif // FACPARAMSET_H
