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

#ifndef METALNODE_H
#define METALNODE_H

#include "node.h"
#include "onechanel.h"

class MetalNode: public Node
{
    Q_OBJECT
public:
    MetalNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8);
    ~MetalNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
public slots:
    void updateMetal(qreal metal);
    void updateScale(float scale);
    void saveMetal(QString dir);
signals:
    void metalChanged(QVariant metal, bool useTexture);
private:
    OneChanelObject *preview;
    float m_metal = 0.0f;
};

#endif // METALNODE_H
