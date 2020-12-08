#ifndef COLOR_H
#define COLOR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class ColorObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ColorObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QVector3D color = QVector3D(1, 1, 1));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    QVector3D color();
    void setColor(QVector3D color);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool createdTexture = true;
    bool resUpdated = false;
signals:
    void textureChanged();
private:
    QVector2D m_resolution;
    QVector3D m_color = QVector3D(1, 1, 1);
    unsigned int m_texture = 0;
};

class ColorRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    ColorRenderer(QVector2D res);
    ~ColorRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createColor();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int colorFBO;
    unsigned int m_colorTexture = 0;
    unsigned int colorVAO = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *colorShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // COLOR_H
