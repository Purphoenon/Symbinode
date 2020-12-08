#ifndef BRIGHTNESSCONTRASTNODE_H
#define BRIGHTNESSCONTRASTNODE_H

#include "node.h"
#include "brightnesscontrast.h"

class BrightnessContrastNode: public Node
{
    Q_OBJECT
public:
    BrightnessContrastNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float brightness = 0.0f, float contrast = 0.0f);
    ~BrightnessContrastNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    float brightness();
    void setBrightness(float value);
    float contrast();
    void setContrast(float value);
signals:
    void brightnessChanged(float value);
    void contrastChanged(float value);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateBrightness(qreal value);
    void updateContrast(qreal value);
private:
    BrightnessContrastObject *preview;
    float m_brightness = 0.0f;
    float m_contrast = 0.0f;
};

#endif // BRIGHTNESSCONTRASTNODE_H
