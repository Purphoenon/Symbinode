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

#include "mixnode.h"
#include <iostream>

MixNode::MixNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float factor, int foregroundOpacity,
                 int backgroundOpacity, int mode, bool includingAlpha): Node(parent, resolution, bpc),
    m_factor(factor), m_fOpacity(foregroundOpacity), m_bOpacity(backgroundOpacity), m_mode(mode),
    m_includingAlpha(includingAlpha)
{
    preview = new MixObject(grNode, m_resolution, m_bpc, m_factor, m_fOpacity, m_bOpacity, m_mode,
                            m_includingAlpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    createSockets(4, 1);
    setTitle("Mix");
    m_socketsInput[0]->setTip("Background");
    m_socketsInput[1]->setTip("Foreground");
    m_socketsInput[2]->setTip("Factor");
    m_socketsInput[3]->setTip("Mask");
    connect(this, &Node::changeScaleView, this, &MixNode::updateScale);
    connect(preview, &MixObject::textureChanged, this, &MixNode::setOutput);
    connect(this, &MixNode::generatePreview, this, &MixNode::previewGenerated);
    connect(preview, &MixObject::updatePreview, this, &MixNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &MixObject::setResolution);
    connect(this, &Node::changeBPC, preview, &MixObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/MixProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(factorChanged(qreal)), this, SLOT(updateFactor(qreal)));
    connect(propertiesPanel, SIGNAL(modeChanged(int)), this, SLOT(updateMode(int)));
    connect(propertiesPanel, SIGNAL(includingAlphaChanged(bool)), this, SLOT(updateIncludingAlpha(bool)));
    connect(propertiesPanel, SIGNAL(foregroundOpacityChanged(int)), this, SLOT(updateForegroundOpacity(int)));
    connect(propertiesPanel, SIGNAL(backgroundOpacityChanged(int)), this, SLOT(updateBackgroundOpacity(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    propertiesPanel->setProperty("startFactor", m_factor);
    propertiesPanel->setProperty("startMode", m_mode);
    propertiesPanel->setProperty("startIncludingAlpha", m_includingAlpha);
    propertiesPanel->setProperty("startForegroundOpacity", m_fOpacity);
    propertiesPanel->setProperty("startBackgroundOpacity", m_bOpacity);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

MixNode::~MixNode() {
    delete preview;
}

void MixNode::operation() {

    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[2]->getEdges().isEmpty()) {
        Node *inputNode2 = static_cast<Node*>(m_socketsInput[2]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode2 && inputNode2->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[3]->getEdges().isEmpty()) {
        Node *inputNode3 = static_cast<Node*>(m_socketsInput[3]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode3 && inputNode3->resolution() != m_resolution) return;
    }

    preview->setFirstTexture((m_socketsInput[0]->value().toUInt()));
    preview->setSecondTexture((m_socketsInput[1]->value().toUInt()));
    preview->setMaskTexture(m_socketsInput[3]->value().toUInt());

    if(m_socketsInput[2]->countEdge() > 0) {
        preview->useFactorTexture = true;
        preview->setFactor(m_socketsInput[2]->value());
    }
    else {
        preview->useFactorTexture = false;
        preview->setFactor(m_factor);
    }
    preview->setMode(m_mode);
    preview->setIncludingAlpha(m_includingAlpha);
    preview->setForegroundOpacity(m_fOpacity);
    preview->setBackgroundOpacity(m_bOpacity);
    preview->mixedTex = true;
    preview->selectedItem = selected();
    preview->update();

    if(m_socketsInput[0]->countEdge() == 0 && m_socketsInput[1]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &MixNode::getPreviewTexture() {
    return preview->texture();
}

void MixNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

float MixNode::factor() {
    return m_factor;
}

void MixNode::setFactor(float f) {
    m_factor = f;
    factorChanged(f);
}

int MixNode::mode() {
    return m_mode;
}

void MixNode::setMode(int mode) {
    m_mode = mode;
    modeChanged(mode);
}

bool MixNode::includingAlpha() {
    return m_includingAlpha;
}

void MixNode::setIncludingAlpha(bool including) {
    m_includingAlpha = including;
    includingAlphaChanged(including);
}

int MixNode::foregroundOpacity() {
    return m_fOpacity;
}

void MixNode::setForegroundOpacity(int opacity) {
    m_fOpacity = opacity;
    foregroundOpacityChanged(opacity);
}

int MixNode::backgroundOpacity() {
    return m_bOpacity;
}

void MixNode::setBackgroundOpacity(int opacity) {
    m_bOpacity = opacity;
    backgroundOpacityChanged(opacity);
}

void MixNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

MixNode *MixNode::clone() {
    return new MixNode(parentItem(), m_resolution, m_bpc, m_factor, m_fOpacity, m_bOpacity, m_mode,
                       m_includingAlpha);
}

void MixNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 7;
    json["factor"] = m_factor;
    json["mode"] = m_mode;
    json["includingAlpha"] = m_includingAlpha;
    json["foregroundOpacity"] = m_fOpacity;
    json["backgroundOpacity"] = m_bOpacity;
}

void MixNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("factor")) {
        setFactor(json["factor"].toVariant().toFloat());
        propertiesPanel->setProperty("startFactor", m_factor);
    }
    if(json.contains("mode")) {
        setMode(json["mode"].toInt());
        propertiesPanel->setProperty("startMode", m_mode);
    }
    if(json.contains("includingAlpha")) {
        setIncludingAlpha(json["includingAlpha"].toBool());
        propertiesPanel->setProperty("startIncludingAlpha", m_includingAlpha);
    }
    if(json.contains("foregroundOpacity")) {
        setForegroundOpacity(json["foregroundOpacity"].toInt());
        propertiesPanel->setProperty("startForegroundOpacity", m_fOpacity);
    }
    if(json.contains("backgroundOpacity")) {
        setBackgroundOpacity(json["backgroundOpacity"].toInt());
        propertiesPanel->setProperty("startBackgroundOpacity", m_bOpacity);
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void MixNode::updateFactor(qreal f) {
    setFactor(static_cast<float>(f));
    if(m_socketsInput[2]->countEdge() == 0) {
        operation();
    }
    dataChanged();
}

void MixNode::updateMode(int mode) {
    setMode(mode);
    operation();
    dataChanged();
}

void MixNode::updateIncludingAlpha(bool including) {
    setIncludingAlpha(including);
    operation();
    dataChanged();
}

void MixNode::updateForegroundOpacity(int opacity) {
    setForegroundOpacity(opacity);
    operation();
    dataChanged();
}

void MixNode::updateBackgroundOpacity(int opacity) {
    setBackgroundOpacity(opacity);
    operation();
    dataChanged();
}

void MixNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void MixNode::previewGenerated() {
    preview->mixedTex = true;
    preview->update();
}
