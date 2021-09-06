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

InverseNode::InverseNode(QQuickItem *parent, QVector2D resolution, GLint bpc): Node(parent, resolution, bpc)
{
    preview = new InverseObject(grNode, m_resolution, m_bpc);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::generatePreview, this, &InverseNode::previewGenerated);
    connect(this, &Node::changeResolution, preview, &InverseObject::setResolution);
    connect(this, &Node::changeBPC, preview, &InverseObject::setBPC);
    connect(preview, &InverseObject::textureChanged, this, &InverseNode::setOutput);
    connect(preview, &InverseObject::updatePreview, this, &InverseNode::updatePreview);
    createSockets(1, 1);
    setTitle("Inverse");
    m_socketsInput[0]->setTip("Color");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BitsProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

InverseNode::~InverseNode() {
    delete preview;
}

void InverseNode::operation() {
    preview->selectedItem = selected();
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    if(deserializing) deserializing = false;
}

unsigned int &InverseNode::getPreviewTexture() {
    return preview->texture();
}

void InverseNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

InverseNode *InverseNode::clone() {
    return new InverseNode(parentItem(), m_resolution, m_bpc);
}

void InverseNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 20;
}

void InverseNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void InverseNode::previewGenerated() {
    preview->inversedTex = true;
    preview->update();
}

void InverseNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
