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

#include "blurnode.h"
#include <iostream>

BlurNode::BlurNode(QQuickItem *parent, QVector2D resolution, float intensity): Node(parent, resolution),
    m_intensity(intensity)
{
    preview = new BlurObject(grNode, m_resolution, m_intensity);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &BlurNode::updateScale);
    connect(preview, &BlurObject::textureChanged, this, &BlurNode::setOutput);
    connect(this, &Node::changeResolution, preview, &BlurObject::setResolution);
    connect(this, &BlurNode::intensityChanged, preview, &BlurObject::setIntensity);
    connect(preview, &BlurObject::updatePreview, this, &BlurNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BlurProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(2, 1);
    setTitle("Blur");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

BlurNode::~BlurNode() {
    delete preview;
}

void BlurNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &BlurNode::getPreviewTexture() {
    return preview->texture();
}

void BlurNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void BlurNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 19;
    json["intensity"] = m_intensity;
}

void BlurNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
        propertiesPanel->setProperty("startIntensity", m_intensity);
    }
}

float BlurNode::intensity() {
    return m_intensity;
}

void BlurNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

void BlurNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void BlurNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void BlurNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    operation();
    dataChanged();
}
