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

#include "mappingnode.h"

MappingNode::MappingNode(QQuickItem *parent, QVector2D resolution, float inputMin, float inputMax,
                         float outputMin, float outputMax):Node(parent, resolution), m_inputMin(inputMin),
    m_inputMax(inputMax), m_outputMin(outputMin), m_outputMax(outputMax)
{
    preview = new MappingObject(grNode, m_resolution, m_inputMin, m_inputMax, m_outputMin, m_outputMax);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &MappingNode::updateScale);
    connect(this, &Node::generatePreview, this, &MappingNode::previewGenerated);
    connect(this, &Node::changeResolution, preview, &MappingObject::setResolution);
    connect(preview, &MappingObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &MappingObject::textureChanged, this, &MappingNode::setOutput);
    connect(this, &MappingNode::inputMinChanged, preview, &MappingObject::setInputMin);
    connect(this, &MappingNode::inputMaxChanged, preview, &MappingObject::setInputMax);
    connect(this, &MappingNode::outputMinChanged, preview, &MappingObject::setOutputMin);
    connect(this, &MappingNode::outputMaxChanged, preview, &MappingObject::setOutputMax);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/MappingProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(inputMinChanged(qreal)), this, SLOT(updateInputMin(qreal)));
    connect(propertiesPanel, SIGNAL(inputMaxChanged(qreal)), this, SLOT(updateInputMax(qreal)));
    connect(propertiesPanel, SIGNAL(outputMinChanged(qreal)), this, SLOT(updateOutputMin(qreal)));
    connect(propertiesPanel, SIGNAL(outputMaxChanged(qreal)), this, SLOT(updateOutputMax(qreal)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    propertiesPanel->setProperty("startInputMin", m_inputMin);
    propertiesPanel->setProperty("startInputMax", m_inputMax);
    propertiesPanel->setProperty("startOutputMin", m_outputMin);
    propertiesPanel->setProperty("startOutputMax", m_outputMax);
    createSockets(2, 1);
    setTitle("Map Range");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

MappingNode::~MappingNode() {
    delete preview;
}

void MappingNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &MappingNode::getPreviewTexture() {
    return preview->texture();
}

void MappingNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void MappingNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 3;
    json["inputMin"] = m_inputMin;
    json["inputMax"] = m_inputMax;
    json["outputMin"] = m_outputMin;
    json["outputMax"] = m_outputMax;
}

void MappingNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("inputMin")) {
        updateInputMin(json["inputMin"].toVariant().toReal());
        propertiesPanel->setProperty("startInputMin", m_inputMin);
    }
    if(json.contains("inputMax")) {
        updateInputMax(json["inputMax"].toVariant().toReal());
        propertiesPanel->setProperty("startInputMax", m_inputMax);
    }
    if(json.contains("outputMin")) {
        updateOutputMin(json["outputMin"].toVariant().toReal());
        propertiesPanel->setProperty("startOutputMin", m_outputMin);
    }
    if(json.contains("outputMax")) {
        updateOutputMax(json["outputMax"].toVariant().toReal());
        propertiesPanel->setProperty("startOutputMax", m_outputMax);
    }
}

float MappingNode::inputMin() {
    return m_inputMin;
}

void MappingNode::setInputMin(float value) {
    m_inputMin = value;
    inputMinChanged(value);
}

float MappingNode::inputMax() {
    return m_inputMax;
}

void MappingNode::setInputMax(float value) {
    m_inputMax = value;
    inputMaxChanged(value);
}

float MappingNode::outputMin() {
    return m_outputMin;
}

void MappingNode::setOutputMin(float value) {
    m_outputMin = value;
    outputMinChanged(value);
}

float MappingNode::outputMax() {
    return  m_outputMax;
}

void MappingNode::setOutputMax(float value) {
    m_outputMax = value;
    outputMaxChanged(value);
}

void MappingNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void MappingNode::previewGenerated() {
    preview->mappedTex = true;
    preview->update();
}

void MappingNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void MappingNode::updateInputMin(qreal value) {
    setInputMin(value);
    dataChanged();
}

void MappingNode::updateInputMax(qreal value) {
    setInputMax(value);
    dataChanged();
}

void MappingNode::updateOutputMin(qreal value) {
    setOutputMin(value);
    dataChanged();
}

void MappingNode::updateOutputMax(qreal value) {
    setOutputMax(value);
    dataChanged();
}
