#ifndef TWODTESTER_H
#define TWODTESTER_H

#include <QDialog>

namespace Ui {
class twoDtester;
}

class twoDtester : public QDialog
{
    Q_OBJECT

public:
    explicit twoDtester(QWidget *parent = nullptr);
    ~twoDtester();

private slots:
    void on_lsvRecord_clicked();

    void on_Tester_clicked();

private:
    Ui::twoDtester *ui;
    void UpdateRRList();
    void ToolsListFill();
};

#endif // TWODTESTER_H
