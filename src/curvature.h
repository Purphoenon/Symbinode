#ifndef CURVATURE_H
#define CURVATURE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class CurvatureObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    CurvatureObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float intensity = 1.0f, int offset = 1);
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
    int offset();
    void setOffset(int offset);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool curvatureTex = false;
    bool bpcUpdated = false;
    bool resUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    float m_intensity = 1.0f;
    int m_offset = 1;
};

class CurvatureRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    CurvatureRenderer(QVector2D resolution, GLint bpc);
    ~CurvatureRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void curvature();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int m_curvatureTexture = 0;
    unsigned int FBO = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *curvatureShader;
    QOpenGLShaderProgram *checkerShader;
};

#endif // CURVATURE_H
