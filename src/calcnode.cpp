#include "calcnode.h"
#include <iostream>

CalcNode::CalcNode(QQuickItem *parent): Node(parent)
{
    setHeight(105);
}

void CalcNode::setType(CalcOpType type) {
    m_type = type;
}

CalcOpType CalcNode::getType() {
    return m_type;
}

void CalcNode::serialize(QJsonObject &json) const {
    std::cout << "calc node save" << std::endl;
    Node::serialize(json);
    json["type"] = m_type;
    if(m_type == OP_INPUT) {
        json["inputText"] = grNode->property("inputValue").toJsonValue();
    }
}
