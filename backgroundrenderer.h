#ifndef BACKGROUNDRENDERER_H
#define BACKGROUNDRENDERER_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BackgroundRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    BackgroundRenderer();
    ~BackgroundRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    unsigned int VAO;
    QOpenGLShaderProgram *backgroundGrid;
    float panX = 0.0f, panY = 0.0f;
    float scale = 1.0f;
};

#endif // BACKGROUNDRENDERER_H
