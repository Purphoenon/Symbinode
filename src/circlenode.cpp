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

#include "circlenode.h"

CircleNode::CircleNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int interpolation, float radius,
                       float smooth, bool useAlpha): Node(parent, resolution, bpc),
    m_interpolation(interpolation), m_radius(radius), m_smooth(smooth), m_useAlpha(useAlpha)
{
    preview = new CircleObject(grNode, m_resolution, m_bpc, m_interpolation, m_radius, m_smooth, m_useAlpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &CircleNode::generatePreview, this, &CircleNode::previewGenerated);
    connect(preview, &CircleObject::changedTexture, this, &CircleNode::setOutput);
    connect(preview, &CircleObject::updatePreview, this, &CircleNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &CircleObject::setResolution);
    connect(this, &Node::changeBPC, preview, &CircleObject::setBPC);
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Circle");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/CircleProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startInterpolation", m_interpolation);
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(interpolationChanged(int)), this, SLOT(updateInterpolation(int)));
    connect(propertiesPanel, SIGNAL(radiusChanged(qreal)), this, SLOT(updateRadius(qreal)));
    connect(propertiesPanel, SIGNAL(smoothValueChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

CircleNode::~CircleNode() {
    delete preview;
}

void CircleNode::operation() {
    preview->selectedItem = selected();
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
    if(deserializing) deserializing = false;
}

unsigned int &CircleNode::getPreviewTexture() {
    return preview->texture();
}

void CircleNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

CircleNode *CircleNode::clone() {
    return new CircleNode(parentItem(), m_resolution, m_bpc, m_interpolation, m_radius, m_smooth,
                          m_useAlpha);
}

void CircleNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 15;
    json["interpolation"] = m_interpolation;
    json["radius"] = m_radius;
    json["smooth"] = m_smooth;
    json["useAlpha"] = m_useAlpha;
}

void CircleNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("interpolation")) {
        m_interpolation = json["interpolation"].toVariant().toInt();
        interpolationChanged(m_interpolation);
    }
    if(json.contains("radius")) {
        m_radius = json["radius"].toVariant().toFloat();
    }
    if(json.contains("smooth")) {
        m_smooth = json["smooth"].toVariant().toFloat();
    }
    if(json.contains("useAlpha")) {
        m_useAlpha = json["useAlpha"].toBool();
    }
    propertiesPanel->setProperty("startInterpolation", m_interpolation);
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);

    preview->setInterpolation(m_interpolation);
    preview->setRadius(m_radius);
    preview->setSmooth(m_smooth);
    preview->setUseAlpha(m_useAlpha);

    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);

    preview->update();
}

int CircleNode::interpolation() {
    return m_interpolation;
}

void CircleNode::setInterpolation(int interpolation) {
    if(m_interpolation == interpolation) return;
    m_interpolation = interpolation;
    interpolationChanged(interpolation);
    preview->setInterpolation(interpolation);
    preview->update();
}

float CircleNode::radius() {
    return m_radius;
}

void CircleNode::setRadius(float radius) {
    if(m_radius == radius) return;
    m_radius = radius;
    radiusChanged(radius);
    preview->setRadius(radius);
    preview->update();
}

float CircleNode::smooth() {
    return m_smooth;
}

void CircleNode::setSmooth(float smooth) {
    if(m_smooth == smooth) return;
    m_smooth = smooth;
    smoothChanged(smooth);
    preview->setSmooth(smooth);
    preview->update();
}

bool CircleNode::useAlpha() {
    return m_useAlpha;
}

void CircleNode::setUseAlpha(bool use) {
    if(m_useAlpha == use) return;
    m_useAlpha = use;
    useAlphaChanged(use);
    preview->setUseAlpha(use);
    preview->update();
}

void CircleNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void CircleNode::previewGenerated() {
    preview->generatedCircle = true;
    preview->update();
}

void CircleNode::updateInterpolation(int interpolation) {
    setInterpolation(interpolation);
    dataChanged();
}

void CircleNode::updateRadius(qreal radius) {
    setRadius(radius);
    dataChanged();
}

void CircleNode::updateSmooth(qreal smooth) {
    setSmooth(smooth);
    dataChanged();
}

void CircleNode::updateUseAlpha(bool use) {
    setUseAlpha(use);
    dataChanged();
}
