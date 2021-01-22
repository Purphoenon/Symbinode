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

#ifndef MAPPINGNODE_H
#define MAPPINGNODE_H

#include "node.h"
#include "mapping.h"

class MappingNode: public Node
{
    Q_OBJECT
public:
    MappingNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float inputMin = 0.0f, float inputMax = 1.0f, float outputMin = 0.0f, float outputMax = 1.0f);
    ~MappingNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    float inputMin();
    void setInputMin(float value);
    float inputMax();
    void setInputMax(float value);
    float outputMin();
    void setOutputMin(float value);
    float outputMax();
    void setOutputMax(float value);
signals:
    void inputMinChanged(float value);
    void inputMaxChanged(float value);
    void outputMinChanged(float value);
    void outputMaxChanged(float value);
public slots:
    void updateScale(float scale);
    void setOutput();
    void updateInputMin(qreal value);
    void updateInputMax(qreal value);
    void updateOutputMin(qreal value);
    void updateOutputMax(qreal value);
private:
    MappingObject *preview;
    float m_inputMin = 0.0f;
    float m_inputMax = 1.0f;
    float m_outputMin = 0.0f;
    float m_outputMax = 1.0f;
};

#endif // MAPPINGNODE_H
