#ifndef AMBIENTOCCLUSION_H
#define AMBIENTOCCLUSION_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class AmbientOcclusionObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    AmbientOcclusionObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float radius = 0.1f, int samples = 8, float strength = 0.3f, float smooth = 0.2f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    void saveTexture(QString fileName);
    float radius();
    void setRadius(float radius);
    int samples();
    void setSamples(int samples);
    float strength();
    void setStrength(float strength);
    float ambientSmooth();
    void setSmooth(float smooth);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool occlusionTex = false;
    bool bluredTex = false;
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
    float m_radius = 0.1f;
    int m_samples = 8;
    float m_strength = 0.3f;
    float m_smooth = 0.2f;
};

class AmbientOcclusionRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    AmbientOcclusionRenderer(QVector2D resolution, GLint bpc);
    ~AmbientOcclusionRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void ambientOcclusion();
    void blur();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int m_texture = 0;
    unsigned int m_ambientTexture = 0;
    unsigned int m_randomTexture = 0;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int FBO = 0;
    unsigned int ambientFBO = 0;
    unsigned int randomFBO = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *ambientShader;
    QOpenGLShaderProgram *randomShader;
    QOpenGLShaderProgram *blurShader;
    QOpenGLShaderProgram *checkerShader;
};

#endif // AMBIENTOCCLUSION_H
