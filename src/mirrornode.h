#ifndef MIRRORNODE_H
#define MIRRORNODE_H
#include "node.h"
#include "mirror.h"

class MirrorNode: public Node
{
    Q_OBJECT
public:
    MirrorNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int dir = 0);
    ~MirrorNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    int direction();
    void setDirection(int dir);
signals:
    void directionChanged(int dir);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateDirection(int dir);
private:
    MirrorObject *preview;
    int m_direction = 0;
};

#endif // MIRRORNODE_H
