#ifndef BLURNODE_H
#define BLURNODE_H

#include "node.h"
#include "blur.h"

class BlurNode: public Node
{
    Q_OBJECT
public:
    BlurNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.5f);
    ~BlurNode();
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
    BlurObject *preview;
    float m_intensity = 0.5f;
};

#endif // BLURNODE_H
