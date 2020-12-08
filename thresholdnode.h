#ifndef THRESHOLDNODE_H
#define THRESHOLDNODE_H

#include "node.h"
#include "threshold.h"

class ThresholdNode: public Node
{
    Q_OBJECT
public:
    ThresholdNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float threshold = 0.5f);
    ~ThresholdNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    float threshold();
    void setThreshold(float value);
signals:
    void thresholdChanged(float value);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateThreshold(qreal value);
private:
    ThresholdObject *preview;
    float m_threshold = 0.5f;
};

#endif // THRESHOLDNODE_H
