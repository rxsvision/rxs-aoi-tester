#include "twodtester.h"
#include "ui_twodtester.h"
#include "mainwindow.h"


twoDtester::twoDtester(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::twoDtester)
{
    ui->setupUi(this);
    ToolsListFill();
    UpdateRRList();
}

twoDtester::~twoDtester()
{
    delete ui;
}

void twoDtester::UpdateRRList(){
    if(ui->rrTpList->count()>0){
        ui->rrTpList->clear();
    }
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    unsigned Nums=0;
    const char *v=0;
    while((v=wrkpd->ResultReportForech(Nums))!=0){
        QString txt = QString::fromLocal8Bit(v);
        ui->rrTpList->addItem(txt);
        Nums+=1;
    }
}

void twoDtester::ToolsListFill(){
    ui->toolsList->clear();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    bool s = true;
    const char *val=0;
    while((val = wrkpd->imgToolsForeach(s))!=0){
        QString sv = QString::fromLocal8Bit(val);
        ui->toolsList->addItem(sv);
    }
}

void twoDtester::on_lsvRecord_clicked()
{
}

void twoDtester::on_Tester_clicked()
{
    QList<QListWidgetItem*> SS = ui->toolsList->selectedItems();
    if( SS.count()<1){
        return;
    }
    QByteArray tookitName = SS[0]->text().toLocal8Bit();
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->imgProcessTester(tookitName.data());
}

