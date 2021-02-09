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

#ifndef ALBEDONODE_H
#define ALBEDONODE_H
#include "node.h"
#include "albedo.h"

class AlbedoNode: public Node
{
    Q_OBJECT
public:
    AlbedoNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~AlbedoNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
signals:
    void albedoChanged(QVariant albedo, bool useTexture);
public slots:
    void updateAlbedo(QVector3D color);
    void updateScale(float scale);
    void saveAlbedo(QString dir);
private:
    AlbedoObject *preview;
    QVector3D m_albedo = QVector3D(1.0f, 1.0f, 1.0f);
};

#endif // ALBEDONODE_H
