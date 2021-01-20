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

#include "backgroundobject.h"
#include "backgroundrenderer.h"
#include <iostream>

BackgroundObject::BackgroundObject(QQuickItem *parent):QQuickFramebufferObject (parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
}

QQuickFramebufferObject::Renderer *BackgroundObject::createRenderer() const {
    return new BackgroundRenderer();
}

void BackgroundObject::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        lastX = event->pos().x();
        lastY = event->pos().y();
    }
    else if(event->button() == Qt::LeftButton) {
        event->setAccepted(false);
    }
    else {
        event->setAccepted(false);
    }
}

void BackgroundObject::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() == Qt::MidButton) {
       offsetX = (lastX - event->pos().x());
       offsetY = (lastY - event->pos().y());
       m_pan.setX(m_pan.x() + offsetX);
       m_pan.setY(m_pan.y() + offsetY);
       lastX = event->pos().x();
       lastY = event->pos().y();
       isPan = true;
       update();
       emit panChanged(m_pan);
    }
    else {
        event->setAccepted(false);
    }
}

void BackgroundObject::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        offsetX = 0.0f;
        offsetY = 0.0f;
    }
    else {
        event->setAccepted(false);
    }
}

void BackgroundObject::wheelEvent(QWheelEvent *event) {
    if(event->angleDelta().y() > 0) {
       if((m_scale + 0.001f*event->angleDelta().y()) > 2.0f) return;
       scaleStep = 0.001f*event->angleDelta().y();
    }
    else {
        if((m_scale + 0.001f*event->angleDelta().y()) < 0.1f) return;
        scaleStep = 0.001f*event->angleDelta().y();
    }

    m_scale += scaleStep;
    lastX = event->pos().x();
    lastY = event->pos().y();
    m_scale = std::min(std::max(m_scale, 0.1f), 2.0f);
    isScaled = true;
    float x = lastX + m_pan.x();
    x = x/(m_scale - scaleStep);
    x = x*m_scale;
    x = x - lastX;
    m_pan.setX(x);
    float y = lastY + m_pan.y();
    y = y/(m_scale - scaleStep);
    y = y*m_scale;
    y = y - lastY;
    m_pan.setY(y);
    emit scaleChanged(m_scale);
    emit panChanged(m_pan);
    update();    
}

float BackgroundObject::viewScale() {
    return m_scale;
}

void BackgroundObject::setViewScale(float scale) {
    m_scale = scale;
    emit scaleChanged(m_scale);
    isScaled = true;
    update();
}

QVector2D BackgroundObject::viewPan() {
    return m_pan;
}

void BackgroundObject::setViewPan(QVector2D pan) {
    m_pan = pan;
    emit panChanged(m_pan);
    isPan = true;
    update();
}
