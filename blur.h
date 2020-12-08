#ifndef BLUR_H
#define BLUR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BlurObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    BlurObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.5f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float intensity();
    void setIntensity(float intensity);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool bluredTex = false;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 0.5f;
};

class BlurRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    BlurRenderer(QVector2D res);
    ~BlurRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createBlur();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *blurShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // BLUR_H
