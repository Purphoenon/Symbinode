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

PolygonNode::PolygonNode(QQuickItem *parent, QVector2D resolution, int sides, float polygonScale,
                         float smooth, bool useAlpha): Node(parent, resolution), m_sides(sides),
    m_polygonScale(polygonScale), m_smooth(smooth), m_useAlpha(useAlpha)
{
    preview = new PolygonObject(grNode, m_resolution, m_sides, m_polygonScale, m_smooth, m_useAlpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &PolygonNode::updateScale);
    connect(this, &PolygonNode::changeSelected, this, &PolygonNode::updatePrev);
    connect(this, &PolygonNode::generatePreview, this, &PolygonNode::previewGenerated);
    connect(preview, &PolygonObject::changedTexture, this, &PolygonNode::setOutput);
    connect(preview, &PolygonObject::updatePreview, this, &PolygonNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &PolygonObject::setResolution);
    connect(this, &PolygonNode::sidesChanged, preview, &PolygonObject::setSides);
    connect(this, &PolygonNode::polygonScaleChanged, preview, &PolygonObject::setPolygonScale);
    connect(this, &PolygonNode::smoothChanged, preview, &PolygonObject::setSmooth);
    connect(this, &PolygonNode::useAlphaChanged, preview, &PolygonObject::setUseAlpha);
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
    connect(propertiesPanel, SIGNAL(sidesChanged(int)), this, SLOT(updateSides(int)));
    connect(propertiesPanel, SIGNAL(polygonScaleChanged(qreal)), this, SLOT(updatePolygonScale(qreal)));
    connect(propertiesPanel, SIGNAL(polygonSmoothChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

PolygonNode::~PolygonNode() {
    delete preview;
}

void PolygonNode::operation() {
    preview->selectedItem = selected();
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
}

void PolygonNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 14;
    json["sides"] = m_sides;
    json["scale"] = m_polygonScale;
    json["smooth"] = m_smooth;
    json["useAlpha"] = m_useAlpha;
}

void PolygonNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
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
}

int PolygonNode::sides() {
    return m_sides;
}

void PolygonNode::setSides(int sides) {
    m_sides = sides;
    sidesChanged(sides);
}

float PolygonNode::polygonScale() {
    return m_polygonScale;
}

void PolygonNode::setPolygonScale(float scale) {
    m_polygonScale = scale;
    polygonScaleChanged(scale);
}

float PolygonNode::smooth() {
    return m_smooth;
}

void PolygonNode::setSmooth(float smooth) {
    m_smooth = smooth;
    smoothChanged(smooth);
}

bool PolygonNode::useAlpha() {
    return m_useAlpha;
}

void PolygonNode::setUseAlpha(bool use) {
    m_useAlpha = use;
    useAlphaChanged(use);
}

void PolygonNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void PolygonNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
        updatePreview(preview->texture(), true);
    }
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
