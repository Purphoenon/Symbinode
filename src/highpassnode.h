#ifndef HIGHPASSNODE_H
#define HIGHPASSNODE_H

#include "node.h"
#include "highpass.h"

class HighPassNode: public Node
{
    Q_OBJECT
public:
    HighPassNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float radius = 1.0f, float contrast = 0.0f);
    ~HighPassNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    HighPassNode *clone() override;
    void saveTexture(QString fileName) override;
    float radius();
    void setRadius(float radius);
    float contrast();
    void setContrast(float contrast);
public slots:
    void setOutput();
    void updateRadius(qreal radius);
    void updateContrast(qreal contrast);
private:
    HighPassObject *preview;
    float m_radius = 1.0f;
    float m_contrast = 0.0f;
};

#endif // HIGHPASSNODE_H
