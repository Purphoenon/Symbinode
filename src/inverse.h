#ifndef INVERSE_H
#define INVERSE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class InverseObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    InverseObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool inversedTex = false;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
};

class InverseRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    InverseRenderer(QVector2D res);
    ~InverseRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void inverte();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int inverseFBO;
    unsigned int m_inversedTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *inverseShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // INVERSE_H
