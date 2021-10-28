#ifndef FLOODFILLTOGRADIENTNODE_H
#define FLOODFILLTOGRADIENTNODE_H

#include "node.h"
#include "floodfilltogradient.h"

class FloodFillToGradientNode: public Node
{
    Q_OBJECT
public:
    FloodFillToGradientNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int rotation = 0, float randomizing = 0.0f, int seed = 1);
    ~FloodFillToGradientNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    FloodFillToGradientNode *clone() override;
    void saveTexture(QString fileName) override;
    int rotation();
    void setRotation(int angle);
    float randomizing();
    void setRandomizing(float rand);
    int seed();
    void setSeed(int seed);
public slots:
    void setOutput();
    void updateRotation(int angle);
    void updateRandomizing(qreal rand);
    void updateSeed(int seed);
private:
    FloodFillToGradientObject *preview;
    int m_rotation = 0;
    float m_randomizing = 0.0f;
    int m_seed = 1;
};

#endif // FLOODFILLTOGRADIENTNODE_H
