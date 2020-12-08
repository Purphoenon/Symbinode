#ifndef COLORINGNODE_H
#define COLORINGNODE_H

#include "node.h"
#include "coloring.h"

class ColoringNode: public Node
{
    Q_OBJECT
public:
    ColoringNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QVector3D color = QVector3D(1, 1, 1));
    ~ColoringNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    QVector3D color();
    void setColor(QVector3D color);
signals:
    void colorChanged(QVector3D color);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateColor(QVector3D color);
private:
    ColoringObject *preview;
    QVector3D m_color = QVector3D(1, 1, 1);
};

#endif // COLORINGNODE_H
