#ifndef NOISE_H
#define NOISE_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class NoiseObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    NoiseObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QString type = "noisePerlin", float noiseScale = 5.0f, float scaleX = 1.0f, float scaleY = 1.0f, int layers = 8, float persistence = 0.5f,
                float amplitude = 1.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    QString noiseType();
    void setNoiseType(QString type);
    float noiseScale();
    void setNoiseScale(float scale);
    float scaleX();
    void setScaleX(float scale);
    float scaleY();
    void setScaleY(float scale);
    int layers();
    void setLayers(int num);
    float persistence();
    void setPersistence(float value);
    float amplitude();
    void setAmplitude(float value);
    QVector2D resolution();
    void setResolution(QVector2D res);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    bool generatedNoise = true;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QString m_noiseType = "noisePerlin";
    float m_noiseScale = 5.0f;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    int m_layers = 8;
    float m_persistence = 0.5f;
    float m_amplitude = 1.0f;
    unsigned int m_texture = 0;
    QVector2D m_resolution;
    unsigned int m_maskTexture = 0;
};

class NoiseRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    NoiseRenderer(QVector2D resolution);
    ~NoiseRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();   
private:
    void createNoise();
    void updateTexResolution();
    QOpenGLShaderProgram *generateNoise;
    QOpenGLShaderProgram *renderTexture;
    unsigned int noiseFBO;
    unsigned int noiseVAO, textureVAO;
    unsigned int noiseTexture = 0;
    QVector2D m_resolution;
    QString m_noiseType;
    unsigned int m_maskTexture = 0;
};

#endif // NOISE_H
