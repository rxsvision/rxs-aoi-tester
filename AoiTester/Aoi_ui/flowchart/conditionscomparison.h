#ifndef CONDITIONSCOMPARISON_H
#define CONDITIONSCOMPARISON_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class ConditionsComparison;
}

class ConditionsComparison : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit ConditionsComparison(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~ConditionsComparison();

private:
    Ui::ConditionsComparison *ui;
};

#endif // CONDITIONSCOMPARISON_H
