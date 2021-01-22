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

#include "transformnode.h"

TransformNode::TransformNode(QQuickItem *parent, QVector2D resolution, float transX, float transY, float scaleX,
                             float scaleY, int angle, bool clamp): Node(parent, resolution), m_transX(transX),
    m_transY(transY), m_scaleX(scaleX), m_scaleY(scaleY), m_angle(angle), m_clamp(clamp)
{
    preview = new TransformObject(grNode, m_resolution, m_transX, m_transY, m_scaleX, m_scaleY, m_angle, m_clamp);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &TransformNode::updateScale);
    connect(preview, &TransformObject::textureChanged, this, &TransformNode::setOutput);
    connect(preview, &TransformObject::updatePreview, this, &TransformNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &TransformObject::setResolution);
    connect(this, &TransformNode::translationXChanged, preview, &TransformObject::setTranslateX);
    connect(this, &TransformNode::translationYChanged, preview, &TransformObject::setTranslateY);
    connect(this, &TransformNode::scaleXChanged, preview, &TransformObject::setScaleX);
    connect(this, &TransformNode::scaleYChanged, preview, &TransformObject::setScaleY);
    connect(this, &TransformNode::rotationChanged, preview, &TransformObject::setRotation);
    connect(this, &TransformNode::clampCoordsChanged, preview, &TransformObject::setClampCoords);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/TransformProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startTransX", m_transX);
    propertiesPanel->setProperty("startTransY", m_transY);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_angle);
    propertiesPanel->setProperty("startClamp", m_clamp);
    connect(propertiesPanel, SIGNAL(transXChanged(qreal)), this, SLOT(updateTranslationX(qreal)));
    connect(propertiesPanel, SIGNAL(transYChanged(qreal)), this, SLOT(updateTranslationY(qreal)));
    connect(propertiesPanel, SIGNAL(scaleXChanged(qreal)), this, SLOT(updateScaleX(qreal)));
    connect(propertiesPanel, SIGNAL(scaleYChanged(qreal)), this, SLOT(updateScaleY(qreal)));
    connect(propertiesPanel, SIGNAL(angleChanged(int)), this, SLOT(updateRotation(int)));
    connect(propertiesPanel, SIGNAL(clampCoordsChanged(bool)), this, SLOT(updateClampCoords(bool)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(2, 1);
    setTitle("Transform");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

TransformNode::~TransformNode() {
    delete preview;
}

void TransformNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &TransformNode::getPreviewTexture() {
    return preview->texture();
}

void TransformNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void TransformNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 16;
    json["transX"] = m_transX;
    json["transY"] = m_transY;
    json["scaleX"] = m_scaleX;
    json["scaleY"] = m_scaleY;
    json["angle"] = m_angle;
    json["clamp"] = m_clamp;
}

void TransformNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("transX")) {
        m_transX = json["transX"].toVariant().toFloat();
    }
    if(json.contains("transY")) {
        m_transY = json["transY"].toVariant().toFloat();
    }
    if(json.contains("scaleX")) {
        m_scaleX = json["scaleX"].toVariant().toFloat();
    }
    if(json.contains("scaleY")) {
        m_scaleY = json["scaleY"].toVariant().toFloat();
    }
    if(json.contains("angle")) {
        m_angle = json["angle"].toVariant().toInt();
    }
    if(json.contains("clamp")) {
        m_clamp = json["clamp"].toVariant().toBool();
    }
    propertiesPanel->setProperty("startTransX", m_transX);
    propertiesPanel->setProperty("startTransY", m_transY);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_angle);
    propertiesPanel->setProperty("startClamp", m_clamp);
}

float TransformNode::translationX() {
    return m_transX;
}

void TransformNode::setTranslationX(float x) {
    m_transX = x;
    translationXChanged(x);
}

float TransformNode::translationY() {
    return m_transY;
}

void TransformNode::setTranslationY(float y) {
    m_transY = y;
    translationYChanged(y);
}

float TransformNode::scaleX() {
    return m_scaleX;
}

void TransformNode::setScaleX(float x) {
    m_scaleX = x;
    scaleXChanged(x);
}

float TransformNode::scaleY() {
    return  m_scaleY;
}

void TransformNode::setScaleY(float y) {
    m_scaleY = y;
    scaleYChanged(y);
}

int TransformNode::rotation() {
    return m_angle;
}

void TransformNode::setRotation(int angle) {
    m_angle = angle;
    rotationChanged(angle);
}

bool TransformNode::clampCoords() {
    return m_clamp;
}

void TransformNode::setClampCoords(bool clamp) {
    m_clamp = clamp;
    clampCoordsChanged(clamp);
}

void TransformNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void TransformNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void TransformNode::updateTranslationX(qreal x) {
    setTranslationX(x);
    operation();
    dataChanged();
}

void TransformNode::updateTranslationY(qreal y) {
    setTranslationY(y);
    operation();
    dataChanged();
}

void TransformNode::updateScaleX(qreal x) {
    setScaleX(x);
    operation();
    dataChanged();
}

void TransformNode::updateScaleY(qreal y) {
    setScaleY(y);
    operation();
    dataChanged();
}

void TransformNode::updateRotation(int angle) {
    setRotation(angle);
    operation();
    dataChanged();
}

void TransformNode::updateClampCoords(bool clamp) {
    setClampCoords(clamp);
    operation();
    dataChanged();
}
