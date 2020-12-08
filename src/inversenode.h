#ifndef INVERSENODE_H
#define INVERSENODE_H

#include "node.h"
#include "inverse.h"

class InverseNode: public Node
{
    Q_OBJECT
public:
    InverseNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~InverseNode();
    void operation();
    void serialize(QJsonObject &json) const;
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
private:
    InverseObject *preview;
};

#endif // INVERSENODE_H
