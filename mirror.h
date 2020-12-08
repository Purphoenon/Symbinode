#ifndef MIRROR_H
#define MIRROR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class MirrorObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MirrorObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int dir = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    int direction();
    void setDirection(int dir);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool mirroredTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    int m_direction = 0;
};

class MirrorRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    MirrorRenderer(QVector2D res);
    ~MirrorRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void mirror();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int mirrorFBO;
    unsigned int m_mirrorTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *mirrorShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // MIRROR_H
