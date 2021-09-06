#ifndef HEXAGONSNODE_H
#define HEXAGONSNODE_H

#include "node.h"
#include "hexagons.h"

class HexagonsNode: public Node
{
    Q_OBJECT
public:
    HexagonsNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int columns = 5, int rows = 6, float size = 0.9f, float smooth = 0.0f, float mask = 0.0f, int seed = 1);
    ~HexagonsNode();
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    void operation() override;
    HexagonsNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
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
signals:
    void columnsChanged(int columns);
    void rowsChanged(int rows);
    void hexSizeChanged(float size);
    void hexSmoothChanged(float smooth);
    void maskChanged(float mask);
    void seedChanged(int seed);
public slots:
    void setOutput();
    void updateColumns(int columns);
    void updateRows(int rows);
    void updateHexSize(qreal size);
    void updateHexSmooth(qreal smooth);
    void updateMask(qreal mask);
    void updateSeed(int seed);
    void previewGenerated();
private:
    HexagonsObject *preview;
    int m_columns = 5;
    int m_rows = 6;
    float m_size = 0.9f;
    float m_smooth = 0.0f;
    float m_mask = 0.0f;
    int m_seed = 1;
};

#endif // HEXAGONSNODE_H
