#ifndef EMISSIONNODE_H
#define EMISSIONNODE_H

#include "node.h"
#include "normal.h"

class EmissionNode: public Node
{
    Q_OBJECT
public:
    EmissionNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~EmissionNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
public slots:
    void updateScale(float scale);
    void emissionSave(QString dir);
signals:
    void emissionChanged(unsigned int emission);
private:
    NormalObject *preview;
};

#endif // EMISSIONNODE_H
