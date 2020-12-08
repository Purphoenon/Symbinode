#ifndef CALCNODEMUL_H
#define CALCNODEMUL_H
#include "calcnode.h"

class CalcNodeMul: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeMul(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODEMUL_H
