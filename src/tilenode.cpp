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

#include "tilenode.h"
#include "frame.h"
#include <iostream>

TileNode::TileNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float offsetX, float offsetY,
                   int columns, int rows, float scale, float scaleX, float scaleY, int rotation,
                   float randPosition, float randRotation, float randScale, float maskStrength,
                   int inputsCount, int seed, bool keepProportion, bool useAlpha, bool depthMask):
    Node(parent, resolution, bpc), m_offsetX(offsetX), m_offsetY(offsetY), m_columns(columns), m_rows(rows),
    m_scaleX(scaleX), m_scaleY(scaleY), m_rotationAngle(rotation), m_randPosition(randPosition),
    m_randRotation(randRotation), m_randScale(randScale), m_maskStrength(maskStrength),
    m_inputsCount(inputsCount), m_seed(seed), m_scale(scale), m_keepProportion(keepProportion),
    m_useAlpha(useAlpha), m_depthMask(depthMask)
{
    preview = new TileObject(grNode, m_resolution, m_bpc, m_offsetX, m_offsetY, m_columns, m_rows, m_scale,
                             m_scaleX, m_scaleY, m_rotationAngle, m_randPosition, m_randRotation,
                             m_randScale, m_maskStrength, m_inputsCount, m_seed, m_keepProportion,
                             m_useAlpha);
    float s = scaleView();
    setHeight((207 + 28*(m_inputsCount - 1))*s);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &TileNode::updateScale);
    connect(this, &Node::generatePreview, this, &TileNode::previewGenerated);
    connect(preview, &TileObject::changedTexture, this, &TileNode::setOutput);
    connect(preview, &TileObject::updatePreview, this, &TileNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &TileObject::setResolution);
    connect(this, &Node::changeBPC, preview, &TileObject::setBPC);
    connect(this, &TileNode::offsetXChanged, preview, &TileObject::setOffsetX);
    connect(this, &TileNode::offsetYChanged, preview, &TileObject::setOffsetY);
    connect(this, &TileNode::columnsChanged, preview, &TileObject::setColumns);
    connect(this, &TileNode::rowsChanged, preview, &TileObject::setRows);
    connect(this, &TileNode::scaleXChanged, preview, &TileObject::setScaleX);
    connect(this, &TileNode::scaleYChanged, preview, &TileObject::setScaleY);
    connect(this, &TileNode::rotationAngleChanged, preview, &TileObject::setRotationAngle);
    connect(this, &TileNode::randPositionChanged, preview, &TileObject::setRandPosition);
    connect(this, &TileNode::randRotationChanged, preview, &TileObject::setRandRotation);
    connect(this, &TileNode::randScaleChanged, preview, &TileObject::setRandScale);
    connect(this, &TileNode::maskStrengthChanged, preview, &TileObject::setMaskStrength);
    connect(this, &TileNode::inputsCountChanged, preview, &TileObject::setInputsCount);
    connect(this, &TileNode::seedChanged, preview, &TileObject::setSeed);
    connect(this, &TileNode::tileScaleChanged, preview, &TileObject::setTileScale);
    connect(this, &TileNode::keepProportionChanged, preview, &TileObject::setKeepProportion);
    connect(this, &TileNode::useAlphaChanged, preview, &TileObject::setUseAlpha);
    connect(this, &TileNode::depthMaskChanged, preview, &TileObject::setDepthMask);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/TileProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startOffsetX", m_offsetX);
    propertiesPanel->setProperty("startOffsetY", m_offsetY);
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startTileScale", m_scale);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_rotationAngle);
    propertiesPanel->setProperty("startRandPosition", m_randPosition);
    propertiesPanel->setProperty("startRandRotation", m_randRotation);
    propertiesPanel->setProperty("startRandScale", m_randScale);
    propertiesPanel->setProperty("startMask", m_maskStrength);
    propertiesPanel->setProperty("startInputsCount", m_inputsCount);
    propertiesPanel->setProperty("startSeed", m_seed);
    propertiesPanel->setProperty("startKeepProportion", m_keepProportion);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);
    propertiesPanel->setProperty("startUseAlpha", m_depthMask);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(offsetXChanged(qreal)), this, SLOT(updateOffsetX(qreal)));
    connect(propertiesPanel, SIGNAL(offsetYChanged(qreal)), this, SLOT(updateOffsetY(qreal)));
    connect(propertiesPanel, SIGNAL(columnsChanged(int)), this, SLOT(updateColums(int)));
    connect(propertiesPanel, SIGNAL(rowsChanged(int)), this, SLOT(updateRows(int)));
    connect(propertiesPanel, SIGNAL(scaleXChanged(qreal)), this, SLOT(updateScaleX(qreal)));
    connect(propertiesPanel, SIGNAL(scaleYChanged(qreal)), this, SLOT(updateScaleY(qreal)));
    connect(propertiesPanel, SIGNAL(rotationAngleChanged(int)), this, SLOT(updateRotationAngle(int)));
    connect(propertiesPanel, SIGNAL(randPositionChanged(qreal)), this, SLOT(updateRandPosition(qreal)));
    connect(propertiesPanel, SIGNAL(randRotationChanged(qreal)), this, SLOT(updateRandRotation(qreal)));
    connect(propertiesPanel, SIGNAL(randScaleChanged(qreal)), this, SLOT(updateRandScale(qreal)));
    connect(propertiesPanel, SIGNAL(maskChanged(qreal)), this, SLOT(updateMaskStrength(qreal)));
    connect(propertiesPanel, SIGNAL(inputsCountChanged(int)), this, SLOT(updateInputsCount(int)));
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(scaleTileChanged(qreal)), this, SLOT(updateTileScale(qreal)));
    connect(propertiesPanel, SIGNAL(keepProportionChanged(bool)), this, SLOT(updateKeepProportion(bool)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(depthMaskChanged(bool)), this, SLOT(updateDepthMask(bool)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    createSockets(2, 1);
    createAdditionalInputs(5);
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < m_inputsCount - 1) {
            s->setVisible(true);
        }
        else {
            s->setVisible(false);
        }
    }
    setTitle("Tile");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    m_additionalInputs[0]->setTip("Texture");
    m_additionalInputs[1]->setTip("Texture");
    m_additionalInputs[2]->setTip("Texture");
    m_additionalInputs[3]->setTip("Texture");
    m_additionalInputs[4]->setTip("Texture");
}

