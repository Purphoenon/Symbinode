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

#include "coloringnode.h"

ColoringNode::ColoringNode(QQuickItem *parent, QVector2D resolution, QVector3D color):
    Node(parent, resolution), m_color(color)
{
    preview = new ColoringObject(grNode, m_resolution, m_color);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &ColoringNode::updateScale);
    connect(preview, &ColoringObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeResolution, preview, &ColoringObject::setResolution);
    connect(preview, &ColoringObject::textureChanged, this, &ColoringNode::setOutput);
    connect(this, &ColoringNode::colorChanged, preview, &ColoringObject::setColor);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/ColorProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startColor", m_color);
    connect(propertiesPanel, SIGNAL(colorChanged(QVector3D)), this, SLOT(updateColor(QVector3D)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(1, 1);
    setTitle("Coloring");
    m_socketsInput[0]->setTip("Texture");
}

ColoringNode::~ColoringNode() {
    delete preview;
}

void ColoringNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &ColoringNode::getPreviewTexture() {
    return preview->texture();
}

void ColoringNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 2;
    QJsonArray color;
    color.append(m_color.x());
    color.append(m_color.y());
    color.append(m_color.z());
    json["color"] = color;
}

void ColoringNode::deserialize(const QJsonObject &json){
    Node::deserialize(json);
    if(json.contains("color")) {
        QJsonArray color = json["color"].toArray();
        QVector3D c = QVector3D(color[0].toVariant().toFloat(), color[1].toVariant().toFloat(), color[2].toVariant().toFloat());
        updateColor(c);
        propertiesPanel->setProperty("startColor", c);
    }
}

QVector3D ColoringNode::color() {
    return m_color;
}

void ColoringNode::setColor(QVector3D color) {
    m_color = color;
    colorChanged(color);
}

void ColoringNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void ColoringNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void ColoringNode::updateColor(QVector3D color) {
    setColor(color);
    dataChanged();
}
