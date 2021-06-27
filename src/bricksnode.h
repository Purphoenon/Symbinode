#ifndef BRICKSNODE_H
#define BRICKSNODE_H

#include "node.h"
#include "bricks.h"

class BricksNode: public Node
{
    Q_OBJECT
public:
    BricksNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int columns = 5, int rows = 15, float offset = 0.5f, float width = 0.9f, float height = 0.8f, float smoothX = 0.0f, float smoothY = 0.0f, float mask = 0.0f, int seed = 1);
    ~BricksNode();
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    void operation() override;
    BricksNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
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
signals:
    void columnsChanged(int columns);
    void rowsChanged(int rows);
    void offsetChanged(float offset);
    void bricksWidthChanged(float width);
    void bricksHeightChanged(float height);
    void maskChanged(float mask);
    void smoothXChanged(float smooth);
    void smoothYChanged(float smooth);
    void seedChanged(int seed);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateColumns(int columns);
    void updateRows(int rows);
    void updateOffset(qreal offset);
    void updateBricksWidth(qreal width);
    void updateBricksHeight(qreal height);
    void updateMask(qreal mask);
    void updateSmoothX(qreal smooth);
    void updateSmoothY(qreal smooth);
    void updateSeed(int seed);
    void previewGenerated();
private:
    BricksObject *preview;
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

#endif // BRICKSNODE_H