TileNode::~TileNode() {
    delete preview;
}

void TileNode::operation() {
    preview->selectedItem = selected();
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *nodeInput0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput0 && nodeInput0->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *nodeInput1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput1 && nodeInput1->resolution() != m_resolution) return;
    }
    if(!m_additionalInputs[0]->getEdges().isEmpty()) {
        Node *nodeInput2 = static_cast<Node*>(m_additionalInputs[0]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput2 && nodeInput2->resolution() != m_resolution) return;
    }
    if(!m_additionalInputs[1]->getEdges().isEmpty()) {
        Node *nodeInput3 = static_cast<Node*>(m_additionalInputs[1]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput3 && nodeInput3->resolution() != m_resolution) return;
    }
    if(!m_additionalInputs[2]->getEdges().isEmpty()) {
        Node *nodeInput4 = static_cast<Node*>(m_additionalInputs[2]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput4 && nodeInput4->resolution() != m_resolution) return;
    }
    if(!m_additionalInputs[3]->getEdges().isEmpty()) {
        Node *nodeInput5 = static_cast<Node*>(m_additionalInputs[3]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput5 && nodeInput5->resolution() != m_resolution) return;
    }
    if(!m_additionalInputs[4]->getEdges().isEmpty()) {
        Node *nodeInput6 = static_cast<Node*>(m_additionalInputs[4]->getEdges()[0]->startSocket()->parentItem());
        if(nodeInput6 && nodeInput6->resolution() != m_resolution) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    preview->setTile1(m_additionalInputs[0]->value().toUInt());
    preview->setTile2(m_additionalInputs[1]->value().toUInt());
    preview->setTile3(m_additionalInputs[2]->value().toUInt());
    preview->setTile4(m_additionalInputs[3]->value().toUInt());
    preview->setTile5(m_additionalInputs[4]->value().toUInt());
}

unsigned int &TileNode::getPreviewTexture() {
    return preview->texture();
}

void TileNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void TileNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 17;
    json["offsetX"] = m_offsetX;
    json["offsetY"] = m_offsetY;
    json["columns"] = m_columns;
    json["rows"] = m_rows;
    json["scale"] = m_scale;
    json["scaleX"] = m_scaleX;
    json["scaleY"] = m_scaleY;
    json["rotation"] = m_rotationAngle;
    json["randPosition"] = m_randPosition;
    json["randRotation"] = m_randRotation;
    json["randScale"] = m_randScale;
    json["maskStrength"] = m_maskStrength;
    json["inputsCount"] = m_inputsCount;
    json["seed"] = m_seed;
    json["keepProportion"] = m_keepProportion;
    json["useAlpha"] = m_useAlpha;
    json["depthMask"] = m_depthMask;
}

void TileNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("offsetX")) {
        m_offsetX = json["offsetX"].toVariant().toFloat();
    }
    if(json.contains("offsetY")) {
        m_offsetY = json["offsetY"].toVariant().toFloat();
    }
    if(json.contains("columns")) {
        m_columns = json["columns"].toVariant().toInt();
    }
    if(json.contains("rows")) {
        m_rows = json["rows"].toVariant().toInt();
    }
    if(json.contains("scale")) {
        m_scale = json["scale"].toVariant().toFloat();
    }
    if(json.contains("scaleX")) {
        m_scaleX = json["scaleX"].toVariant().toFloat();
    }
    if(json.contains("scaleY")) {
        m_scaleY = json["scaleY"].toVariant().toFloat();
    }
    if(json.contains("rotation")) {
        m_rotationAngle = json["rotation"].toVariant().toFloat();
    }
    if(json.contains("randPosition")) {
        m_randPosition = json["randPosition"].toVariant().toFloat();
    }
    if(json.contains("randRotation")) {
        m_randRotation = json["randRotation"].toVariant().toFloat();
    }
    if(json.contains("randScale")) {
        m_randScale = json["randScale"].toVariant().toFloat();
    }
    if(json.contains("maskStrength")) {
        m_maskStrength = json["maskStrength"].toVariant().toFloat();
    }
    if(json.contains("inputsCount")) {
        m_inputsCount = json["inputsCount"].toInt();
    }
    if(json.contains("seed")) {
        m_seed = json["seed"].toInt();
    }
    if(json.contains("keepProportion")) {
        m_keepProportion = json["keepProportion"].toBool();
    }
    if(json.contains("useAlpha")) {
        m_useAlpha = json["useAlpha"].toBool();
    }
    if(json.contains("depthMask")) {
        m_depthMask = json["depthMask"].toBool();
    }
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < m_inputsCount - 1) {
            s->setVisible(true);
        }
        else {
            s->setVisible(false);
        }
    }
    propertiesPanel->setProperty("startOffsetX", m_offsetX);
    propertiesPanel->setProperty("startOffsetY", m_offsetY);
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startTileScale", m_scale);
    propertiesPanel->setProperty("startScaleX", m_scaleX);
    propertiesPanel->setProperty("startScaleY", m_scaleY);
    propertiesPanel->setProperty("startRotation", m_rotationAngle);
    propertiesPanel->setProperty("startRandPosition", m_randPosition);
    propertiesPanel->setProperty("startRandRotation", m_randRotation);
    propertiesPanel->setProperty("startRandScale", m_randScale);
    propertiesPanel->setProperty("startMask", m_maskStrength);
    propertiesPanel->setProperty("startInputsCount", m_inputsCount);
    propertiesPanel->setProperty("startSeed", m_seed);
    propertiesPanel->setProperty("startKeepProportion", m_keepProportion);
    propertiesPanel->setProperty("startUseAlpha", m_useAlpha);
    propertiesPanel->setProperty("startDepthMask", m_depthMask);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

float TileNode::offsetX() {
    return m_offsetX;
}

void TileNode::setOffsetX(float offset) {
    m_offsetX = offset;
    offsetXChanged(offset);
}

float TileNode::offsetY() {
    return m_offsetY;
}

void TileNode::setOffsetY(float offset) {
    m_offsetY = offset;
    offsetYChanged(offset);
}

int TileNode::columns() {
    return m_columns;
}

void TileNode::setColumns(int columns) {
    m_columns = columns;
    columnsChanged(columns);
}

int TileNode::rows() {
    return m_rows;
}

void TileNode::setRows(int rows) {
    m_rows = rows;
    rowsChanged(rows);
}

float TileNode::scaleX() {
    return m_scaleX;
}

void TileNode::setScaleX(float scale) {
    m_scaleX = scale;
    scaleXChanged(scale);
}

float TileNode::scaleY() {
    return m_scaleY;
}

void TileNode::setScaleY(float scale) {
    m_scaleY = scale;
    scaleYChanged(scale);
}

int TileNode::rotationAngle() {
    return m_rotationAngle;
}

void TileNode::setRotationAngle(int angle) {
    m_rotationAngle = angle;
    rotationAngleChanged(angle);
}

float TileNode::randPosition() {
    return m_randPosition;
}

