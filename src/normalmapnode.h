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

#ifndef NORMALMAPNODE_H
#define NORMALMAPNODE_H

#include "node.h"
#include "normalmap.h"

class NormalMapNode: public Node
{
    Q_OBJECT
public:
    NormalMapNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float strenght = 6.0f);
    ~NormalMapNode();
    void operation();
    unsigned int &getPreviewTexture();
    float strenght();
    void setOutput();
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
public slots:
    void updateStrenght(qreal strenght);
    void previewGenerated();
    void updateScale(float scale);
signals:
    void strenghtChanged(float strenght);
private:
    NormalMapObject *preview;
    float m_strenght = 6.0f;
};

#endif // NORMALMAPNODE_H
