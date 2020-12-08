#include "calcnodeadd.h"
#include <iostream>

CalcNodeAdd::CalcNodeAdd(QQuickItem *parent): CalcNode (parent)
{
    createSockets(2, 1);
    setTitle("Add");
    setContentText("+");
    setType(OP_ADD);
}

void CalcNodeAdd::operation() {
    m_socketOutput[0]->setValue(m_socketsInput[0]->value().toInt() + m_socketsInput[1]->value().toInt());
}
