#ifndef ROUGHNODE_H
#define ROUGHNODE_H

#include "node.h"
#include "onechanel.h"

class RoughNode: public Node
{
    Q_OBJECT
public:
    RoughNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~RoughNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
public slots:
    void updateRough(qreal rough);
    void updatePrev(bool sel);
    void updateScale(float scale);
    void saveRough(QString dir);
signals:
    void roughChanged(QVariant rough, bool useTexture);
private:
    OneChanelObject *preview;
    float m_rough = 0.2f;
};

#endif // ROUGHNODE_H
