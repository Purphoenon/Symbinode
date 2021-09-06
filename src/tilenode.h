/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TILENODE_H
#define TILENODE_H

#include "node.h"
#include "tile.h"

class TileNode: public Node
{
    Q_OBJECT
public:
    TileNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float offsetX = 0.0f, float offsetY = 0.0f, int columns = 5, int rows = 5, float scale = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, int rotation = 0, float randPosition = 0.0f, float randRotation = 0.0f, float randScale = 0.0f, float maskStrength = 0.0f, int inputsCount = 1, int seed = 1, bool keepProportion = false, bool useAlpha = true, bool depthMask = true);
    ~TileNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    TileNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
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
    int seed();
    void setSeed(int seed);
    float tileScale();
    void setTileScale(float scale);
    bool keepProportion();
    void setKeepProportion(bool keep);
    bool useAlpha();
    void setUseAlpha(bool use);
    bool depthMask();
    void setDepthMask(bool depth);
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
    void seedChanged(int seed);
    void tileScaleChanged(float scale);
    void keepProportionChanged(bool keep);
    void useAlphaChanged(bool use);
    void depthMaskChanged(bool depth);
public slots:
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
    void updateSeed(int seed);
    void updateTileScale(qreal scale);
    void updateKeepProportion(bool keep);
    void updateUseAlpha(bool use);
    void updateDepthMask(bool depth);
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
    int m_seed = 1;
    float m_scale = 1.0f;
    bool m_keepProportion = false;
    bool m_useAlpha = true;
    bool m_depthMask = true;
};

#endif // TILENODE_H