void TileNode::setRandPosition(float rand) {
    m_randPosition = rand;
    randPositionChanged(rand);
}

float TileNode::randRotation() {
    return m_randRotation;
}

void TileNode::setRandRotation(float rand) {
    m_randRotation = rand;
    randRotationChanged(rand);
}

float TileNode::randScale() {
    return m_randScale;
}

void TileNode::setRandScale(float rand) {
    m_randScale = rand;
    randScaleChanged(rand);
}

float TileNode::maskStrength() {
    return m_maskStrength;
}

void TileNode::setMaskStrength(float mask) {
    m_maskStrength = mask;
    maskStrengthChanged(mask);
}

int TileNode::inputsCount() {
    return m_inputsCount;
}

void TileNode::setInputsCount(int count) {
    m_inputsCount = count;
    for(int i = 0; i < 5; ++i) {
        Socket *s = m_additionalInputs[i];
        if(i < count - 1) {
            s->setVisible(true);
            for(auto edge: s->getEdges()) {
                edge->setVisible(true);
            }
        }
        else {
            s->setVisible(false);
            for(auto edge: s->getEdges()) {
                edge->setVisible(false);
            }
        }
    }
    setHeight((207 + 28*(m_inputsCount - 1))*scaleView());
    inputsCountChanged(count);
}

int TileNode::seed() {
    return m_seed;
}

void TileNode::setSeed(int seed) {
    m_seed = seed;
    seedChanged(seed);
}

float TileNode::tileScale() {
    return m_scale;
}

void TileNode::setTileScale(float scale) {
    m_scale = scale;
    tileScaleChanged(scale);
}

bool TileNode::keepProportion() {
    return m_keepProportion;
}

void TileNode::setKeepProportion(bool keep) {
    m_keepProportion = keep;
    keepProportionChanged(keep);
}

bool TileNode::useAlpha() {
    return m_useAlpha;
}

void TileNode::setUseAlpha(bool use) {
    m_useAlpha = use;
    useAlphaChanged(use);
}

bool TileNode::depthMask() {
    return m_depthMask;
}

void TileNode::setDepthMask(bool depth) {
    m_depthMask = depth;
    depthMaskChanged(depth);
}

void TileNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void TileNode::updateScale(float scale) {
    setHeight((207 + 28*(m_inputsCount - 1))*scale);
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void TileNode::previewGenerated() {
    preview->tiledTex = true;
    preview->randUpdated = true;
    preview->update();
}

void TileNode::updateOffsetX(qreal offset) {
    setOffsetX(offset);
    operation();
    dataChanged();
}

void TileNode::updateOffsetY(qreal offset) {
    setOffsetY(offset);
    operation();
    dataChanged();
}

void TileNode::updateColums(int columns) {
    setColumns(columns);
    operation();
    dataChanged();
}

void TileNode::updateRows(int rows) {
    setRows(rows);
    operation();
    dataChanged();
}

void TileNode::updateScaleX(qreal scale) {
    setScaleX(scale);
    operation();
    dataChanged();
}

void TileNode::updateScaleY(qreal scale) {
    setScaleY(scale);
    operation();
    dataChanged();
}

void TileNode::updateRotationAngle(int angle) {
    setRotationAngle(angle);
    operation();
    dataChanged();
}

void TileNode::updateRandPosition(qreal rand) {
    setRandPosition(rand);
    operation();
    dataChanged();
}

void TileNode::updateRandRotation(qreal rand) {
    setRandRotation(rand);
    operation();
    dataChanged();
}

void TileNode::updateRandScale(qreal rand) {
    setRandScale(rand);
    operation();
    dataChanged();
}

void TileNode::updateMaskStrength(qreal mask) {
    setMaskStrength(mask);
    operation();
    dataChanged();
}

void TileNode::updateInputsCount(int count) {
    setInputsCount(count);
    if(attachedFrame()) attachedFrame()->resizeByContent();
    operation();
    dataChanged();
}

void TileNode::updateSeed(int seed) {
    setSeed(seed);
    operation();
    dataChanged();
}

void TileNode::updateTileScale(qreal scale) {
    setTileScale(scale);
    operation();
    dataChanged();
}

void TileNode::updateKeepProportion(bool keep) {
    setKeepProportion(keep);
    operation();
    dataChanged();
}

void TileNode::updateUseAlpha(bool use) {
    setUseAlpha(use);
    operation();
    dataChanged();
}

void TileNode::updateDepthMask(bool depth) {
    setDepthMask(depth);
    operation();
    dataChanged();
}
