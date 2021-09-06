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

#ifndef CUBICBEZIER_H
#define CUBICBEZIER_H

#include <QQuickItem>

class CubicBezier: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
    Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
    Q_PROPERTY(QPointF p3 READ p3 WRITE setP3 NOTIFY p3Changed)
    Q_PROPERTY(QPointF p4 READ p4 WRITE setP4 NOTIFY p4Changed)
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
public:
    CubicBezier(QQuickItem *parent = nullptr);
    ~CubicBezier();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    QPointF p1() const;
    QPointF p2() const;
    QPointF p3() const;
    QPointF p4() const;
    int segmentCount() const;
    float lineWidth() const;
    QColor color() const;
    void setP1(const QPointF &point);
    void setP2(const QPointF &point);
    void setP3(const QPointF &point);
    void setP4(const QPointF &point);
    void setSegmentCount(int count);
    void setLineWidth(float width);
    void setColor(QColor color);
signals:
    void p1Changed(const QPointF &point);
    void p2Changed(const QPointF &point);
    void p3Changed(const QPointF &point);
    void p4Changed(const QPointF &point);
    void segmentCountChanged(int count);
    void lineWidthChanged(float width);
private:
    QPointF m_p1;
    QPointF m_p2;
    QPointF m_p3;
    QPointF m_p4;
    int m_segmentCount;
    float m_lineWidth;
    QColor m_color;
};

#endif // CUBICBEZIER_H
