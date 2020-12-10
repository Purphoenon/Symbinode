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

#include "cubicbezier.h"
#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <iostream>

CubicBezier::CubicBezier(QQuickItem *parent): QQuickItem (parent), m_p1(0, 0), m_p2(1, 0), m_p3(0, 1),
    m_p4(1, 1), m_segmentCount(64), m_lineWidth(3.0f)
{
    setFlag(ItemHasContents, true);
}

CubicBezier::~CubicBezier() {}

QSGNode *CubicBezier::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
    QSGGeometryNode *node = nullptr;
    QSGGeometry *geometry = nullptr;
    if(!oldNode) {
        node = new QSGGeometryNode();
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_segmentCount);
        geometry->setLineWidth(m_lineWidth);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geometry);
        node->setFlags(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial();
        material->setColor(QColor(104, 163, 219));
        node->setMaterial(material);
        node->setFlags(QSGNode::OwnsMaterial);
    }
    else {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        geometry->setLineWidth(m_lineWidth);
        geometry->allocate(m_segmentCount);
    }
    QSizeF itemSize = size();
    QSGGeometry::Point2D *vertieces = geometry->vertexDataAsPoint2D();

    for(int i = 0; i < m_segmentCount; ++i) {
        qreal t = i/qreal(m_segmentCount - 1);
        qreal invt = 1 - t;
        QPointF pos = invt*invt*invt*m_p1 + 3*invt*invt*t*m_p2 + 3*invt*t*t*m_p3 + t*t*t*m_p4;
        float x = pos.x()*itemSize.width();
        float y = pos.y()*itemSize.height();
        vertieces[i].set(x, y);
    }
    node->markDirty(QSGNode::DirtyGeometry);
    return node;
}

QPointF CubicBezier::p1() const {
    return m_p1;
}

QPointF CubicBezier::p2() const {
    return m_p2;
}

QPointF CubicBezier::p3() const {
    return m_p3;
}

QPointF CubicBezier::p4() const {
    return m_p4;
}

int CubicBezier::segmentCount() const {
    return m_segmentCount;
}

float CubicBezier::lineWidth() const {
    return m_lineWidth;
}

void CubicBezier::setP1(const QPointF &point) {
    if(point == m_p1) return;
    m_p1 = point;
    emit p1Changed(point);
    update();
}

void CubicBezier::setP2(const QPointF &point) {
    if(point == m_p2) return;
    m_p2 = point;
    emit p2Changed(point);
    update();
}

void CubicBezier::setP3(const QPointF &point) {
    if(point == m_p3) return;
    m_p3 = point;
    emit p3Changed(point);
    update();
}

void CubicBezier::setP4(const QPointF &point) {
    if(point == m_p4) return;
    m_p4 = point;
    emit p4Changed(point);
    update();
}

void CubicBezier::setSegmentCount(int count) {
    if(count == m_segmentCount) return;
    m_segmentCount = count;
    emit segmentCountChanged(count);
    update();
}

void CubicBezier::setLineWidth(float width) {
    if(width == m_lineWidth) return;
    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}
