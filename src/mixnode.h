#ifndef MIXNODE_H
#define MIXNODE_H
#include "node.h"
#include "mix.h"

class MixNode: public Node
{
    Q_OBJECT
public:
    MixNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float factor = 0.5f, int mode = 0);
    ~MixNode();
    void operation();
    float factor();
    void setFactor(float f);
    int mode();
    void setMode(int mode);
    void setOutput();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
signals:
    void factorChanged(float f);
    void modeChanged(int mode);
public slots:
    void updatePrev(bool sel);
    void previewGenerated();
    void updateFactor(qreal f);
    void updateMode(int mode);
    void updateScale(float scale);
private:
    MixObject *preview = nullptr;
    float m_factor = 0.5;
    int m_mode = 0;
};

#endif // MIXNODE_H
