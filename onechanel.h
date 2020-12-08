#ifndef ONECHANEL_H
#define ONECHANEL_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class OneChanelObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    OneChanelObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    QVariant value();
    void setValue(QVariant val);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool useTex = false;
    bool selectedItem = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void updateValue(QVariant data, bool useTexture);
private:
    QVariant m_value;
    QVector2D m_resolution;
};

class OneChanelRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    OneChanelRenderer(QVector2D resolution);
    ~OneChanelRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void saveTexture(QString dir);
    QOpenGLShaderProgram *renderChanel;
    float val = 0.0f;
    QVector2D m_resolution;
    unsigned int texture = 0;
    unsigned int VAO = 0;
};

#endif // ONECHANEL_H
