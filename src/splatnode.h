#ifndef SPLATNODE_H
#define SPLATNODE_H

#include "node.h"
#include "splat.h"

class SplatNode: public Node
{
    Q_OBJECT
public:
    SplatNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int size = 2, int imagePerCell = 3);
    ~SplatNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    int size();
    void setSize(int size);
    int imagePerCell();
    void setImagePerCell(int count);
signals:
    void sizeChanged(int size);
    void imagePerCellChanged(int count);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void updateSize(int size);
    void updateImagePerCell(int count);
private:
    SplatObject *preview;
    int m_size = 2;
    int m_imagePerCell = 3;
};

#endif // SPLATNODE_H
