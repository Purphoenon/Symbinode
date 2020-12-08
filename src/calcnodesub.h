#ifndef CALCNODESUB_H
#define CALCNODESUB_H
#include "calcnode.h"

class CalcNodeSub: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeSub(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODESUB_H
