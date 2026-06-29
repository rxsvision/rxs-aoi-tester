#ifndef TCPSERVERRECVFORM_H
#define TCPSERVERRECVFORM_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class TcpServerRecvForm;
}

class TcpServerRecvForm : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit TcpServerRecvForm(imgTaskFlowerProcesser *,int curID,QWidget *parent = nullptr);
    ~TcpServerRecvForm();


    bool SetParamValue(void *) override;
    bool GetParamValue(void *) override;

private slots:
    void on_SendTester_clicked();

private:
    Ui::TcpServerRecvForm *ui;
};

#endif // TCPSERVERRECVFORM_H
