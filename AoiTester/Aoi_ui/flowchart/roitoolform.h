#ifndef ROITOOLFORM_H
#define ROITOOLFORM_H

#include <QWidget>
#include <photolabel.h>

namespace Ui {
class roiToolForm;
}

class roiToolForm : public QWidget
{
    Q_OBJECT

public:
    explicit roiToolForm(QWidget *parent = nullptr);
    ~roiToolForm();

    void LinkWith(PhotoLabel *tage,Qt::Corner pos);

private slots:
    void on_rectRoi_triggered(QAction *arg1);
    void on_confim_triggered(QAction *arg1);
    void on_cancelRoi_triggered(QAction *arg1);

private:
    Ui::roiToolForm *ui;
    void repositionOverlay(Qt::Corner pos);
    PhotoLabel *vpWidget;
};

#endif // ROITOOLFORM_H
