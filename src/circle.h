#ifndef CIRCLE_H
#define CIRCLE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class CircleObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    CircleObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int interpolation = 1, float radius = 0.5f, float smooth = 0.01f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    int interpolation();
    void setInterpolation(int interpolation);
    float radius();
    void setRadius(float radius);
    float smooth();
    void setSmooth(float smooth);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedCircle = true;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture;
    unsigned int m_maskTexture = 0;
    int m_interpolation = 1;
    float m_radius = 0.5f;
    float m_smooth = 0.01f;
};

class CircleRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    CircleRenderer(QVector2D resolution);
    ~CircleRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createCircle();
    void updateTexResolution();
    QOpenGLShaderProgram *generateCircle;
    QOpenGLShaderProgram *renderTexture;
    unsigned int circleFBO;
    unsigned int circleVAO, textureVAO;
    unsigned int circleTexture;
    unsigned int maskTexture = 0;
    QVector2D m_resolution;
};

#endif // CIRCLE_H
