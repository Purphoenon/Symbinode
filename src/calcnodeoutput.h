#ifndef CALCNODEOUTPUT_H
#define CALCNODEOUTPUT_H
#include "calcnode.h"

class CalcNodeOutput: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeOutput(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODEOUTPUT_H
