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

#include "normalmapnode.h"

NormalMapNode::NormalMapNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float strenght):
    Node(parent, resolution, bpc), m_strenght(strenght)
{
    createSockets(1, 1);
    setTitle("Normal Map");
    m_socketsInput[0]->setTip("Height");
    preview = new NormalMapObject(grNode, m_resolution, m_bpc, m_strenght);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    propView = new QQuickView();
    connect(preview, &NormalMapObject::textureChanged, this, &NormalMapNode::setOutput);
    connect(this, &NormalMapNode::generatePreview, this, &NormalMapNode::previewGenerated);
    connect(this, &NormalMapNode::strenghtChanged, preview, &NormalMapObject::setStrenght);
    connect(this, &Node::changeScaleView, this, &NormalMapNode::updateScale);
    connect(preview, &NormalMapObject::updatePreview, this, &NormalMapNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &NormalMapObject::setResolution);
    connect(this, &Node::changeBPC, preview, &NormalMapObject::setBPC);
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/NormalMapProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(strenghtChanged(qreal)), this, SLOT(updateStrenght(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    propertiesPanel->setProperty("startStrenght", m_strenght/10.0f);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

NormalMapNode::~NormalMapNode() {
    delete preview;
}

void NormalMapNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
    }
    preview->setGrayscaleTexture(m_socketsInput[0]->value().toUInt());
    preview->selectedItem = selected();
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &NormalMapNode::getPreviewTexture() {
    return preview->normalTexture();
}

void NormalMapNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

float NormalMapNode::strenght() {
    return m_strenght;
}

void NormalMapNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 11;
    json["strength"] = m_strenght;
}

void NormalMapNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("strength")) {
        updateStrenght(json["strength"].toVariant().toFloat());
        propertiesPanel->setProperty("startStrenght", m_strenght/10.0f);
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void NormalMapNode::updateStrenght(qreal strenght) {
    m_strenght = static_cast<float>(strenght);
    strenghtChanged(m_strenght);
    operation();
    dataChanged();
}

void NormalMapNode::setOutput() {
    m_socketOutput[0]->setValue(preview->normalTexture());
}

void NormalMapNode::previewGenerated() {
    preview->normalGenerated = true;
    preview->update();
}

void NormalMapNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}
