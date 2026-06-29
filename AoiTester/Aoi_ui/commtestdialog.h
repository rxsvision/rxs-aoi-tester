#ifndef COMMTESTDIALOG_H
#define COMMTESTDIALOG_H

#include <QDialog>

namespace Ui {
class commTestDialog;
}

class commTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit commTestDialog(QWidget *parent = nullptr);
    ~commTestDialog();

    void clientListFill();

private slots:
    void on_cmdSend_clicked();

    void on_ipList_doubleClicked(const QModelIndex &index);

    void on_openTcpServer_clicked();

    void on_CloseServer_clicked();

    void on_refClient_clicked();

private:
    Ui::commTestDialog *ui;
};

#endif // COMMTESTDIALOG_H
