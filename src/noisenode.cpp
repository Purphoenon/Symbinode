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

#include "noisenode.h"
#include <iostream>

NoiseNode::NoiseNode(QQuickItem *parent, QVector2D resolution, GLint bpc, NoiseParams perlin,
                     NoiseParams simple, QString noiseType): Node(parent, resolution, bpc),
    m_noiseType(noiseType), perlinNoise(perlin), simpleNoise(simple)
{
    preview = new NoiseObject(grNode, m_resolution, m_bpc, m_noiseType, noiseScale(), scaleX(), scaleY(), layers(), persistence(), amplitude(), seed());
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &NoiseNode::generatePreview, this, &NoiseNode::previewGenerated);
    connect(this, &NoiseNode::noiseTypeChanged, preview, &NoiseObject::setNoiseType);
    connect(preview, &NoiseObject::updatePreview, this, &NoiseNode::updatePreview);
    connect(preview, &NoiseObject::changedTexture, this, &NoiseNode::setOutput);
    connect(this, &Node::changeResolution, preview, &NoiseObject::setResolution);
    connect(this, &Node::changeBPC, preview, &NoiseObject::setBPC);
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Noise");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/NoiseProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    int index = 0;
    if(m_noiseType == "noiseSimple") index = 1;
    propertiesPanel->setProperty("type", index);
    propertiesPanel->setProperty("startNoiseScale", noiseScale());
    propertiesPanel->setProperty("startScaleX", scaleX());
    propertiesPanel->setProperty("startScaleY", scaleY());
    propertiesPanel->setProperty("startLayers", layers());
    propertiesPanel->setProperty("startPersistence", persistence());
    propertiesPanel->setProperty("startAmplitude", amplitude());
    propertiesPanel->setProperty("startSeed", seed());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(noiseScaleChanged(qreal)), this, SLOT(updateNoiseScale(qreal)));
    connect(propertiesPanel, SIGNAL(scaleXChanged(qreal)), this, SLOT(updateScaleX(qreal)));
    connect(propertiesPanel, SIGNAL(scaleYChanged(qreal)), this, SLOT(updateScaleY(qreal)));
    connect(propertiesPanel, SIGNAL(layersChanged(int)), this, SLOT(updateLayers(int)));
    connect(propertiesPanel, SIGNAL(persistenceChanged(qreal)), this, SLOT(updatePersistence(qreal)));
    connect(propertiesPanel, SIGNAL(amplitudeChanged(qreal)), this, SLOT(updateAmplitude(qreal)));
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(noiseTypeChanged(QString)), this, SLOT(updateNoiseType(QString)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

NoiseNode::~NoiseNode() {
    delete preview;
}

QString NoiseNode::noiseType() {
    return m_noiseType;
}

void NoiseNode::setNoiseType(QString type) {
    if(m_noiseType == type) return;
    m_noiseType = type;    
    propertiesPanel->setProperty("startNoiseScale", noiseScale());
    propertiesPanel->setProperty("startScaleX", scaleX());
    propertiesPanel->setProperty("startScaleY", scaleY());
    propertiesPanel->setProperty("startLayers", layers());
    propertiesPanel->setProperty("startPersistence", persistence());
    propertiesPanel->setProperty("startAmplitude", amplitude());
    propertiesPanel->setProperty("startSeed", seed());
    noiseTypeChanged(type);

    preview->setNoiseScale(noiseScale());
    preview->setScaleX(scaleX());
    preview->setScaleY(scaleY());
    preview->setLayers(layers());
    preview->setPersistence(persistence());
    preview->setAmplitude(amplitude());
    preview->setSeed(seed());
    preview->update();
}

NoiseParams NoiseNode::perlinParams() {
    return perlinNoise;
}

NoiseParams NoiseNode::simpleParams() {
    return  simpleNoise;
}

int NoiseNode::noiseScale() {
    if(m_noiseType == "noisePerlin") return perlinNoise.scale;
    else if(m_noiseType == "noiseSimple") return simpleNoise.scale;
    return perlinNoise.scale;
}

void NoiseNode::setNoiseScale(int scale) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.scale == scale) return;
        perlinNoise.scale = scale;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.scale == scale) return;
        simpleNoise.scale = scale;
    }
    noiseScaleChanged(scale);
    preview->setNoiseScale(scale);
    preview->update();
}

