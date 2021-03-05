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

#include "albedonode.h"
#include <iostream>

AlbedoNode::AlbedoNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    preview = new AlbedoObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &AlbedoObject::updatePreview, this, &AlbedoNode::updatePreview);
    connect(this, &Node::changeScaleView, this, &AlbedoNode::updateScale);
    connect(preview, &AlbedoObject::updateAlbedo, this, &AlbedoNode::albedoChanged);
    connect(this, &Node::changeResolution, preview, &AlbedoObject::setResolution);
    createSockets(1, 0);
    setTitle("Albedo");
    m_socketsInput[0]->setTip("Albedo");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/AlbedoProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(albedoChanged(QVector3D)), this, SLOT(updateAlbedo(QVector3D)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

AlbedoNode::~AlbedoNode() {
    delete preview;
}

void AlbedoNode::operation() {
    if(m_socketsInput[0]->countEdge() > 0) {
        preview->useAlbedoTex = true;
        preview->setAlbedoTexture(m_socketsInput[0]->value().toUInt());
    }
    else {
        preview->useAlbedoTex = false;        
    }
    preview->setAlbedoValue(m_albedo);
    preview->selectedItem = selected();
    preview->update();
}

unsigned int &AlbedoNode::getPreviewTexture() {
    return preview->texture();
}

void AlbedoNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void AlbedoNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 8;
    QJsonArray albedo;
    albedo.append(m_albedo.x());
    albedo.append(m_albedo.y());
    albedo.append(m_albedo.z());
    json["albedo"] = albedo;
}

void AlbedoNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("albedo") && json["albedo"].isArray()) {
        QJsonArray albedo = json["albedo"].toArray();
        QVector3D color = QVector3D(albedo[0].toVariant().toFloat(), albedo[1].toVariant().toFloat(),
                             albedo[2].toVariant().toFloat());
        updateAlbedo(color);
        propertiesPanel->setProperty("startColor", color);
    }
}

void AlbedoNode::updateAlbedo(QVector3D color) {
    m_albedo = color;
    operation();
    /*if(m_socketsInput[0]->countEdge() == 0) {
        operation();
    }*/
    dataChanged();
}

void AlbedoNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void AlbedoNode::saveAlbedo(QString dir) {
    qDebug("save albedo");
    preview->saveTexture(dir.append("/albedo.png"));
}
