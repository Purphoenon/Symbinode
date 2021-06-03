#ifndef HEIGHTNODE_H
#define HEIGHTNODE_H

#include "node.h"
#include "onechanel.h"
#include "normal.h"

class HeightNode: public Node
{
    Q_OBJECT
public:
    HeightNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~HeightNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
public slots:
    void updateScale(float scale);
    void heightSave(QString dir);
signals:
    void heightChanged(unsigned int heightMap);
private:
    NormalObject *preview;
};

#endif // HEIGHTNODE_H
