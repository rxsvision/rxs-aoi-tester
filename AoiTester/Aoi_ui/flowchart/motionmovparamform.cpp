#include "motionmovparamform.h"
#include "ui_motionmovparamform.h"

motionmovparamform::motionmovparamform(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent) :
    ShowParamBaseForm(itfp,curID,parent),
    ui(new Ui::motionmovparamform)
{
    ui->setupUi(this);
}

motionmovparamform::~motionmovparamform()
{
    delete ui;
}

bool motionmovparamform::GetParamValue(void *d) {
    int nums = ui->AxSelList->rowCount();
    if(nums<1){return false;}
    std::vector<AxMoveparam> svam;
    QByteArray *axnames = new QByteArray[nums];
    for(int x=0 ; x< nums ; x++){
        AxMoveparam amp;
        axnames[x] = ui->AxSelList->item(x,0)->text().toLocal8Bit();
        amp.AxName = axnames[x].data();
        amp.method = ui->AxSelList->item(x,1)->text().toLocal8Bit().data()[0];
        amp.TagePos=ui->AxSelList->item(x,2)->text().toFloat();
        amp.TageVel=ui->AxSelList->item(x,3)->text().toFloat();
        amp.waitInp=ui->AxSelList->item(x,4)->text().toFloat();
        amp.acc=ui->AxSelList->item(x,5)->text().toFloat();
        amp.dec=ui->AxSelList->item(x,6)->text().toFloat();
        svam.push_back(amp);
    }
    MotionMovStep *sbb = (MotionMovStep *)((*itfper)[Curid]);
    sbb->AxListCreater(nums,&svam[0]);
    delete[] axnames;
    return true;
}

bool motionmovparamform::SetParamValue(void *d) {
    MotionMovParam *mmp = (MotionMovParam *)d;
    int rc = 0;
    while((rc = ui->AxSelList->rowCount())>0)
    {
        ui->AxSelList->removeRow(0);
    }
    QTableWidgetItem *qwi = 0;
    for(int x=0;x< mmp->AxNum;x++){
        ui->AxSelList->insertRow(x);
        qwi = new QTableWidgetItem(QString::fromLocal8Bit(mmp->AxisLister[x].AxName));
        ui->AxSelList->setItem(x,0,qwi);
        char v[2]={(char)mmp->AxisLister[x].method,0};
        qwi = new QTableWidgetItem(QString::fromLocal8Bit(v));
        ui->AxSelList->setItem(x,1,qwi);
        qwi = new QTableWidgetItem(QString::number(mmp->AxisLister[x].TagePos));
        ui->AxSelList->setItem(x,2,qwi);
        qwi = new QTableWidgetItem(QString::number(mmp->AxisLister[x].TageVel));
        ui->AxSelList->setItem(x,3,qwi);
        qwi = new QTableWidgetItem(QString::number(mmp->AxisLister[x].waitInp));
        ui->AxSelList->setItem(x,4,qwi);
        qwi = new QTableWidgetItem(QString::number(mmp->AxisLister[x].acc));
        ui->AxSelList->setItem(x,5,qwi);
        qwi = new QTableWidgetItem(QString::number(mmp->AxisLister[x].dec));
        ui->AxSelList->setItem(x,6,qwi);
    }
    return true;
}

void motionmovparamform::on_addNew_clicked()
{
    int rc = ui->AxSelList->rowCount();
    ui->AxSelList->insertRow(rc);
    QTableWidgetItem *qwi = new QTableWidgetItem(QString::fromLocal8Bit("AOI-X"));
    ui->AxSelList->setItem(rc,0,qwi);
    ui->AxSelList->setItem(rc,1,new QTableWidgetItem(QString::fromLocal8Bit("A")));
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("20.0"));
    ui->AxSelList->setItem(rc,2,qwi);
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("20.0"));
    ui->AxSelList->setItem(rc,3,qwi);
    ui->AxSelList->setItem(rc,4,new QTableWidgetItem(QString::fromLocal8Bit("-1")));
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("100.0"));
    ui->AxSelList->setItem(rc,5,qwi);
    qwi = new QTableWidgetItem(QString::fromLocal8Bit("100.0"));
    ui->AxSelList->setItem(rc,6,qwi);
}


void motionmovparamform::on_SelDel_clicked()
{
    QList<QTableWidgetItem*> ss = ui->AxSelList->selectedItems();
    int rc = ss.count();
    if(rc<1){return;}
    foreach (QTableWidgetItem* s, ss) {
        ui->AxSelList->removeRow(s->row());
    }
}

