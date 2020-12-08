#ifndef METALNODE_H
#define METALNODE_H

#include "node.h"
#include "onechanel.h"

class MetalNode: public Node
{
    Q_OBJECT
public:
    MetalNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~MetalNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
public slots:
    void updateMetal(qreal metal);
    void updatePrev(bool sel);
    void updateScale(float scale);
    void saveMetal(QString dir);
signals:
    void metalChanged(QVariant metal, bool useTexture);
private:
    OneChanelObject *preview;
    float m_metal = 0.0f;
};

#endif // METALNODE_H
