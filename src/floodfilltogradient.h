#ifndef FLOODFILLTOGRADIENT_H
#define FLOODFILLTOGRADIENT_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class FloodFillToGradientObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    FloodFillToGradientObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int rotation = 0, float randomizing = 0.0f, int seed = 1);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    int rotation();
    void setRotation(int angle);
    float randomizing();
    void setRandomizing(float rand);
    int seed();
    void setSeed(int seed);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool toGradientTex = false;
    bool bpcUpdated = false;
    bool randUpdate = false;
    bool resUpdate = false;
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
    int m_rotation = 0;
    float m_randomizing = 0.0f;
    int m_seed = 1;
};

class FloodFillToGradientRenderer: public QQuickFramebufferObject::Renderer,
                                   public QOpenGLFunctions_4_4_Core
{
public:
    FloodFillToGradientRenderer(QVector2D resolution, GLint bpc);
    ~FloodFillToGradientRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void toGradient();
    void createRandom();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int textureVAO = 0;
    unsigned int FBO = 0;
    unsigned int randomFBO = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_gradientTexture = 0;
    unsigned int m_randomTexture = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *gradientShader;
    QOpenGLShaderProgram *randomShader;
};

#endif // FLOODFILLTOGRADIENT_H
