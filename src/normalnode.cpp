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

#include "normalnode.h"
#include <iostream>

NormalNode::NormalNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 0);
    setTitle("Normal");
    m_socketsInput[0]->setTip("Normal");
    preview = new NormalObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &NormalObject::updatePreview, this, &NormalNode::updatePreview);
    connect(this, &Node::changeScaleView, this, &NormalNode::updateScale);
    connect(preview, &NormalObject::updateNormal, this, &NormalNode::normalChanged);
    connect(this, &Node::changeResolution, preview, &NormalObject::setResolution);
}

NormalNode::~NormalNode() {
    delete preview;
}

void NormalNode::operation() {
    preview->setNormalTexture(m_socketsInput[0]->value().toUInt());
    preview->selectedItem = selected();
    preview->update();
}

unsigned int &NormalNode::getPreviewTexture() {
    return preview->normalTexture();
}

void NormalNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void NormalNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 12;
}

void NormalNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void NormalNode::saveNormal(QString dir) {
    preview->saveTexture(dir.append("/normal.png"));
}
