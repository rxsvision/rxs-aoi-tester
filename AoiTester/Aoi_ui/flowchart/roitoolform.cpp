#include "roitoolform.h"
#include "ui_roitoolform.h"
#include <iostream>

roiToolForm::roiToolForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::roiToolForm)
{
    ui->setupUi(this);
}

roiToolForm::~roiToolForm()
{
    delete ui;
}


void roiToolForm::repositionOverlay(Qt::Corner pos){
    int dx = 0;
    int dy = 0;
    static const int margin = 1;
    switch (pos)
    {
    case Qt::TopLeftCorner:
        dx = margin;
        dy = margin;
        break;
    case Qt::TopRightCorner:
        dx = std::max(margin, vpWidget->width() - this->width() - margin);
        dy = margin;
        break;
    case Qt::BottomLeftCorner:
        dx = margin;
        dy = std::max(margin, vpWidget->height() - this->height() - margin);
        break;
    case Qt::BottomRightCorner:
        dx = std::max(margin, vpWidget->width() - this->width() - margin);
        dy = std::max(margin, vpWidget->height() - this->height() - margin);
        break;
    }
    this->move(QPoint(dx, dy));
    //this->raise();
}

void roiToolForm::LinkWith(PhotoLabel *tage,Qt::Corner pos){
    vpWidget = tage;
    repositionOverlay(pos);
    //setWindowFlags(Qt::Window |Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    show();
}

void roiToolForm::on_rectRoi_triggered(QAction *arg1)
{
    std::cout << "rect" << std::endl;
    PhotoLabel *vpl = (PhotoLabel *)this->parent();
    vpl->set_draw_shap(DRAW_SHAP_E::DRAW_RECT);
}


void roiToolForm::on_confim_triggered(QAction *arg1)
{
    std::cout << "confim" << std::endl;
    PhotoLabel *vpl = (PhotoLabel *)this->parent();
    vpl->set_draw_shap(DRAW_SHAP_E::DRAW_OK);
}


void roiToolForm::on_cancelRoi_triggered(QAction *arg1)
{
    std::cout << "cancelRoi" << std::endl;
    PhotoLabel *vpl = (PhotoLabel *)this->parent();
    vpl->set_draw_shap(DRAW_SHAP_E::DRAW_NO);
}

