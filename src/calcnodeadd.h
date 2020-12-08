#ifndef CALCNODEADD_H
#define CALCNODEADD_H
#include "calcnode.h"

class CalcNodeAdd: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeAdd(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODEADD_H
