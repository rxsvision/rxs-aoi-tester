#include "help.h"
#include "ui_help.h"
#include "rxsFixdal.h"

help::help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::help)
{
    ui->setupUi(this);
}

help::~help()
{
    delete ui;
}

void help::on_OpenDir_clicked()
{
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    wrkpd->ShowOpenDir("C:\\Users\\Administrator\\Desktop\\SVN_Real3DMeasure\\QtUi\\各项说明书\\Real3DMeasureV2.0软件说明书.doc");
}

