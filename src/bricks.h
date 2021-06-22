#ifndef BRICKS_H
#define BRICKS_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BricksObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    BricksObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int columns = 5, int rows = 15, float offset = 0.5f, float width = 0.9f, float height = 0.8f, float smoothX = 0.0f, float smoothY = 0.0f, float mask = 0.0f, int seed = 1);
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
    float offset();
    void setOffset(float offset);
    float bricksWidth();
    void setBricksWidth(float width);
    float bricksHeight();
    void setBricksHeight(float height);
    float mask();
    void setMask(float mask);
    float smoothX();
    void setSmoothX(float smooth);
    float smoothY();
    void setSmoothY(float smooth);
    int seed();
    void setSeed(int seed);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool generatedTex = true;
    bool resUpdated = false;
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
    int m_rows = 15;
    float m_offset = 0.5f;
    float m_width = 0.9f;
    float m_height = 0.8f;
    float m_mask = 0.0f;
    float m_smoothX = 0.0f;
    float m_smoothY = 0.0f;
    int m_seed = 1;
};

class BricksRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    BricksRenderer(QVector2D res, GLint bpc);
    ~BricksRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createBricks();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA16;
    unsigned int bricksFBO;
    unsigned int m_bricksTexture = 0;
    unsigned int m_maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *bricksShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // BRICKS_H
