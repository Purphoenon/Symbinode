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

#ifndef POLYGONNODE_H
#define POLYGONNODE_H

#include "node.h"
#include "polygon.h"

class PolygonNode: public Node
{
    Q_OBJECT
public:
    PolygonNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int sides = 3, float polygonScale = 0.4f, float smooth = 0.0f, bool useAlpha = true);
    ~PolygonNode();
    void operation();
    unsigned int &getPreviewTexture();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    int sides();
    void setSides(int sides);
    float polygonScale();
    void setPolygonScale(float scale);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
signals:
    void sidesChanged(int sides);
    void polygonScaleChanged(float scale);
    void smoothChanged(float smooth);
    void useAlphaChanged(bool use);
public slots:
    void updateScale(float scale);
    void setOutput();
    void previewGenerated();
    void updateSides(int sides);
    void updatePolygonScale(qreal scale);
    void updateSmooth(qreal smooth);
    void updateUseAlpha(bool use);
private:
    PolygonObject *preview;
    int m_sides = 3;
    float m_polygonScale = 0.4f;
    float m_smooth = 0.0f;
    bool m_useAlpha = true;
};

#endif // POLYGONNODE_H
