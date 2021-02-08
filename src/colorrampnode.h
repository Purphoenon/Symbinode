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

#ifndef COLORRAMPNODE_H
#define COLORRAMPNODE_H

#include "node.h"
#include "colorramp.h"

class ColorRampNode: public Node
{
    Q_OBJECT
public:
    ColorRampNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QJsonArray stops = {QJsonArray{1, 1, 1, 1}, QJsonArray{0, 0, 0, 0}});
    ~ColorRampNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    QJsonArray stops() const;
signals:
    void stopsChanged(QVariant gradients);
public slots:
    void updateScale(float scale);
    void previewGenerated();
    void setOutput();
private:
    ColorRampObject *preview;
};

#endif // COLORRAMPNODE_H
