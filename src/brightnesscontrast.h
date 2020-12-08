#ifndef BRIGHTNESSCONTRAST_H
#define BRIGHTNESSCONTRAST_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BrightnessContrastObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    BrightnessContrastObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float brightness = 0.0f, float contrast = 0.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float brightness();
    void setBrightness(float value);
    float contrast();
    void setContrast(float value);
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
    unsigned int m_sourceTexture = 0;
    unsigned int m_texture = 0;
    float m_brightness = 0.0f;
    float m_contrast = 0.0f;
};

class BrightnessContrastRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    BrightnessContrastRenderer(QVector2D res);
    ~BrightnessContrastRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void create();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int brightnessContrastFBO;
    unsigned int m_brightnessContrastTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *brightnessContrastShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // BRIGHTNESSCONTRAST_H
