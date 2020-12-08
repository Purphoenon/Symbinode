#ifndef TILENODE_H
#define TILENODE_H

#include "node.h"
#include "tile.h"

class TileNode: public Node
{
    Q_OBJECT
public:
    TileNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float offsetX = 0.0f, float offsetY = 0.0f, int columns = 5, int rows = 5, float scaleX = 1.0f, float scaleY = 1.0f, int rotation = 0, float randPosition = 0.0f, float randRotation = 0.0f, float randScale = 0.0f, float maskStrength = 0.0f, int inputsCount = 1, bool keepProportion = false);
    ~TileNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
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
    void setOutput();
signals:
    void offsetXChanged(float offset);
    void offsetYChanged(float offset);
    void columnsChanged(int columns);
    void rowsChanged(int rows);
    void scaleXChanged(float scale);
    void scaleYChanged(float scale);
    void rotationAngleChanged(int angle);
    void randPositionChanged(float rand);
    void randRotationChanged(float rand);
    void randScaleChanged(float rand);
    void maskStrengthChanged(float mask);
    void inputsCountChanged(int count);
    void keepProportionChanged(bool keep);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void previewGenerated();
    void updateOffsetX(qreal offset);
    void updateOffsetY(qreal offset);
    void updateColums(int columns);
    void updateRows(int rows);
    void updateScaleX(qreal scale);
    void updateScaleY(qreal scale);
    void updateRotationAngle(int angle);
    void updateRandPosition(qreal rand);
    void updateRandRotation(qreal rand);
    void updateRandScale(qreal rand);
    void updateMaskStrength(qreal mask);
    void updateInputsCount(int count);
    void updateKeepProportion(bool keep);
private:
    TileObject *preview;
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

#endif // TILENODE_H
