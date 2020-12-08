#ifndef CALCNODEINPUT_H
#define CALCNODEINPUT_H
#include "calcnode.h"

class CalcNodeInput: public CalcNode
{
    Q_OBJECT
public:
    CalcNodeInput(QQuickItem *parent = nullptr);
    void operation();
};

#endif // CALCNODEINPUT_H
