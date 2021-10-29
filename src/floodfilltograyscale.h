#ifndef FLOODFILLTOGRAYSCALE_H
#define FLOODFILLTOGRAYSCALE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class FloodFillToGrayscaleObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    FloodFillToGrayscaleObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int seed = 1);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    int seed();
    void setSeed(int seed);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool toGrayscaleTex = false;
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
    int m_seed = 1;
};

class FloodFillToGrayscaleRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    FloodFillToGrayscaleRenderer(QVector2D resolution, GLint bpc);
    ~FloodFillToGrayscaleRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void toGrayscale();
    void createRandom();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int textureVAO = 0;
    unsigned int FBO = 0;
    unsigned int randomFBO = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_grayscaleTexture = 0;
    unsigned int m_randomTexture = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *grayscaleShader;
    QOpenGLShaderProgram *randomShader;
};

#endif // FLOODFILLTOGRAYSCALE_H
