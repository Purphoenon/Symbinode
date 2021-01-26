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

#include "cutline.h"
#include <QPainter>

CutLine::CutLine(QQuickItem *parent): QQuickPaintedItem (parent)
{
     m_pen = QPen(QColor(255, 255, 255));
     m_pen.setWidthF(2.0);
     m_pen.setDashPattern(QVector<qreal>({3, 3}));
     setZ(2);
}

void CutLine::paint(QPainter *painter) {
    painter->setBrush(Qt::NoBrush);
    painter->setPen(m_pen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPolyline(m_linePoints.data(), m_linePoints.size());
}

void CutLine::addPoint(QPointF point) {
    m_linePoints.push_back(point);
}

QPointF CutLine::pointAt(int index) {
    return m_linePoints[index];
}

int CutLine::pointCount() {
    return m_linePoints.size();
}
