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

#include "brightnesscontrastnode.h"

BrightnessContrastNode::BrightnessContrastNode(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                               float brightness, float contrast):
    Node(parent, resolution, bpc), m_brightness(brightness), m_contrast(contrast)
{
    preview = new BrightnessContrastObject(grNode, m_resolution, m_bpc, m_brightness, m_contrast);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &BrightnessContrastNode::updateScale);
    connect(this, &Node::generatePreview, this, &BrightnessContrastNode::previewGenerated);
    connect(preview, &BrightnessContrastObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &BrightnessContrastObject::textureChanged, this, &BrightnessContrastNode::setOutput);
    connect(this, &Node::changeResolution, preview, &BrightnessContrastObject::setResolution);
    connect(this, &Node::changeBPC, preview, &BrightnessContrastObject::setBPC);
    connect(this, &BrightnessContrastNode::brightnessChanged, preview, &BrightnessContrastObject::setBrightness);
    connect(this, &BrightnessContrastNode::contrastChanged, preview, &BrightnessContrastObject::setContrast);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BrightnessContrastProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startBrightness", m_brightness);
    propertiesPanel->setProperty("startContrast", m_contrast);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(brightnessChanged(qreal)), this, SLOT(updateBrightness(qreal)));
    connect(propertiesPanel, SIGNAL(contrastChanged(qreal)), this, SLOT(updateContrast(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(1, 1);
    setTitle("Brightness-Contrast");
    m_socketsInput[0]->setTip("Texture");
}

BrightnessContrastNode::~BrightnessContrastNode() {
    delete preview;
}

void BrightnessContrastNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &BrightnessContrastNode::getPreviewTexture() {
    return preview->texture();
}

void BrightnessContrastNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void BrightnessContrastNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 21;
    json["brightness"] = m_brightness;
    json["contrast"] = m_contrast;
}

void BrightnessContrastNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("brightness")) {
        updateBrightness(json["brightness"].toVariant().toReal());
        propertiesPanel->setProperty("startBrightness", m_brightness);
    }
    if(json.contains("contrast")) {
        updateContrast(json["contrast"].toVariant().toReal());
        propertiesPanel->setProperty("startContrast", m_contrast);
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

float BrightnessContrastNode::brightness() {
    return m_brightness;
}

void BrightnessContrastNode::setBrightness(float value) {
    m_brightness = value;
    brightnessChanged(value);
}

float BrightnessContrastNode::contrast() {
    return m_contrast;
}

void BrightnessContrastNode::setContrast(float value) {
    m_contrast = value;
    contrastChanged(value);
}

void BrightnessContrastNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void BrightnessContrastNode::previewGenerated() {
    preview->created = true;
    preview->update();
}

void BrightnessContrastNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void BrightnessContrastNode::updateBrightness(qreal value) {
    setBrightness(value);
    dataChanged();
}

void BrightnessContrastNode::updateContrast(qreal value) {
    setContrast(value);
    dataChanged();
}
