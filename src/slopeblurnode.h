#ifndef SLOPEBLURNODE_H
#define SLOPEBLURNODE_H

#include "node.h"
#include "slopeblur.h"

class SlopeBlurNode: public Node
{
    Q_OBJECT
public:
    SlopeBlurNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int mode = 0, float intensity = 0.5f, int samples = 1);
    ~SlopeBlurNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    SlopeBlurNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    int mode();
    void setMode(int mode);
    float intensity();
    void setIntensity(float intensity);
    int samples();
    void setSamples(int samples);
signals:
    void modeChanged(int mode);
    void intensityChanged(float intensity);
    void samplesChanged(int samples);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateMode(int mode);
    void updateIntensity(qreal intensity);
    void updateSamples(int samples);
private:
    SlopeBlurObject *preview;
    int m_mode = 0;
    float m_intensity = 0.5f;
    int m_samples = 1;
};

#endif // SLOPEBLURNODE_H
