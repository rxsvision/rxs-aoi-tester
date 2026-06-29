#include "resultreport.h"
#include "ui_resultreport.h"
#include "rxsFixdal.h"

WorkProcessDal *wrkpd;


ResultReport::ResultReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultReport)
{
    ui->setupUi(this);
    wrkpd = WorkProcessDalGet();
    ui->meResultReport->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->meResultReport->setSelectionMode(QAbstractItemView::SingleSelection);
    UpdateRRTemp();
    rrRepListSelectedSet();
    ui->chkboxRunSaveRp->setChecked(true);
    inEdit = false;
}

ResultReport::~ResultReport()
{
    delete ui;
}

void ResultReport::rrRepListSelectedSet(){
    if(ui->ReportGrps->count()>0){
        ui->ReportGrps->clear();
    }
    const char *v = 0;
    const char *last = 0;
    int Nums=0;
    while((v=wrkpd->ResultReportForech(Nums))!=0){
        last = v;
        QString txt = QString::fromLocal8Bit(v);
        ui->ReportGrps->addItem(txt);
        Nums+=1;
    }
    if(last == 0){return;}
    ui->ReportGrps->setCurrentText(QString::fromLocal8Bit(last));
}

void ResultReport::ShowRRList_Head(const char* s,bool clrContent){
    QStringList scpGrpheader;
    int  i =0;
    const char *v=0;
    while((v = wrkpd->ResultReportFiledsGet(s,i))!=0){
        scpGrpheader<<QString::fromLocal8Bit(v);
        i+=1;
    }
    int ns = scpGrpheader.length();
    if(clrContent){
        if(ns>0){
            while(ui->meResultReport->rowCount()){
                ui->meResultReport->removeRow(0);
            }
            ui->meResultReport->setColumnCount(ns);
            ui->meResultReport->setHorizontalHeaderLabels(scpGrpheader);
        }
    }
}

void ResultReport::ShowRRLIst_Val(const char* s){
    int i=0;
    const char *v = 0;
    bool isng=false;
    unsigned rows = wrkpd->ResultReportRowCount(s);
    unsigned cols = ui->meResultReport->columnCount();
    for(unsigned row = 0;row < rows;row++){
        ui->meResultReport->insertRow(row);
        i=0;
        while(i<cols){
            QByteArray qba = ui->meResultReport->horizontalHeaderItem(i)->text().toLocal8Bit();
            const char *thName = qba.data();
            if((v = wrkpd->ResultReportFiledValGet(s,row,thName))!=0){
                QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit(v));
                if(val->text()==QString::fromLocal8Bit("NG")){
                    isng=true;
                }
                ui->meResultReport->setItem(row,i,val);
            }
            else{
                QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("/"));
                ui->meResultReport->setItem(rows,i,val);
            }
            i+=1;
        }
        if(isng){
            for(int col =0;col<ui->meResultReport->columnCount();col++){
                QTableWidgetItem *val =ui->meResultReport->item(row,col);
                if(val != 0){
                    val->setBackground(QBrush(Qt::red));
                }
            }
        }
        isng=false;
    }
}

void ResultReport::ShowRRLIst_ValNext(const char* s){
    const char *v = 0;
    bool isng=false;
    unsigned rows = wrkpd->ResultReportRowCount(s);
    if(rows<1){return ;}
    int i=0;
    unsigned cols = ui->meResultReport->columnCount();
    rows-=1;
    ui->meResultReport->insertRow(rows);
    while(i<cols){
        QByteArray qba = ui->meResultReport->horizontalHeaderItem(i)->text().toLocal8Bit();
        const char *thName = qba.data();
        if((v = wrkpd->ResultReportFiledValGet(s,rows,thName))!=0){
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit(v));
            if(val->text()==QString::fromLocal8Bit("NG")){
                isng=true;
            }
            ui->meResultReport->setItem(rows,i,val);
        }else{
            QTableWidgetItem *val = new QTableWidgetItem(QString::fromLocal8Bit("/"));
            ui->meResultReport->setItem(rows,i,val);
        }
        i+=1;
    }
    if(isng){
        for(int col =0;col<cols;col++){
            QTableWidgetItem *val =ui->meResultReport->item(rows,col);
            if(val != 0){
                val->setBackground(QBrush(Qt::red));
            }
        }
    }
    isng = false;
}

