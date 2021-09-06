#ifndef BEVEL_H
#define BEVEL_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BevelObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    BevelObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, float distance = -0.5f, float smooth = 0.0f, bool useAlpha = false);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    void saveTexture(QString fileName);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float distance();
    void setDistance(float dist);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool beveledTex = false;
    bool resUpdated = false;
    bool bpcUpdated = false;
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
    float m_dist = -0.5f;
    float m_smooth = 0.0f;
    bool m_alpha = false;
};

class BevelRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    BevelRenderer(QVector2D res, GLint bpc);
    ~BevelRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void jumpFlooding();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int m_sourceTexture = 0;
    unsigned int m_initTexture;
    unsigned int m_jfaTexture[2];
    unsigned int m_blurTexture[2];
    unsigned int m_bevelTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int FBO = 0;
    unsigned int jfaFBO[2];
    unsigned int blurFBO[2];
    unsigned int bevelFBO = 0;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *bevelShader;
    QOpenGLShaderProgram *preparationShader;
    QOpenGLShaderProgram *jfaShader;
    QOpenGLShaderProgram *blurShader;
};

#endif // BEVEL_H
