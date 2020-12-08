#ifndef ALBEDONODE_H
#define ALBEDONODE_H
#include "node.h"
#include "albedo.h"

class AlbedoNode: public Node
{
    Q_OBJECT
public:
    AlbedoNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~AlbedoNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
signals:
    void albedoChanged(QVariant albedo, bool useTexture);
public slots:
    void updateAlbedo(QVector3D color);
    void updatePrev(bool sel);
    void updateScale(float scale);
    void saveAlbedo(QString dir);
private:
    AlbedoObject *preview;
    QVector3D m_albedo = QVector3D(1.0f, 1.0f, 1.0f);
};

#endif // ALBEDONODE_H
