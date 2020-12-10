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

#ifndef CIRCLENODE_H
#define CIRCLENODE_H

#include "node.h"
#include "circle.h"

class CircleNode: public Node
{
    Q_OBJECT
public:
    CircleNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int interpolation = 1, float radius = 0.5f, float smooth = 0.01f);
    ~CircleNode();
    void operation();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    int interpolation();
    void setInterpolation(int interpolation);
    float radius();
    void setRadius(float radius);
    float smooth();
    void setSmooth(float smooth);
signals:
    void interpolationChanged(int interpolation);
    void radiusChanged(float radius);
    void smoothChanged(float smooth);
public slots:
    void updateScale(float scale);
    void updatePrev(bool sel);
    void setOutput();
    void previewGenerated();
    void updateInterpolation(int interpolation);
    void updateRadius(qreal radius);
    void updateSmooth(qreal smooth);
private:
    CircleObject *preview;
    int m_interpolation = 1;
    float m_radius = 0.5f;
    float m_smooth = 0.01f;
};

#endif // CIRCLENODE_H
