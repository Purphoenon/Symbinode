#ifndef GRADIENT_H
#define GRADIENT_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include "FreeImage.h"

class GradientObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    GradientObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QString type = "linear", float startX = 0.0f, float startY = 0.0f, float endX = 0.0f, float endY = 1.0f, float centerWidth = 0.0f, bool tiling = false);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    QString gradientType();
    void setGradientType(QString type);
    float startX();
    void setStartX(float x);
    float startY();
    void setStartY(float y);
    float endX();
    void setEndX(float x);
    float endY();
    void setEndY(float y);
    float reflectedWidth();
    void setReflectedWidth(float width);
    bool tiling();
    void setTiling(bool tiling);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    void saveTexture(QString fileName);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedGradient = true;
    bool resUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void changedTexture();
private:
    QString m_gradientType = "linear";
    float m_startX = 0.0f;
    float m_startY = 0.0f;
    float m_endX = 0.0f;
    float m_endY = 1.0f;
    float m_reflectedWidth = 0.0f;
    bool m_tiling = false;
    unsigned int m_gradientTexture = 0;
    unsigned int m_maskTexture = 0;
    QVector2D m_resolution;
};

class GradientRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    GradientRenderer(QVector2D res);
    ~GradientRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createGradient();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QOpenGLShaderProgram *gradientShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    QVector2D m_resolution;
    unsigned int gradientFBO;
    unsigned int gradientVAO, textureVAO;
    unsigned int gradientTexture = 0;
    unsigned int m_maskTexture = 0;
    QString m_gradientType;
};

#endif // GRADIENT_H
