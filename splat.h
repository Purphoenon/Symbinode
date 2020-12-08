#ifndef SPLAT_H
#define SPLAT_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include <QQuickWindow>

class SplatObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    SplatObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int size = 2, int imagePerCell = 3);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    int size();
    void setSize(int size);
    int imagePerCell();
    void setImagePerCell(int count);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool bombedTex = false;
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
    int m_size = 2;
    int m_imagePerSell = 3;
};

class SplatRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    SplatRenderer(QVector2D res);
    ~SplatRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void bomb();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int bombingFBO;
    unsigned int randomFBO;
    unsigned int m_bombingTexture = 0;
    unsigned int m_randomTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *bombingShader;
    QOpenGLShaderProgram *randomShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // SPLAT_H
