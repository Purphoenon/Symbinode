#ifndef POLYGONT_H
#define POLYGONT_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class PolygonObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    PolygonObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int sides = 3, float polygonScale = 0.4f, float smooth = 0.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    int sides();
    void setSides(int sides);
    float polygonScale();
    void setPolygonScale(float scale);
    float smooth();
    void setSmooth(float smooth);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedPolygon = true;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    int m_sides = 3;
    float m_scale = 0.4f;
    float m_smooth = 0.0f;
};

class PolygonRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    PolygonRenderer(QVector2D resolution);
    ~PolygonRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createPolygon();
    void updateTexResolution();
    QOpenGLShaderProgram *generatePolygon;
    QOpenGLShaderProgram *renderTexture;
    unsigned int polygonFBO;
    unsigned int polygonVAO, textureVAO;
    unsigned int polygonTexture;
    unsigned int maskTexture = 0;
    QVector2D m_resolution;
};

#endif // POLYGONT_H
