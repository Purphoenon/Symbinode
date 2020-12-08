#ifndef CALCNODE_H
#define CALCNODE_H
#include "node.h"

enum CalcOpType {OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_INPUT, OP_OUTPUT};

class CalcNode: public Node
{
    Q_OBJECT
public:
    CalcNode(QQuickItem *parent = nullptr);
    void setType(CalcOpType type);
    CalcOpType getType();
    void serialize(QJsonObject &json) const;
private:
    CalcOpType m_type = OP_ADD;
};

#endif // CALCNODE_H
