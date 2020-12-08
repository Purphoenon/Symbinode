#ifndef COLORNODE_H
#define COLORNODE_H

#include "node.h"
#include "color.h"

class ColorNode: public Node
{
    Q_OBJECT
public:
    ColorNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QVector3D color = QVector3D(1, 1, 1));
    ~ColorNode();
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
    void updateColor(QVector3D color);
    void previewGenerated();
private:
    ColorObject *preview;
    QVector3D m_color = QVector3D(1, 1, 1);
};

#endif // COLORNODE_H
