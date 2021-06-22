#ifndef DIRECTIONALWARP_H
#define DIRECTIONALWARP_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class DirectionalWarpObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    DirectionalWarpObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float intensity = 0.1f, int angle = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int warpTexture();
    void setWarpTexture(unsigned int texture);
    void saveTexture(QString fileName);
    float intensity();
    void setIntensity(float intensity);
    int angle();
    void setAngle(int angle);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool warpedTex = false;
    bool resUpdated = false;
    bool bpcUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void changedTexture();
private:
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 0.1f;
    int m_angle = 0;
};

class DirectionalWarpRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    DirectionalWarpRenderer(QVector2D res, GLint bpc);
    ~DirectionalWarpRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createDirectionalWarp();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_warpedTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int warpFBO = 0;
    QOpenGLShaderProgram *dirWarpShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // DIRECTIONALWARP_H
