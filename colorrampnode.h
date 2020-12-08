#ifndef COLORRAMPNODE_H
#define COLORRAMPNODE_H

#include "node.h"
#include "colorramp.h"

class ColorRampNode: public Node
{
    Q_OBJECT
public:
    ColorRampNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QJsonArray stops = {QJsonArray{1, 1, 1, 1}, QJsonArray{0, 0, 0, 0}});
    ~ColorRampNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    QJsonArray stops() const;
signals:
    void stopsChanged(QVariant gradients);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
private:
    ColorRampObject *preview;
};

#endif // COLORRAMPNODE_H
