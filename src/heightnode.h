#ifndef HEIGHTNODE_H
#define HEIGHTNODE_H

#include "node.h"
#include "onechanel.h"
#include "normal.h"

class HeightNode: public Node
{
    Q_OBJECT
public:
    HeightNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16);
    ~HeightNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
public slots:
    void heightSave(QString dir);
signals:
    void heightChanged(unsigned int heightMap);
private:
    NormalObject *preview;
};

#endif // HEIGHTNODE_H
