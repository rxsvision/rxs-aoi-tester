#ifndef MOTIONOPTER_H
#define MOTIONOPTER_H

#include <QDialog>
#include <QPushButton>
#include <QSignalMapper>
#include <QTime>
#include "rxsFixdal.h"
#include "AxisInfoCtr.h"

typedef struct {
    int Rows;
    int Cols;
}DioLayout;


namespace Ui {
class MotionOpter;
}

class MotionOpter : public QDialog
{
    Q_OBJECT

public:
    explicit MotionOpter(QWidget *parent = nullptr);
    ~MotionOpter();


private slots:
    void DoutSignalSet(QString oName);
    void dioUpdate();
    void on_fixReset_clicked();
    void on_EmgStop_clicked();

private:
    Ui::MotionOpter *ui;
    rxsMainFrame *mfper;
    dioPort *DoGrp =0;
    dioPort *DiGrp=0;

    QPushButton *DinBtn;
    QPushButton *DoutBtn;
    unsigned DoutNum;
    unsigned DinNum;
    QTimer *timer;
    DioLayout   Dirc;
    DioLayout   Dorc;
    QSignalMapper *doutMapID;

    void *rsxMainFramePtr;

    void DIOLayout();
    void RefsuhDio();

    std::vector<AxisInfoCtr*> AicGrps;
};

#endif // MOTIONOPTER_H
