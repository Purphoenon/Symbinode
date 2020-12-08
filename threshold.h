#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class ThresholdObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ThresholdObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float threshold = 0.5f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float threshold();
    void setThreshold(float value);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool created = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_threshold = 0.5f;
};

class ThresholdRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    ThresholdRenderer(QVector2D res);
    ~ThresholdRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void create();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int thresholdFBO;
    unsigned int m_thresholdTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *thresholdShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // THRESHOLD_H
