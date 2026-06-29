#ifndef DATAOUTSET_H
#define DATAOUTSET_H

#include <QWidget>
#include "showparambaseform.h"

namespace Ui {
class DataOutSet;
}

class DataOutSet : public ShowParamBaseForm
{
    Q_OBJECT

public:
    explicit DataOutSet(imgTaskFlowerProcesser *itfp,int curID,QWidget *parent = nullptr);
    ~DataOutSet();

private:
    Ui::DataOutSet *ui;
};

#endif // DATAOUTSET_H
