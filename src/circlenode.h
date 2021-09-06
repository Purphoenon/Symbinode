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
    CircleNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA16, int interpolation = 1, float radius = 0.5f, float smooth = 0.01f, bool useAlpha = true);
    ~CircleNode();
    void operation() override;
    unsigned int &getPreviewTexture() override;
    void saveTexture(QString fileName) override;
    CircleNode *clone() override;
    void serialize(QJsonObject &json) const override;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) override;
    int interpolation();
    void setInterpolation(int interpolation);
    float radius();
    void setRadius(float radius);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
signals:
    void interpolationChanged(int interpolation);
    void radiusChanged(float radius);
    void smoothChanged(float smooth);
    void useAlphaChanged(bool use);
public slots:
    void setOutput();
    void previewGenerated();
    void updateInterpolation(int interpolation);
    void updateRadius(qreal radius);
    void updateSmooth(qreal smooth);
    void updateUseAlpha(bool use);
private:
    CircleObject *preview;
    int m_interpolation = 1;
    float m_radius = 0.5f;
    float m_smooth = 0.01f;
    bool m_useAlpha = true;
};

#endif // CIRCLENODE_H
