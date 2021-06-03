#ifndef SLOPEBLUR_H
#define SLOPEBLUR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class SlopeBlurObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    SlopeBlurObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int mode = 0, float intensity = 0.5f, int samples = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int slopeTexture();
    void setSlopeTexture(unsigned int texture);
    void saveTexture(QString fileName);
    int mode();
    void setMode(int mode);
    float intensity();
    void setIntensity(float intensity);
    int samples();
    void setSamples(int samples);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool slopedTex = false;
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
    unsigned int m_slopeTexture = 0;
    unsigned int m_maskTexture = 0;
    int m_mode = 0;
    float m_intensity = 0.5f;
    int m_samples = 1;
};

class SlopeBlurRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    SlopeBlurRenderer(QVector2D res);
    ~SlopeBlurRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createSlopeBlur();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_slopeTexture = 0;
    unsigned int m_slopedTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int slopeFBO = 0;
    QOpenGLShaderProgram *slopeBlurShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // SLOPEBLUR_H
