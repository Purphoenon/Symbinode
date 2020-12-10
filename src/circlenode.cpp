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

CircleNode::CircleNode(QQuickItem *parent, QVector2D resolution, int interpolation, float radius, float smooth):
    Node(parent, resolution), m_interpolation(interpolation), m_radius(radius), m_smooth(smooth)
{
    preview = new CircleObject(grNode, m_resolution, m_interpolation, m_radius, m_smooth);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &CircleNode::updateScale);
    connect(this, &CircleNode::changeSelected, this, &CircleNode::updatePrev);
    connect(this, &CircleNode::generatePreview, this, &CircleNode::previewGenerated);
    connect(preview, &CircleObject::changedTexture, this, &CircleNode::setOutput);
    connect(preview, &CircleObject::updatePreview, this, &CircleNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &CircleObject::setResolution);
    connect(this, &CircleNode::interpolationChanged, preview, &CircleObject::setInterpolation);
    connect(this, &CircleNode::radiusChanged, preview, &CircleObject::setRadius);
    connect(this, &CircleNode::smoothChanged, preview, &CircleObject::setSmooth);    
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Circle");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/CircleProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startInterpolation", m_interpolation);
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    connect(propertiesPanel, SIGNAL(interpolationChanged(int)), this, SLOT(updateInterpolation(int)));
    connect(propertiesPanel, SIGNAL(radiusChanged(qreal)), this, SLOT(updateRadius(qreal)));
    connect(propertiesPanel, SIGNAL(smoothValueChanged(qreal)), this, SLOT(updateSmooth(qreal)));
}

CircleNode::~CircleNode() {
    delete preview;
}

void CircleNode::operation() {
    preview->selectedItem = selected();
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
}

void CircleNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 15;
    json["interpolation"] = m_interpolation;
    json["radius"] = m_radius;
    json["smooth"] = m_smooth;
}

void CircleNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
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
    propertiesPanel->setProperty("startInterpolation", m_interpolation);
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSmooth", m_smooth);
}

int CircleNode::interpolation() {
    return m_interpolation;
}

void CircleNode::setInterpolation(int interpolation) {
    m_interpolation = interpolation;
    interpolationChanged(interpolation);
}

float CircleNode::radius() {
    return m_radius;
}

void CircleNode::setRadius(float radius) {
    m_radius = radius;
    radiusChanged(radius);
}

float CircleNode::smooth() {
    return m_smooth;
}

void CircleNode::setSmooth(float smooth) {
    m_smooth = smooth;
    smoothChanged(smooth);
}

void CircleNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void CircleNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
        updatePreview(preview->texture(), true);
    }
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
