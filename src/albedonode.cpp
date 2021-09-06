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

AlbedoNode::AlbedoNode(QQuickItem *parent, QVector2D resolution, GLint bpc): Node(parent, resolution, bpc)
{
    preview = new AlbedoObject(grNode, m_resolution, m_bpc);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &AlbedoObject::updatePreview, this, &AlbedoNode::updatePreview);
    connect(preview, &AlbedoObject::updateAlbedo, this, &AlbedoNode::albedoChanged);
    connect(this, &Node::changeResolution, preview, &AlbedoObject::setResolution);
    connect(this, &Node::changeBPC, preview, &AlbedoObject::setBPC);
    createSockets(1, 0);
    setTitle("Albedo");
    m_socketsInput[0]->setTip("Albedo");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/AlbedoProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(albedoChanged(QVector3D)), this, SLOT(updateAlbedo(QVector3D)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

AlbedoNode::~AlbedoNode() {
    delete preview;
}

void AlbedoNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
        preview->useAlbedoTex = true;
        preview->setAlbedoTexture(m_socketsInput[0]->value().toUInt());
    }
    else {
        preview->useAlbedoTex = false;        
    }
    preview->setAlbedoValue(m_albedo);
    preview->albedoUpdated = true;
    preview->update();
    if(deserializing) deserializing = false;
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
        propertiesPanel->setProperty("startColor", color);
    }

    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void AlbedoNode::updateAlbedo(QVector3D color) {
    if(m_albedo == color) return;
    m_albedo = color;
    operation();
    dataChanged();
}

void AlbedoNode::saveAlbedo(QString dir) {
    qDebug("save albedo");
    preview->saveTexture(dir.append("/albedo.png"));
}
