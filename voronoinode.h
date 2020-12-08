#ifndef VORONOINODE_H
#define VORONOINODE_H

#include "node.h"
#include "voronoi.h"

struct VoronoiParams {
    float jitter = 1.0f;
    float intensity = 1.0f;
    float borders = 0.0f;
    int scale = 5;
    int scaleX = 1;
    int scaleY = 1;
    bool inverse = false;
};

class VoronoiNode: public Node
{
    Q_OBJECT
public:
    VoronoiNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024),
                VoronoiParams crystals = VoronoiParams(), VoronoiParams borders = VoronoiParams(),
                VoronoiParams solid = VoronoiParams(), VoronoiParams worley = VoronoiParams(),
                QString voronoiType = "crystals");
    ~VoronoiNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    VoronoiParams crystalsParam();
    VoronoiParams bordersParam();
    VoronoiParams solidParam();
    VoronoiParams worleyParam();
    QString voronoiType();
    void setVoronoiType(QString type);
    int voronoiScale();
    void setVoronoiScale(int scale);
    int scaleX();
    void setScaleX(int scale);
    int scaleY();
    void setScaleY(int scale);
    float jitter();
    void setJitter(float jitter);
    bool inverse();
    void setInverse(bool inverse);
    float intensity();
    void setIntensity(float intensity);
    float bordersSize();
    void setBordersSize(float size);
signals:
    void voronoiTypeChanged(QString type);
    void voronoiScaleChanged(int scale);
    void scaleXChanged(int scale);
    void scaleYChanged(int scale);
    void jitterChanged(float jitter);
    void inverseChanged(bool inverse);
    void intensityChanged(float intensity);
    void bordersSizeChanged(float size);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void previewGenerated();
    void updateVoronoiType(QString type);
    void updateVoronoiScale(int scale);
    void updateScaleX(int scale);
    void updateScaleY(int scale);
    void updateJitter(qreal jitter);
    void updateInverse(bool inverse);
    void updateIntensity(qreal intensity);
    void updateBordersSize(qreal size);
private:
    VoronoiObject *preview;
    QString m_voronoiType = "crystals";
    VoronoiParams m_crystals;
    VoronoiParams m_borders;
    VoronoiParams m_solid;
    VoronoiParams m_worley;
};

#endif // VORONOINODE_H
