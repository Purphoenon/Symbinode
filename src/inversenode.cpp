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

#include "inversenode.h"

InverseNode::InverseNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    preview = new InverseObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &InverseNode::updateScale);
    connect(this, &Node::changeSelected, this, &InverseNode::updatePrev);
    connect(this, &Node::changeResolution, preview, &InverseObject::setResolution);
    connect(preview, &InverseObject::textureChanged, this, &InverseNode::setOutput);
    connect(preview, &InverseObject::updatePreview, this, &InverseNode::updatePreview);
    createSockets(1, 1);
    setTitle("Inverse");
    m_socketsInput[0]->setTip("Color");
}

InverseNode::~InverseNode() {
    delete preview;
}

void InverseNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void InverseNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 20;
}

void InverseNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void InverseNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void InverseNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
