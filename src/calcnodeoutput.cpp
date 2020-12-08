#include "calcnodeoutput.h"
#include <iostream>

CalcNodeOutput::CalcNodeOutput(QQuickItem *parent): CalcNode (parent)
{
    createSockets(1, 0);
    setTitle("Output");
    setType(OP_OUTPUT);
}

void CalcNodeOutput::operation() {
    setContentText(m_socketsInput[0]->value().toString());
    std::cout << "operation out" << std::endl;
}
