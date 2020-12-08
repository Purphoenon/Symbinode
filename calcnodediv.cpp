#include "calcnodediv.h"

CalcNodeDiv::CalcNodeDiv(QQuickItem *parent): CalcNode (parent)
{
    createSockets(2, 1);
    setTitle("Divide");
    setContentText("/");
    setType(OP_DIV);
}

void CalcNodeDiv::operation() {
    if(m_socketsInput[1]->value().toInt() != 0) {
        m_socketOutput[0]->setValue(m_socketsInput[0]->value().toInt() / m_socketsInput[1]->value().toInt());
    }
    else {
        m_socketOutput[0]->reset();
    }
}
