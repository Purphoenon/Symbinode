#ifndef POLARTRANSFORM_H
#define POLARTRANSFORM_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class PolarTransformObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    PolarTransformObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float radius = 2.0f, bool clamp = false, int angle = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    void saveTexture(QString fileName);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    float radius();
    void setRadius(float radius);
    bool clamp();
    void setClamp(bool clamp);
    int angle();
    void setAngle(int angle);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool polaredTex = false;
    bool resUpdated = false;
    bool bpcUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_radius = 2.0;
    bool m_clamp = false;
    int m_angle = 0;
};

class PolarTransformRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    PolarTransformRenderer(QVector2D res, GLint bpc);
    ~PolarTransformRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void transformToPolar();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int polarFBO;
    unsigned int m_polarTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *polarShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // POLARTRANSFORM_H
