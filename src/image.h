#ifndef IMAGE_H
#define IMAGE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class ImageObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ImageObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float transX = 0.0f, float transY = 0.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    void loadTexture(QString file);
    float translateX();
    void setTranslateX(float x);
    float translateY();
    void setTranslateY(float y);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool loadTex = false;
    bool layerUpdate = false;
    bool resUpdate = false;
    QString texName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    QVector2D m_resolution;
    float m_translateX = 0.0f;
    float m_translateY = 0.0f;
};

class ImageRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    ImageRenderer(QVector2D resolution);
    ~ImageRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void loadTexture(QString file);
    void createLayer();
    void updateTexResolution();
    QVector2D m_resolution;
    bool isTexture = false;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int FBO = 0;
    unsigned int m_texture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *layerShader;
};

#endif // IMAGE_H
