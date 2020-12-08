#ifndef WARPNODE_H
#define WARPNODE_H

#include "node.h"
#include "warp.h"

class WarpNode: public Node
{
    Q_OBJECT
public:
    WarpNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.1f);
    ~WarpNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    float intensity();
    void setIntensity(float intensity);
signals:
    void intensityChanged(float intensity);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateIntensity(qreal intensity);
private:
    WarpObject *preview;
    float m_intensity = 0.1f;
};

#endif // WARPNODE_H
