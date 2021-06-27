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

#include "warpnode.h"

WarpNode::WarpNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float intensity):
    Node(parent, resolution, bpc), m_intensity(intensity)
{
    preview = new WarpObject(grNode, m_resolution, m_bpc, m_intensity);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &WarpNode::updateScale);
    connect(this, &Node::generatePreview, this, &WarpNode::previewGenerated);
    connect(preview, &WarpObject::changedTexture, this, &WarpNode::setOutput);
    connect(this, &Node::changeResolution, preview, &WarpObject::setResolution);
    connect(this, &Node::changeBPC, preview, &WarpObject::setBPC);
    connect(this, &WarpNode::intensityChanged, preview, &WarpObject::setIntensity);
    connect(preview, &WarpObject::updatePreview, this, &WarpNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/WarpProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(3, 1);
    setTitle("Warp");
    m_socketsInput[0]->setTip("Source");
    m_socketsInput[1]->setTip("Factor");
    m_socketsInput[2]->setTip("Mask");
}

WarpNode::~WarpNode() {
    delete preview;
}

void WarpNode::operation() {
    preview->selectedItem = selected();
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[2]->getEdges().isEmpty()) {
        Node *inputNode2 = static_cast<Node*>(m_socketsInput[2]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode2 && inputNode2->resolution() != m_resolution) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setWarpTexture(m_socketsInput[1]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[2]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &WarpNode::getPreviewTexture() {
    return preview->texture();
}

void WarpNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

WarpNode *WarpNode::clone() {
    return new WarpNode(parentItem(), m_resolution, m_bpc, m_intensity);
}

void WarpNode::serialize(QJsonObject &json) const{
    Node::serialize(json);
    json["type"] = 18;
    json["intensity"] = m_intensity;
}

void WarpNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    propertiesPanel->setProperty("startIntensity", m_intensity);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

float WarpNode::intensity() {
    return m_intensity;
}

void WarpNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

void WarpNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void WarpNode::previewGenerated() {
    preview->warpedTex = true;
    preview->update();
}

void WarpNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void WarpNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    operation();
    dataChanged();
}
