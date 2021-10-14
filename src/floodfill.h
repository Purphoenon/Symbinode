#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include <QTime>
#include "FreeImage.h"

class FloodFillObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    FloodFillObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool filledTex = false;
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
};

class FloodFillRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    FloodFillRenderer(QVector2D resolution, GLint bpc);
    ~FloodFillRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void filling();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int resultFBO = 0;
    unsigned int FBO[2];
    unsigned int preFBO = 0;
    unsigned int fillFBO[2];
    unsigned int m_floodFillTexture = 0;
    unsigned int m_maskFillTexture[2];
    unsigned int m_fillTexture[2];
    unsigned int m_prefloodFillTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *preparationShader;
    QOpenGLShaderProgram *fillShader;
    QOpenGLShaderProgram *finalFillShader;
    QOpenGLShaderProgram *maskShader;
};

#endif // FLOODFILL_H
