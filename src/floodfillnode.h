#ifndef FLOODFILLNODE_H
#define FLOODFILLNODE_H

#include "node.h"
#include "floodfill.h"

class FloodFillNode: public Node
{
    Q_OBJECT
public:
    FloodFillNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16);
    ~FloodFillNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    FloodFillNode *clone() override;
    void saveTexture(QString fileName) override;
public slots:
    void setOutput();
private:
    FloodFillObject *preview;
};

#endif // FLOODFILLNODE_H
