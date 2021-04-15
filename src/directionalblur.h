#ifndef DIRECTIONALBLUR_H
#define DIRECTIONALBLUR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class DirectionalBlurObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    DirectionalBlurObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 3.75f, int angle = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    float intensity();
    void setIntensity(float intensity);
    int angle();
    void setAngle(int angle);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool bluredTex = false;
    bool resUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 3.75f;
    int m_angle = 0;
};

class DirectionalBlurRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    DirectionalBlurRenderer(QVector2D res);
    ~DirectionalBlurRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createDirectionalBlur();
    void updateTexResolution();
    void saveTexture(QString fileName);
    float m_intensity = 3.75f;
    QVector2D m_resolution;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *dirBlurShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // DIRECTIONALBLUR_H
