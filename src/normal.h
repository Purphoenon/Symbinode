#ifndef NORMAL_H
#define NORMAL_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class NormalObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    NormalObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    QVector2D resolution();
    void setResolution(QVector2D res);
    unsigned int normalTexture();
    void setNormalTexture(unsigned int texture);
    bool selectedItem = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void updateNormal(unsigned int normalMap);
private:
    QVector2D m_resolution;
    unsigned int m_normalMap = 0;
};

class NormalRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    NormalRenderer(QVector2D resolution);
    ~NormalRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void saveTexture(QString name);
    QOpenGLShaderProgram *renderNormal;
    QVector2D m_resolution;
    unsigned int VAO = 0;
    unsigned int m_normalTexture = 0;
};

#endif // NORMAL_H