void ResultReport::UpdNextResult(){

}

void ResultReport::ResultDataClr(){
    if(wrkpd !=0){
        wrkpd->CheckResultClr();
    }
    ui->meResultReport->clearContents();
    while(ui->meResultReport->rowCount()){
        ui->meResultReport->removeRow(0);
    }
}

void ResultReport::on_clrData_clicked()
{
    ResultDataClr();
}

void ResultReport::on_expData_clicked()
{
    if(wrkpd !=0){
        wrkpd->SaveCheckResultReport();
    }
}

short ResultReport::GetCurrentReportStatus(int rps)
{

    return 0;
}

void ResultReport::UpdateRRTemp(){
//    if(ui->rrTempSelected->count()>0){
//        ui->rrTempSelected->clear();
//    }
//    WorkProcessDal *wrkpd = WorkProcessDalGet();
//    unsigned Nums=0;
//    const char *v=0;
//    while((v=wrkpd->ResultReportTempNameForech(Nums))!=0){
//        QString txt = QString::fromLocal8Bit(v);
//        ui->rrTempSelected->addItem(txt);
//        Nums+=1;
//    }
}

void ResultReport::on_expclr_clicked()
{
    on_expData_clicked();
    ResultDataClr();
}

void ResultReport::on_chkboxRunSaveRp_stateChanged(int arg1)
{
    if(ui->chkboxRunSaveRp->checkState() == Qt::Checked){
        wrkpd->CheckResultReportSave();
    }else{
        wrkpd->CheckResultReportSave(false);
    }
}

void ResultReport::on_rgAssemable_clicked()
{
    //报表隐藏字段设定
}


void ResultReport::on_ReportGrps_currentIndexChanged(int index)
{
    rxsMainFrame *psys = rxsMainFrameGet();
    if(psys->sysStatus->CurFixStatus() == SystemInfo::FixStaus::RUNING){
        ccConsole::Print(QString::fromLocal8Bit("加工运行中,请加工换成后在切换"));
        return;
    }
    int curi = ui->ReportGrps->currentIndex();
    QString curtext = ui->ReportGrps->currentText();
    if(curtext.length()>0){
        QByteArray qba = curtext.toLocal8Bit();
        const char* s = qba.data();
        ShowRRList_Head(s);
        ShowRRLIst_Val(s);
    }
}

void ResultReport::on_addRow_clicked()
{
    int rc = ui->meResultReport->rowCount();
    ui->meResultReport->insertRow(rc);
    int cn = ui->meResultReport->columnCount();
    if(cn == 0){
    }

    for(int cl=0; cl < cn; cl++){
        ui->meResultReport->setItem(rc,cl,new QTableWidgetItem(QString::number(cl)));
    }
}


void ResultReport::on_editMode_clicked()
{
    if(inEdit){
        inEdit = false;
        ui->editMode->setText(QString::fromLocal8Bit("进入编辑"));
    }else{
        inEdit=true;
        ui->editMode->setText(QString::fromLocal8Bit("退出编辑"));
    }
}


void ResultReport::on_meResultReport_itemDoubleClicked(QTableWidgetItem *item)
{
    if(inEdit){
        return;
    }else{
        if(wrkpd !=0){
            int cri = ui->meResultReport->currentRow();
            if(cri<0){
                ccConsole::Warning(QString::fromLocal8Bit("请先在解结果报表栏中选中"));
                return;
            }
            int colnums = ui->meResultReport->columnCount();
            QByteArray kid = ui->meResultReport->item(cri,colnums-2)->text().toLocal8Bit();
            QByteArray ofn = ui->meResultReport->item(cri,colnums-1)->text().toLocal8Bit();
            wrkpd->PointCould_MatchShow(kid.data(),0,ofn.data());
        }
    }
}

