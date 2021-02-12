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

#ifndef CUTLINE_H
#define CUTLINE_H

#include <QQuickPaintedItem>
#include <QPen>
#include <vector>

class CutLine: public QQuickPaintedItem
{
    Q_OBJECT
public:
    CutLine(QQuickItem *parent = nullptr);
    void paint(QPainter *painter);
    void addPoint(QPointF point);
    QPointF pointAt(int index);
    int pointCount();
private:
    QPen m_pen;
    std::vector<QPointF> m_linePoints;
};

#endif // CUTLINE_H
