#ifndef BACKGROUNDOBJECT_H
#define BACKGROUNDOBJECT_H
#include <QQuickFramebufferObject>

class BackgroundObject: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(float viewScale READ viewScale WRITE setViewScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector2D viewPan READ viewPan WRITE setViewPan NOTIFY panChanged)
public:
    BackgroundObject(QQuickItem *parent = nullptr);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    float viewScale();
    void setViewScale(float scale);
    QVector2D viewPan();
    void setViewPan(QVector2D pan);
    bool isPan = false;
    bool isScaled = false;
signals:
    void panChanged(QVector2D pan);
    void scaleChanged(float scale);
private:
    float offsetX = 0, offsetY = 0;
    float lastX, lastY;
    float scaleStep = 0.0f;
    float m_scale = 1.0f;
    QVector2D m_pan = QVector2D(0.0f, 0.0f);
};

#endif // BACKGROUNDOBJECT_H
