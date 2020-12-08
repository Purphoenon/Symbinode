#include "calcnodemul.h"

CalcNodeMul::CalcNodeMul(QQuickItem *parent): CalcNode (parent)
{
    createSockets(2, 1);
    setTitle("Multiply");
    setContentText("*");
    setType(OP_MUL);
}

void CalcNodeMul::operation() {
    m_socketOutput[0]->setValue(m_socketsInput[0]->value().toInt() * m_socketsInput[1]->value().toInt());
}
