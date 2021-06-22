#ifndef GRAYSCALENODE_H
#define GRAYSCALENODE_H

#include "node.h"
#include "grayscale.h"

class GrayscaleNode: public Node
{
    Q_OBJECT
public:
    GrayscaleNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8);
    ~GrayscaleNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
public slots:
    void updateScale(float scale);
    void previewGenerated();
    void setOutput();
private:
    GrayscaleObject *preview;
};

#endif // GRAYSCALENODE_H
