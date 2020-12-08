#ifndef NORMALMAPNODE_H
#define NORMALMAPNODE_H

#include "node.h"
#include "normalmap.h"

class NormalMapNode: public Node
{
    Q_OBJECT
public:
    NormalMapNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float strenght = 6.0f);
    ~NormalMapNode();
    void operation();
    float strenght();
    void setOutput();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
public slots:
    void updateStrenght(qreal strenght);
    void updatePrev(bool sel);
    void previewGenerated();
    void updateScale(float scale);
signals:
    void strenghtChanged(float strenght);
private:
    NormalMapObject *preview;
    float m_strenght = 6.0f;
};

#endif // NORMALMAPNODE_H
