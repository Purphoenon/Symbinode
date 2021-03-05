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

#ifndef BACKGROUNDOBJECT_H
#define BACKGROUNDOBJECT_H
#include <QQuickFramebufferObject>

class BackgroundObject: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(float viewScale READ viewScale WRITE setViewScale NOTIFY scaleChanged)
    Q_PROPERTY(QVector2D viewPan READ viewPan WRITE setViewPan NOTIFY panChanged)
public:
    BackgroundObject(QQuickItem *parent = nullptr);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    float viewScale();
    void setViewScale(float scale);
    QVector2D viewPan();
    void setViewPan(QVector2D pan);
    bool isPan = false;
    bool isScaled = false;
signals:
    void panChanged(QVector2D pan);
    void scaleChanged(float scale);
private:
    float offsetX = 0, offsetY = 0;
    float lastX, lastY;
    float startX, startY;
    float scaleStep = 0.0f;
    float m_scale = 1.0f;
    QVector2D m_pan = QVector2D(0.0f, 0.0f);
};

#endif // BACKGROUNDOBJECT_H
