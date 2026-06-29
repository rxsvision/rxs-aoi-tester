#ifndef AXMOTION_H
#define AXMOTION_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QTime>


namespace Ui {
class AxMotion;
}


typedef struct _axInfo{
    QGroupBox   *GrpBox;
    QPushButton *Pjog;
    QPushButton *Njog;
    QPushButton *Org;
    QLineEdit   *PosShow;
    QLabel      *HomeS;
    QLabel      *Plimt;
    QLabel      *Nlimt;
    QPushButton *Sevron;
}axInfoLayout;

class AxMotion : public QDialog
{
    Q_OBJECT

public:
    explicit AxMotion(void *pSystem,QWidget *parent = nullptr);
    ~AxMotion();

private:
    Ui::AxMotion *ui;
    unsigned AxNum;
    axInfoLayout *layoutWidgetGrp;
    QTimer *timer;
    void AxInfoShowLayout();
    QSignalMapper *orgMapID;
    QSignalMapper *jogMapID;
    QSignalMapper *stopMapID;
    QSignalMapper *servMapID;

    void *rsxMainFramePtr;
    void ReleaseLayoutSource();
    void RefushStatus();

private slots:
    void JogMove(QString sVal);
    void AxOrg(QString AxName);
    void AxServon(QString AxName);
    void AxMotionStop(QString AxName);
    void statusUpdate();
};

#endif // AXMOTION_H
