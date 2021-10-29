#ifndef FLOODFILLTORANDOMGRAYSCALENODE_H
#define FLOODFILLTORANDOMGRAYSCALENODE_H

#include "node.h"
#include "floodfilltograyscale.h"

class FloodFillToGrayscaleNode: public Node
{
    Q_OBJECT
public:
    FloodFillToGrayscaleNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int seed = 1);
    ~FloodFillToGrayscaleNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    FloodFillToGrayscaleNode *clone() override;
    void saveTexture(QString fileName) override;
    int seed();
    void setSeed(int seed);
public slots:
    void setOutput();
    void updateSeed(int seed);
private:
    FloodFillToGrayscaleObject *preview;
    int m_seed = 1;
};

#endif // FLOODFILLTORANDOMGRAYSCALENODE_H
