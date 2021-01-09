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

#include "colorrampnode.h"
#include <QQmlComponent>
#include <iostream>

ColorRampNode::ColorRampNode(QQuickItem *parent, QVector2D resolution, QJsonArray stops): Node(parent, resolution)
{
    preview = new ColorRampObject(grNode, m_resolution, stops);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &ColorRampNode::updateScale);
    connect(this, &Node::changeSelected, this, &ColorRampNode::updatePrev);
    connect(this, &Node::changeResolution, preview, &ColorRampObject::setResolution);
    connect(preview, &ColorRampObject::textureChanged, this, &ColorRampNode::setOutput);
    connect(preview, &ColorRampObject::updatePreview, this, &ColorRampNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/ColorRampProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(gradientStopAdded(QVector3D, qreal, int)), preview, SLOT(gradientAdd(QVector3D, qreal, int)));
    connect(propertiesPanel, SIGNAL(positionChanged(qreal, int)), preview, SLOT(positionUpdate(qreal, int)));
    connect(propertiesPanel, SIGNAL(colorChanged(QVector3D, int)), preview, SLOT(colorUpdate(QVector3D, int)));
    connect(propertiesPanel, SIGNAL(gradientStopDeleted(int)), preview, SLOT(gradientDelete(int)));
    connect(this, SIGNAL(stopsChanged(QVariant)), propertiesPanel, SIGNAL(gradientsStopsChanged(QVariant)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(2, 1);
    setTitle("Color Ramp");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    stopsChanged(QVariant(stops));
}

ColorRampNode::~ColorRampNode() {
    delete preview;
}

void ColorRampNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void ColorRampNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 0;
    json["gradientsStops"] = stops();
}

void ColorRampNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("gradientsStops")) {
        QJsonArray gradientStops = json["gradientsStops"].toVariant().toJsonArray();
        stopsChanged(QVariant(gradientStops));
        preview->setGradientsStops(gradientStops);
    }
}

QJsonArray ColorRampNode::stops() const{
    QList<QVariant> gradientsStops = propertiesPanel->property("gradientStops").toList();
    QJsonArray stops;
    for(auto s: gradientsStops) {
        QJsonArray i = s.toJsonArray();
        stops.push_back(i);
    }
    return stops;
}

void ColorRampNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void ColorRampNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void ColorRampNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
