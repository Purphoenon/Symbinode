#ifndef COLORING_H
#define COLORING_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class ColoringObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ColoringObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QVector3D color = QVector3D(1, 1, 1));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    QVector3D color();
    void setColor(QVector3D color);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool colorizedTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_texture = 0;
    QVector3D m_color = QVector3D(1, 1, 1);
};

class ColoringRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    ColoringRenderer(QVector2D res);
    ~ColoringRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void colorize();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int colorFBO;
    unsigned int m_colorTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *coloringShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // COLORING_H
