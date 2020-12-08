#ifndef WARP_H
#define WARP_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class WarpObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    WarpObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.1f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int warpTexture();
    void setWarpTexture(unsigned int texture);
    float intensity();
    void setIntensity(float intensity);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool warpedTex = false;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 0.1f;
};

class WarpRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    WarpRenderer(QVector2D res);
    ~WarpRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createWarp();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_warpedTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int warpFBO = 0;
    QOpenGLShaderProgram *warpShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // WARP_H
