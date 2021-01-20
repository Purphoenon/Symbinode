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

#include "colornode.h"

ColorNode::ColorNode(QQuickItem *parent, QVector2D resolution, QVector3D color): Node(parent, resolution),
    m_color(color)
{
    preview = new ColorObject(grNode, m_resolution, m_color);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &ColorNode::updateScale);
    connect(this, &Node::changeResolution, preview, &ColorObject::setResolution);
    connect(this, &ColorNode::generatePreview, this, &ColorNode::previewGenerated);
    connect(this, &ColorNode::colorChanged, preview, &ColorObject::setColor);
    connect(preview, &ColorObject::updatePreview, this, &ColorNode::updatePreview);
    connect(preview, &ColorObject::textureChanged, this, &ColorNode::operation);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/ColorProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startColor", m_color);
    connect(propertiesPanel, SIGNAL(colorChanged(QVector3D)), this, SLOT(updateColor(QVector3D)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(0, 1);
    setTitle("Color");
}

ColorNode::~ColorNode() {
    delete preview;
}

void ColorNode::operation() {
    m_socketOutput[0]->setValue(preview->texture());
}

unsigned int &ColorNode::getPreviewTexture() {
    return preview->texture();
}

void ColorNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 1;
    QJsonArray color;
    color.append(m_color.x());
    color.append(m_color.y());
    color.append(m_color.z());
    json["color"] = color;
}

void ColorNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("color")) {
        QJsonArray color = json["color"].toVariant().toJsonArray();
        QVector3D colorValue = QVector3D(color[0].toVariant().toFloat(), color[1].toVariant().toFloat(), color[2].toVariant().toFloat());
        updateColor(colorValue);
        propertiesPanel->setProperty("startColor", colorValue);
    }
}

QVector3D ColorNode::color() {
    return m_color;
}

void ColorNode::setColor(QVector3D color) {
    m_color = color;
    colorChanged(color);
}

void ColorNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void ColorNode::previewGenerated() {
    preview->createdTexture = true;
    update();
}

void ColorNode::updateColor(QVector3D color) {
    setColor(color);
    dataChanged();
}
