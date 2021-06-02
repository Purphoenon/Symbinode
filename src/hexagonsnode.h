#ifndef HEXAGONSNODE_H
#define HEXAGONSNODE_H

#include "node.h"
#include "hexagons.h"

class HexagonsNode: public Node
{
    Q_OBJECT
public:
    HexagonsNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int columns = 5, int rows = 6, float size = 0.9f, float smooth = 0.0f);
    ~HexagonsNode();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    int columns();
    void setColumns(int columns);
    int rows();
    void setRows(int rows);
    float hexSize();
    void setHexSize(float size);
    float hexSmooth();
    void setHexSmooth(float smooth);
signals:
    void columnsChanged(int columns);
    void rowsChanged(int rows);
    void hexSizeChanged(float size);
    void hexSmoothChanged(float smooth);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateColumns(int columns);
    void updateRows(int rows);
    void updateHexSize(qreal size);
    void updateHexSmooth(qreal smooth);
    void previewGenerated();
private:
    HexagonsObject *preview;
    int m_columns = 5;
    int m_rows = 6;
    float m_size = 0.9f;
    float m_smooth = 0.0f;
};

#endif // HEXAGONSNODE_H
