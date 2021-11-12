#ifndef CURVATURENODE_H
#define CURVATURENODE_H

#include "node.h"
#include "curvature.h"

class CurvatureNode: public Node
{
    Q_OBJECT
public:
    CurvatureNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float intensity = 1.0f, int offset = 1);
    ~CurvatureNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    CurvatureNode *clone() override;
    void saveTexture(QString fileName) override;
    float intensity();
    void setIntensity(float intensity);
    int offset();
    void setOffset(int offset);
public slots:
    void setOutput();
    void updateIntensity(qreal intensity);
    void updateOffset(int offset);
private:
    CurvatureObject *preview;
    float m_intensity = 1.0f;
    int m_offset = 1;
};

#endif // CURVATURENODE_H
