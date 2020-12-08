#ifndef MAPPINGNODE_H
#define MAPPINGNODE_H

#include "node.h"
#include "mapping.h"

class MappingNode: public Node
{
    Q_OBJECT
public:
    MappingNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float inputMin = 0.0f, float inputMax = 1.0f, float outputMin = 0.0f, float outputMax = 1.0f);
    ~MappingNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    float inputMin();
    void setInputMin(float value);
    float inputMax();
    void setInputMax(float value);
    float outputMin();
    void setOutputMin(float value);
    float outputMax();
    void setOutputMax(float value);
signals:
    void inputMinChanged(float value);
    void inputMaxChanged(float value);
    void outputMinChanged(float value);
    void outputMaxChanged(float value);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateInputMin(qreal value);
    void updateInputMax(qreal value);
    void updateOutputMin(qreal value);
    void updateOutputMax(qreal value);
private:
    MappingObject *preview;
    float m_inputMin = 0.0f;
    float m_inputMax = 1.0f;
    float m_outputMin = 0.0f;
    float m_outputMax = 1.0f;
};

#endif // MAPPINGNODE_H
