#ifndef EMISSIONNODE_H
#define EMISSIONNODE_H

#include "node.h"
#include "normal.h"

class EmissionNode: public Node
{
    Q_OBJECT
public:
    EmissionNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16);
    ~EmissionNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
public slots:
    void updateScale(float scale);
    void emissionSave(QString dir);
signals:
    void emissionChanged(unsigned int emission);
private:
    NormalObject *preview;
};

#endif // EMISSIONNODE_H
