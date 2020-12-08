#ifndef MIX_H
#define MIX_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class MixObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MixObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float factor = 0.5f, int mode = 0);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int firstTexture();
    void setFirstTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int secondTexture();
    void setSecondTexture(unsigned int texture);
    QVariant factor();
    void setFactor(QVariant f);
    int mode();
    void setMode(int mode);
    QVector2D resolution();
    void setResolution(QVector2D res);
    unsigned int &texture();
    void setTexture(unsigned int texture);    
    bool mixedTex = false;
    bool selectedItem = false;
    bool useFactorTexture = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    unsigned int m_firstTexture = 0;
    unsigned int m_secondTexture = 0;
    QVariant m_factor = QVariant(0.5f);
    int m_mode = 0;
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
};

class MixRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    MixRenderer(QVector2D resolution);
    ~MixRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();   
private:
    void mix();
    void updateTextureRes();
    QOpenGLShaderProgram *mixShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int firstTexture = 0;
    unsigned int secondTexture = 0;
    unsigned int mixFBO;
    unsigned int mixTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int factorTexture = 0;
    float mixFactor = 0.5f;
    QVector2D m_resolution;
    unsigned int VAO;
};

#endif // MIX_H
