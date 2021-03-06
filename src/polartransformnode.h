#ifndef POLARTRANSFORMNODE_H
#define POLARTRANSFORMNODE_H

#include "node.h"
#include "polartransform.h"

class PolarTransformNode: public Node
{
    Q_OBJECT
public:
    PolarTransformNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float radius = 2.0f, bool clamp = false, int angle = 0);
    ~PolarTransformNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    float radius();
    void setRadius(float radius);
    bool clamp();
    void setClamp(bool clamp);
    int angle();
    void setAngle(int angle);
signals:
    void radiusChanged(float radius);
    void clampChanged(bool clamp);
    void angleChanged(int angle);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateRadius(qreal radius);
    void updateClamp(bool clamp);
    void updateAngle(int angle);
private:
    PolarTransformObject *preview;
    float m_radius = 2.0;
    bool m_clamp = false;
    int m_angle = 0;
};

#endif // POLARTRANSFORMNODE_H
