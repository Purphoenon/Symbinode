#include "calcnodeinput.h"
#include <iostream>

CalcNodeInput::CalcNodeInput(QQuickItem *parent): CalcNode (parent)
{
    createSockets(0, 1);
    setTitle("Input");
    setType(OP_INPUT);
}

void CalcNodeInput::operation() {
    std::cout << "operation" << std::endl;
    m_socketOutput[0]->setValue(grNode->property("inputValue").toInt());
}
