#ifndef DIRECTIONALBLURNODE_H
#define DIRECTIONALBLURNODE_H

#include "node.h"
#include "directionalblur.h"

class DirectionalBlurNode: public Node
{
    Q_OBJECT
public:
    DirectionalBlurNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 3.75f, int angle = 0);
    ~DirectionalBlurNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    float intensity();
    void setIntensity(float intensity);
    int angle();
    void setAngle(int angle);
signals:
    void intensityChanged(float intensity);
    void angleChanged(int angle);
public slots:
    void updateScale(float scale);
    void updateIntensity(qreal intensity);
    void updateAngle(int angle);
    void setOutput();
private:
    DirectionalBlurObject *preview;
    float m_intensity = 3.75f;
    int m_angle = 0;
};

#endif // DIRECTIONALBLURNODE_H
