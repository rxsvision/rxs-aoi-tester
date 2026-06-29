#ifndef AXISINFOCTR_H
#define AXISINFOCTR_H

#include <QWidget>
#include "rxsFixdal.h"

namespace Ui {
class AxisInfoCtr;
}

class AxisInfoCtr : public QWidget
{
    Q_OBJECT

public:
    explicit AxisInfoCtr(AxisInfo *,QWidget *parent = nullptr);
    ~AxisInfoCtr();
    AxisInfo *Axi;
    void ui_refush_dataupdata();
private slots:
    void on_ServOn_clicked();
    void on_jogPmove_pressed();
    void on_jogPmove_released();
    void on_jogNmove_pressed();
    void on_jogNmove_released();
    void on_absMove_clicked();
    void on_relMov_clicked();

private:
    Ui::AxisInfoCtr *ui;
};

#endif // AXISINFOCTR_H
