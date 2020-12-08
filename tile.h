#ifndef TILE_H
#define TILE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class TileObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    TileObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float offsetX = 0.0f, float offsetY = 0.0f, int columns = 5, int rows = 5, float scaleX = 1.0f, float scaleY = 1.0f, int rotation = 0, float randPosition = 0.0f, float randRotation = 0.0f, float randScale = 0.0f, float maskStrength = 0.0f, int inputsCount = 1, bool keepProportion = false);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int tile1();
    void setTile1(unsigned int texture);
    unsigned int tile2();
    void setTile2(unsigned int texture);
    unsigned int tile3();
    void setTile3(unsigned int texture);
    unsigned int tile4();
    void setTile4(unsigned int texture);
    unsigned int tile5();
    void setTile5(unsigned int texture);
    float offsetX();
    void setOffsetX(float offset);
    float offsetY();
    void setOffsetY(float offset);
    int columns();
    void setColumns(int columns);
    int rows();
    void setRows(int rows);
    float scaleX();
    void setScaleX(float scale);
    float scaleY();
    void setScaleY(float scale);
    int rotationAngle();
    void setRotationAngle(int angle);
    float randPosition();
    void setRandPosition(float rand);
    float randRotation();
    void setRandRotation(float rand);
    float randScale();
    void setRandScale(float rand);
    float maskStrength();
    void setMaskStrength(float mask);
    int inputsCount();
    void setInputsCount(int count);
    bool keepProportion();
    void setKeepProportion(bool keep);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool tiledTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_tile1 = 0;
    unsigned int m_tile2 = 0;
    unsigned int m_tile3 = 0;
    unsigned int m_tile4 = 0;
    unsigned int m_tile5 = 0;
    unsigned int m_maskTexture = 0;
    float m_offsetX = 0.0f;
    float m_offsetY = 0.0f;
    int m_columns = 5;
    int m_rows = 5;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    int m_rotationAngle = 0;
    float m_randPosition = 0.0f;
    float m_randRotation = 0.0f;
    float m_randScale = 0.0f;
    float m_maskStrength = 0.0f;
    int m_inputsCount = 1;
    bool m_keepProportion = false;
};

class TileRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    TileRenderer(QVector2D res);
    ~TileRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createTile();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_tile1 = 0;
    unsigned int m_tile2 = 0;
    unsigned int m_tile3 = 0;
    unsigned int m_tile4 = 0;
    unsigned int m_tile5 = 0;
    unsigned int m_tiledTexture = 0;
    unsigned int m_randomTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int tileFBO = 0;
    unsigned int randomFBO = 0;
    QOpenGLShaderProgram *tileShader;
    QOpenGLShaderProgram *randomShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // TILE_H
