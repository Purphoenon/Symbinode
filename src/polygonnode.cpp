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

#include "polygonnode.h"
#include <iostream>

PolygonNode::PolygonNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int sides, float polygonScale,
                         float smooth, bool useAlpha): Node(parent, resolution, bpc), m_sides(sides),
    m_polygonScale(polygonScale), m_smooth(smooth), m_useAlpha(useAlpha)
{
    preview = new PolygonObject(grNode, m_resolution, m_bpc, m_sides, m_polygonScale, m_smooth, m_useAlpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &PolygonNode::generatePreview, this, &PolygonNode::previewGenerated);
    connect(preview, &PolygonObject::changedTexture, this, &PolygonNode::setOutput);
    connect(preview, &PolygonObject::updatePreview, this, &PolygonNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &PolygonObject::setResolution);
    connect(this, &Node::changeBPC, preview, &PolygonObject::setBPC);
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Polygon");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/PolygonProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startSides", m_sides);
    propertiesPanel->setProperty("startScale", m_polygonScale);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);
    propertiesPanel->setProperty("startBits", m_useAlpha);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(sidesChanged(int)), this, SLOT(updateSides(int)));
    connect(propertiesPanel, SIGNAL(polygonScaleChanged(qreal)), this, SLOT(updatePolygonScale(qreal)));
    connect(propertiesPanel, SIGNAL(polygonSmoothChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

PolygonNode::~PolygonNode() {
    delete preview;
}

void PolygonNode::operation() {
    preview->selectedItem = selected();

    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && (inputNode->resolution() == m_resolution)) {
            preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
        }        
    }
    deserializing = false;
}

unsigned int &PolygonNode::getPreviewTexture() {
    return preview->texture();
}

void PolygonNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

PolygonNode *PolygonNode::clone() {
    return new PolygonNode(parentItem(), m_resolution, m_bpc, m_sides, m_polygonScale, m_smooth, m_useAlpha);
}

void PolygonNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 14;
    json["sides"] = m_sides;
    json["scale"] = m_polygonScale;
    json["smooth"] = m_smooth;
    json["useAlpha"] = m_useAlpha;
}

void PolygonNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("sides")) {
        m_sides = json["sides"].toVariant().toInt();
    }
    if(json.contains("scale")) {
        m_polygonScale = json["scale"].toVariant().toFloat();
    }
    if(json.contains("smooth")) {
        m_smooth = json["smooth"].toVariant().toFloat();
    }
    if(json.contains("useAlpha")) {
        m_useAlpha = json["useAlpha"].toBool();
    }
    propertiesPanel->setProperty("startSides", m_sides);
    propertiesPanel->setProperty("startScale", m_polygonScale);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);

    preview->setSides(m_sides);
    preview->setPolygonScale(m_polygonScale);
    preview->setSmooth(m_smooth);
    preview->setUseAlpha(m_useAlpha);
    preview->update();
}

int PolygonNode::sides() {
    return m_sides;
}

void PolygonNode::setSides(int sides) {
    if(m_sides == sides) return;
    m_sides = sides;
    sidesChanged(sides);
    preview->setSides(sides);
    preview->update();
}

float PolygonNode::polygonScale() {
    return m_polygonScale;
}

void PolygonNode::setPolygonScale(float scale) {
    if(m_polygonScale == scale) return;
    m_polygonScale = scale;
    polygonScaleChanged(scale);
    preview->setPolygonScale(scale);
    preview->update();
}

float PolygonNode::smooth() {
    return m_smooth;
}

void PolygonNode::setSmooth(float smooth) {
    if(m_smooth == smooth) return;
    m_smooth = smooth;
    smoothChanged(smooth);
    preview->setSmooth(smooth);
    preview->update();
}

bool PolygonNode::useAlpha() {
    return m_useAlpha;
}

void PolygonNode::setUseAlpha(bool use) {
    if(m_useAlpha == use) return;
    m_useAlpha = use;
    useAlphaChanged(use);
    preview->setUseAlpha(use);
    preview->update();
}

void PolygonNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void PolygonNode::previewGenerated() {
    preview->generatedPolygon = true;
    preview->update();
}

void PolygonNode::updateSides(int sides) {
    setSides(sides);
    dataChanged();
}

void PolygonNode::updatePolygonScale(qreal scale) {
    setPolygonScale(scale);
    dataChanged();
}

void PolygonNode::updateSmooth(qreal smooth) {
    setSmooth(smooth);
    dataChanged();
}

void PolygonNode::updateUseAlpha(bool use) {
    setUseAlpha(use);
    dataChanged();
}