int NoiseNode::scaleX() {
    if(m_noiseType == "noisePerlin") return perlinNoise.scaleX;
    else if(m_noiseType == "noiseSimple") return simpleNoise.scaleX;
    return perlinNoise.scaleX;
}

void NoiseNode::setScaleX(int scale) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.scaleX == scale) return;
        perlinNoise.scaleX = scale;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.scaleX == scale) return;
        simpleNoise.scaleX = scale;
    }
    scaleXChanged(scale);
    preview->setScaleX(scale);
    preview->update();
}

int NoiseNode::scaleY() {
    if(m_noiseType == "noisePerlin") return perlinNoise.scaleY;
    else if(m_noiseType == "noiseSimple") return simpleNoise.scaleY;
    return perlinNoise.scaleY;
}

void NoiseNode::setScaleY(int scale) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.scaleY == scale) return;
        perlinNoise.scaleY = scale;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.scaleY == scale) return;
        simpleNoise.scaleY = scale;
    }
    scaleYChanged(scale);
    preview->setScaleY(scale);
    preview->update();
}

int NoiseNode::layers() {
    if(m_noiseType == "noisePerlin") return perlinNoise.layers;
    else if(m_noiseType == "noiseSimple") return simpleNoise.layers;
    return perlinNoise.layers;
}

void NoiseNode::setLayers(int num) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.layers == num) return;
        perlinNoise.layers = num;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.layers == num) return;
        simpleNoise.layers = num;
    }
    layersChanged(num);
    preview->setLayers(num);
    preview->update();
}

float NoiseNode::persistence() {
    if(m_noiseType == "noisePerlin") return perlinNoise.persistence;
    else if(m_noiseType == "noiseSimple") return simpleNoise.persistence;
    return perlinNoise.persistence;
}

void NoiseNode::setPersistence(float value) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.persistence == value) return;
        perlinNoise.persistence = value;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.persistence == value) return;
        simpleNoise.persistence = value;
    }
    persistenceChanged(value);
    preview->setPersistence(value);
    preview->update();
}

float NoiseNode::amplitude() {
    if(m_noiseType == "noisePerlin") return perlinNoise.amplitude;
    else if(m_noiseType == "noiseSimple") return simpleNoise.amplitude;
    return perlinNoise.amplitude;
}

void NoiseNode::setAmplitude(float value) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.amplitude == value) return;
        perlinNoise.amplitude = value;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.amplitude == value) return;
        simpleNoise.amplitude = value;
    }
    amplitudeChanged(value);
    preview->setAmplitude(value);
    preview->update();
}

int NoiseNode::seed() {
    if(m_noiseType == "noisePerlin") return perlinNoise.seed;
    else if(m_noiseType == "noiseSimple") return simpleNoise.seed;
    return perlinNoise.seed;
}

void NoiseNode::setSeed(int seed) {
    if(m_noiseType == "noisePerlin") {
        if(perlinNoise.seed == seed) return;
        perlinNoise.seed = seed;
    }
    else if(m_noiseType == "noiseSimple") {
        if(simpleNoise.seed == seed) return;
        simpleNoise.seed = seed;
    }
    seedChanged(seed);
    preview->setSeed(seed);
    preview->update();
}

unsigned int &NoiseNode::getPreviewTexture() {
    return preview->texture();
}

void NoiseNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void NoiseNode::operation() {
    preview->selectedItem = selected();
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value().toUInt() == 0 && deserializing) return;
    }
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
    m_socketOutput[0]->setValue(preview->texture());
    deserializing = false;
}

NoiseNode *NoiseNode::clone() {
    return new NoiseNode(parentItem(), m_resolution, m_bpc, perlinNoise, simpleNoise, m_noiseType);
}

void NoiseNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 6;
    QJsonObject perlinParams;
    perlinParams["scale"] = perlinNoise.scale;
    perlinParams["scaleX"] = perlinNoise.scaleX;
    perlinParams["scaleY"] = perlinNoise.scaleY;
    perlinParams["persistence"] = perlinNoise.persistence;
    perlinParams["amplitude"] = perlinNoise.amplitude;
    perlinParams["layers"] = perlinNoise.layers;
    perlinParams["seed"] = perlinNoise.seed;
    json["perlinParams"] = perlinParams;
    QJsonObject simpleParams;
    simpleParams["scale"] = simpleNoise.scale;
    simpleParams["scaleX"] = simpleNoise.scaleX;
    simpleParams["scaleY"] = simpleNoise.scaleY;
    simpleParams["persistence"] = simpleNoise.persistence;
    simpleParams["amplitude"] = simpleNoise.amplitude;
    simpleParams["layers"] = simpleNoise.layers;
    simpleParams["seed"] = simpleNoise.seed;
    json["simpleParams"] = simpleParams;
    json["noiseType"] = m_noiseType;
}

void NoiseNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("perlinParams")) {
        QJsonObject perlinParams = json["perlinParams"].toObject();
        if(perlinParams.contains("scale")) {
            perlinNoise.scale = perlinParams["scale"].toVariant().toInt();
        }
        if(perlinParams.contains("scaleX")) {
            perlinNoise.scaleX = perlinParams["scaleX"].toInt();
        }
        if(perlinParams.contains("scaleY")) {
            perlinNoise.scaleY = perlinParams["scaleY"].toInt();
        }
        if(perlinParams.contains("persistence")) {
            perlinNoise.persistence = perlinParams["persistence"].toVariant().toFloat();
        }
        if(perlinParams.contains("amplitude")) {
            perlinNoise.amplitude = perlinParams["amplitude"].toVariant().toFloat();
        }
        if(perlinParams.contains("layers")) {
            perlinNoise.layers = perlinParams["layers"].toVariant().toInt();
        }
        if(perlinParams.contains("seed")) {
            perlinNoise.seed = perlinParams["seed"].toInt();
        }
    }
    if(json.contains("simpleParams")) {
        QJsonObject simpleParams = json["simpleParams"].toObject();
        if(simpleParams.contains("scale")) {
            simpleNoise.scale = simpleParams["scale"].toVariant().toInt();
        }
        if(simpleParams.contains("scaleX")) {
            simpleNoise.scaleX = simpleParams["scaleX"].toInt();
        }
        if(simpleParams.contains("scaleY")) {
            simpleNoise.scaleY = simpleParams["scaleY"].toInt();
        }
        if(simpleParams.contains("persistence")) {
            simpleNoise.persistence = simpleParams["persistence"].toVariant().toFloat();
        }
        if(simpleParams.contains("amplitude")) {
            simpleNoise.amplitude = simpleParams["amplitude"].toVariant().toFloat();
        }
        if(simpleParams.contains("layers")) {
            simpleNoise.layers = simpleParams["layers"].toVariant().toInt();
        }
        if(simpleParams.contains("seed")) {
            simpleNoise.seed = simpleParams["seed"].toInt();
        }
    }
    if(json.contains("noiseType")) {
        m_noiseType = json["noiseType"].toVariant().toString();
        noiseTypeChanged(m_noiseType);
        int index = 0;
        if(m_noiseType == "noiseSimple") index = 1;
        propertiesPanel->setProperty("type", index);
        propertiesPanel->setProperty("startNoiseScale", noiseScale());
        propertiesPanel->setProperty("startScaleX", scaleX());
        propertiesPanel->setProperty("startScaleY", scaleY());
        propertiesPanel->setProperty("startLayers", layers());
        propertiesPanel->setProperty("startPersistence", persistence());
        propertiesPanel->setProperty("startAmplitude", amplitude());
        propertiesPanel->setProperty("startSeed", seed());

        preview->setNoiseScale(noiseScale());
        preview->setScaleX(scaleX());
        preview->setScaleY(scaleY());
        preview->setLayers(layers());
        preview->setPersistence(persistence());
        preview->setAmplitude(amplitude());
        preview->setSeed(seed());
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

void NoiseNode::updateNoiseType(QString type) {
    setNoiseType(type);    
    dataChanged();
}

void NoiseNode::updateNoiseScale(qreal scale) {
    setNoiseScale(scale);
    dataChanged();
}

void NoiseNode::updateScaleX(qreal scale) {
    setScaleX(scale);
    dataChanged();
}

void NoiseNode::updateScaleY(qreal scale) {
    setScaleY(scale);
    dataChanged();
}

void NoiseNode::updateLayers(int layers) {
    setLayers(layers);
    dataChanged();
}

void NoiseNode::updatePersistence(qreal value) {
    setPersistence(value);
    dataChanged();
}

void NoiseNode::updateAmplitude(qreal value) {
    setAmplitude(value);
    dataChanged();
}

void NoiseNode::updateSeed(int seed) {
    setSeed(seed);
    dataChanged();
}

void NoiseNode::previewGenerated() {
    preview->generatedNoise = true;
    preview->update();
}

void NoiseNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
