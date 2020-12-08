#include "calcnodesub.h"

CalcNodeSub::CalcNodeSub(QQuickItem *parent): CalcNode (parent)
{
    createSockets(2, 1);
    setTitle("Substract");
    setContentText("-");
    setType(OP_SUB);
}

void CalcNodeSub::operation() {
    m_socketOutput[0]->setValue(m_socketsInput[0]->value().toInt() - m_socketsInput[1]->value().toInt());
}
