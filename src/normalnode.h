#ifndef NORMALNODE_H
#define NORMALNODE_H

#include "node.h"
#include "normal.h"

class NormalNode: public Node
{
     Q_OBJECT
public:
    NormalNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~NormalNode();
    void operation();
    void serialize(QJsonObject &json) const;
public slots:
    void updatePrev(bool sel);
    void updateScale(float scale);
    void saveNormal(QString dir);
signals:
    void normalChanged(unsigned int normalMap);
private:
    NormalObject *preview;
};

#endif // NORMALNODE_H
