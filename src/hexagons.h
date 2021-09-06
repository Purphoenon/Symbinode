#ifndef HEXAGONS_H
#define HEXAGONS_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class HexagonsObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    HexagonsObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int columns = 5, int rows = 6, float size = 0.9f, float smooth = 0.0f, float mask = 0.0f, int seed = 1);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    void saveTexture(QString fileName);
    int columns();
    void setColumns(int columns);
    int rows();
    void setRows(int rows);
    float hexSize();
    void setHexSize(float size);
    float hexSmooth();
    void setHexSmooth(float smooth);
    float mask();
    void setMask(float mask);
    int seed();
    void setSeed(int seed);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool generatedTex = true;
    bool resUpdated = true;
    bool bpcUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void changedTexture();
private:
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    int m_columns = 5;
    int m_rows = 6;
    float m_size = 0.8f;
    float m_smooth = 0.0f;
    float m_mask = 0.0f;
    int m_seed = 1;
};

class HexagonsRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    HexagonsRenderer(QVector2D res, GLint bpc);
    ~HexagonsRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createHexagons();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int hexagonsFBO;
    unsigned int m_hexagonsTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *hexagonsShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // HEXAGONS_H
