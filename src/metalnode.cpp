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

#include "metalnode.h"

MetalNode::MetalNode(QQuickItem *parent, QVector2D resolution, GLint bpc): Node(parent, resolution, bpc)
{
    createSockets(1, 0);
    setTitle("Metalness");
    m_socketsInput[0]->setTip("Metalness");
    preview = new OneChanelObject(grNode, m_resolution, m_bpc);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &OneChanelObject::updatePreview, this, &MetalNode::updatePreview);
    connect(this, &Node::changeScaleView, this, &MetalNode::updateScale);
    connect(preview, &OneChanelObject::updateValue, this, &MetalNode::metalChanged);
    connect(this, &Node::changeResolution, preview, &OneChanelObject::setResolution);
    connect(this, &Node::changeBPC, preview, &OneChanelObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/MetalProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(metalChanged(qreal)), this, SLOT(updateMetal(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

MetalNode::~MetalNode() {
    delete preview;
}

void MetalNode::operation() {
    if(m_socketsInput[0]->countEdge() > 0) {
        preview->setValue(m_socketsInput[0]->value());
        preview->useTex = true;
    }
    else {
        preview->setValue(m_metal);
        preview->useTex = false;
    }
    preview->selectedItem = selected();
    preview->update();
}

unsigned int &MetalNode::getPreviewTexture() {
    return preview->texture();
}

void MetalNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void MetalNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 9;
    json["metal"] = m_metal;
}

void MetalNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("metal")) {
        updateMetal(json["metal"].toVariant().toFloat());
        propertiesPanel->setProperty("startMetal", m_metal);
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void MetalNode::updateMetal(qreal metal) {
    m_metal = metal;
    if(m_socketsInput[0]->countEdge() == 0) {
        operation();
    }
    dataChanged();
}

void MetalNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void MetalNode::saveMetal(QString dir) {
    preview->saveTexture(dir.append("/metalness.png"));
}
