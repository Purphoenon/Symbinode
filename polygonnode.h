#ifndef POLYGONNODE_H
#define POLYGONNODE_H

#include "node.h"
#include "polygon.h"

class PolygonNode: public Node
{
    Q_OBJECT
public:
    PolygonNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int sides = 3, float polygonScale = 0.4f, float smooth = 0.0f);
    ~PolygonNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    int sides();
    void setSides(int sides);
    float polygonScale();
    void setPolygonScale(float scale);
    float smooth();
    void setSmooth(float smooth);
signals:
    void sidesChanged(int sides);
    void polygonScaleChanged(float scale);
    void smoothChanged(float smooth);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void previewGenerated();
    void updateSides(int sides);
    void updatePolygonScale(qreal scale);
    void updateSmooth(qreal smooth);
private:
    PolygonObject *preview;
    int m_sides = 3;
    float m_polygonScale = 0.4f;
    float m_smooth = 0.0f;
};

#endif // POLYGONNODE_H
