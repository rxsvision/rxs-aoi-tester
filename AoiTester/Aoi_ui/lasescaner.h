#ifndef LASESCANER_H
#define LASESCANER_H

#include <QDialog>
#include <QTime>
#include <QListWidgetItem>
#include <QComboBox>
#include "photolabel.h"
#include <QThread>
namespace Ui {
class LaseScaner;
}

class LaseScaner : public QDialog
{
    Q_OBJECT

public:
    explicit LaseScaner(QWidget *parent = nullptr);
    ~LaseScaner();
    void FillImageableNames(QComboBox *isCombox);
    PhotoLabel *photoLabel;
    void mainReadyOpter();

private slots:
    void onTimer();
    void on_Conn_clicked();
    void on_scanTigger_clicked();
    void on_paramSet_clicked();
    void on_StopCollection_clicked();
    void on_chwBower_clicked();
    void on_cfgBower_clicked();
    void onComboxSelect(int);
    void on_dark_clicked();
    void on_numZero_clicked();
    void on_GathPrev_clicked();
    void on_CloseCon_clicked();
    void on_sensorList_itemClicked(QListWidgetItem *item);
    void on_horizontalSlider_sliderMoved(int position);

    void on_conGraber_clicked();
    void on_crmGraber_clicked();

    void on_AutoLCIGrab_clicked();

    void on_AutoLPGrab_clicked();

    void on_Slider_huangguang_valueChanged(int value);
    void on_Slider_tongzhou_valueChanged(int value);
    void on_Slider_duoguangpu_valueChanged(int value);

private:
    int GetColorIndex();
    int GetColorIndexAndVal(unsigned char &val);
    void FillShowLightInfo();
    Ui::LaseScaner *ui;
    void ShowData_ref();
    void CreamGraber();
    void lpGraberScaner();

    //bit 0,相机连续采集
    //bit 1,点光谱，线扫 采集
    int optflag;
    QTimer *timer;


    void on_lsOpen_clicked();
    void on_lsClose_clicked();
    void on_lsvSet_clicked();
};

#endif // LASESCANER_H
