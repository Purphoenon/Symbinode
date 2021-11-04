#ifndef HIGHPASS_H
#define HIGHPASS_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>


class HighPassObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    HighPassObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float radius = 1.0f, float contrast = 0.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    float radius();
    void setRadius(float radius);
    float contrast();
    void setContrast(float contrast);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool contrastTex = false;
    bool highPassTex = false;
    bool bpcUpdated = false;
    bool resUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_radius = 1.0f;
    float m_contrast = 0.0f;
};

class HighPassRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    HighPassRenderer(QVector2D resolution, GLint bpc);
    ~HighPassRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void highPass();
    void blur();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int FBO = 0;
    unsigned int highpassTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *blurShader;
    QOpenGLShaderProgram *highpassShader;
    QOpenGLShaderProgram *checkerShader;
};

#endif // HIGHPASS_H
