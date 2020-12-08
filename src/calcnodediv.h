#ifndef CALCNODEDIV_H
#define CALCNODEDIV_H
#include "calcnode.h"

class CalcNodeDiv: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeDiv(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODEDIV_H
