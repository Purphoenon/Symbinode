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

#ifndef WARPNODE_H
#define WARPNODE_H

#include "node.h"
#include "warp.h"

class WarpNode: public Node
{
    Q_OBJECT
public:
    WarpNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float intensity = 0.1f);
    ~WarpNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    float intensity();
    void setIntensity(float intensity);
signals:
    void intensityChanged(float intensity);
public slots:
    void updateScale(float scale);
    void previewGenerated();
    void setOutput();
    void updateIntensity(qreal intensity);
private:
    WarpObject *preview;
    float m_intensity = 0.1f;
};

#endif // WARPNODE_H
