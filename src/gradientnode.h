#ifndef GRADIENTNODE_H
#define GRADIENTNODE_H

#include "node.h"
#include "gradient.h"

struct GradientParams {
    float startX = 0.0f;
    float startY = 0.0f;
    float endX = 1.0f;
    float endY = 1.0f;
    float centerWidth = 0.0f;//for reflected gradient
    bool tiling = false;//for linear and reflected gradients
};

class GradientNode: public Node
{
    Q_OBJECT
public:
    GradientNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GradientParams linear = GradientParams(), GradientParams reflected = GradientParams(), GradientParams angular = GradientParams{0.5f, 0.5f, 1.0f, 0.5f, 0.0f}, GradientParams radial = GradientParams{0.5f, 0.5f, 0.5f, 1.0f, 0.0f}, QString gradientType = "linear");
    ~GradientNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    GradientParams linearParam();
    GradientParams reflectedParam();
    GradientParams angularParam();
    GradientParams radialParam();
    QString gradientType();
    void setGradientType(QString type);
    float startX();
    void setStartX(float x);
    float startY();
    void setStartY(float y);
    float endX();
    void setEndX(float x);
    float endY();
    void setEndY(float y);
    float centerWidth();
    void setCenterWidth(float width);
    bool tiling();
    void setTiling(bool tiling);
signals:
    void gradientTypeChanged(QString type);
    void startXChanged(float x);
    void startYChanged(float y);
    void endXChanged(float x);
    void endYChanged(float y);
    void centerWidthChanged(float width);
    void tilingChanged(bool tiling);
public slots:
    void updateScale(float scale);
    void updateGradientType(QString type);
    void updateStartX(qreal x);
    void updateStartY(qreal y);
    void updateEndX(qreal x);
    void updateEndY(qreal y);
    void updateCenterWidth(qreal width);
    void updateTiling(bool tiling);
    void setOutput();
    void previewGenerated();
private:
    GradientObject *preview;
    QString m_gradientType = "linear";
    GradientParams m_linear;
    GradientParams m_reflected;
    GradientParams m_angular;
    GradientParams m_radial;
};

#endif // GRADIENTNODE_H
