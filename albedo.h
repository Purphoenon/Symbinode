#ifndef ALBEDO_H
#define ALBEDO_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include "FreeImage.h"

class AlbedoObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    AlbedoObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    QVariant albedo();
    void setAlbedo(QVariant albedo);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool useAlbedoTex = false;
    bool selectedItem = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void updateAlbedo(QVariant albedo, bool useTexture);
private:
    QVariant m_albedo = QVector3D(1.0f, 1.0f, 1.0f);
    QVector2D m_resolution;
};

class AlbedoRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    AlbedoRenderer(QVector2D resolution);
    ~AlbedoRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void saveTexture(QString dir);
    QOpenGLShaderProgram *renderAlbedo;
    unsigned int VAO = 0;
    unsigned int albedoTexture = 0;
    QVector3D albedoVal = QVector3D(1.0f, 1.0f, 1.0f);
    QVector2D m_resolution;
};

#endif // ALBEDO_H
