#ifndef SYSSTATUS_H
#define SYSSTATUS_H

#include <QWidget>

namespace Ui {
class sysStatus;
}

class sysStatus : public QWidget
{
    Q_OBJECT

public:
    explicit sysStatus(void *rxsmp,QWidget *parent = nullptr);
    ~sysStatus();

    void uiCtr_ref();
    void UpdateCycleTime(long t=-1);
    QString GetRepCnt();
    QString GetProductInfo();
private slots:
    void on_noCalu_clicked(bool checked);
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_movOutPos_clicked();
    void on_zSaftyPos_clicked();
    void on_movInPos_clicked();

    void on_AutoOutMatter_clicked(bool checked);

    void on_srcSave_clicked(bool checked);

    void on_noCaluTwod_stateChanged(int arg1);

private:
    Ui::sysStatus *ui;
};

#endif // SYSSTATUS_H
