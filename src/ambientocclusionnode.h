#ifndef AMBIENTOCCLUSIONNODE_H
#define AMBIENTOCCLUSIONNODE_H

#include "node.h"
#include "ambientocclusion.h"

class AmbientOcclusionNode: public Node
{
    Q_OBJECT
public:
    AmbientOcclusionNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float radius = 0.1f, int samples = 8, float strength = 0.3f, float smooth = 0.2f);
    ~AmbientOcclusionNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    AmbientOcclusionNode *clone() override;
    void saveTexture(QString fileName) override;
    float radius();
    void setRadius(float radius);
    int samples();
    void setSamples(int samples);
    float strength();
    void setStrength(float strength);
    float ambientSmooth();
    void setSmooth(float smooth);
public slots:
    void setOutput();
    void updateRadius(qreal radius);
    void updateSamples(int samples);
    void updateStrength(qreal strength);
    void updateSmooth(qreal smooth);
private:
    AmbientOcclusionObject *preview;
    float m_radius = 0.1f;
    int m_samples = 8;
    float m_strength = 0.3f;
    float m_smooth = 0.2f;
};

#endif // AMBIENTOCCLUSIONNODE_H
