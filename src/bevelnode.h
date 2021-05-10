#ifndef BEVELNODE_H
#define BEVELNODE_H

#include "node.h"
#include "bevel.h"

class BevelNode: public Node
{
    Q_OBJECT
public:
    BevelNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float distance = -0.5f, float smooth = 0.0f, bool useAlpha = false);
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    float distance();
    void setDistance(float dist);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
signals:
    void distanceChanged(float dist);
    void smoothChanged(float smooth);
    void useAlphaChanged(bool use);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateDistance(qreal dist);
    void updateSmooth(qreal smooth);
    void updateUseAlpha(bool use);
private:
    BevelObject *preview;
    float m_dist = -0.5f;
    float m_smooth = 0.0f;
    bool m_alpha = false;
};

#endif // BEVELNODE_H
