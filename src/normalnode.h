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

#ifndef NORMALNODE_H
#define NORMALNODE_H

#include "node.h"
#include "normal.h"

class NormalNode: public Node
{
     Q_OBJECT
public:
    NormalNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~NormalNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
public slots:
    void updateScale(float scale);
    void saveNormal(QString dir);
signals:
    void normalChanged(unsigned int normalMap);
private:
    NormalObject *preview;
};

#endif // NORMALNODE_H
