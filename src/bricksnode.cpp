#include "bricksnode.h"

BricksNode::BricksNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int columns, int rows,
                       float offset, float width, float height, float smoothX, float smoothY, float mask,
                       int seed): Node (parent, resolution, bpc), m_columns(columns), m_rows(rows),
    m_offset(offset), m_width(width), m_height(height), m_mask(mask), m_smoothX(smoothX), m_smoothY(smoothY),
    m_seed(seed)
{
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Bricks");
    preview = new BricksObject(grNode, m_resolution, m_bpc, m_columns, m_rows, m_offset, m_width, m_height,
                               m_smoothX, m_smoothY, m_mask, m_seed);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &BricksNode::updateScale);
    connect(preview, &BricksObject::changedTexture, this, &BricksNode::setOutput);
    connect(this, &BricksNode::columnsChanged, preview, &BricksObject::setColumns);
    connect(this, &BricksNode::rowsChanged, preview, &BricksObject::setRows);
    connect(this, &BricksNode::offsetChanged, preview, &BricksObject::setOffset);
    connect(this, &BricksNode::bricksWidthChanged, preview, &BricksObject::setBricksWidth);
    connect(this, &BricksNode::bricksHeightChanged, preview, &BricksObject::setBricksHeight);
    connect(this, &BricksNode::maskChanged, preview, &BricksObject::setMask);
    connect(this, &BricksNode::smoothXChanged, preview, &BricksObject::setSmoothX);
    connect(this, &BricksNode::smoothYChanged, preview, &BricksObject::setSmoothY);
    connect(this, &BricksNode::seedChanged, preview, &BricksObject::setSeed);
    connect(preview, &BricksObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeResolution, preview, &BricksObject::setResolution);
    connect(this, &Node::changeBPC, preview, &BricksObject::setBPC);
    connect(this, &Node::generatePreview, this, &BricksNode::previewGenerated);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BricksProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startOffset", m_offset);
    propertiesPanel->setProperty("startWidth", m_width);
    propertiesPanel->setProperty("startHeight", m_height);
    propertiesPanel->setProperty("startSmoothX", m_smoothX);
    propertiesPanel->setProperty("startSmothY", m_smoothY);
    propertiesPanel->setProperty("startMask", m_mask);
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(columnsChanged(int)), this, SLOT(updateColumns(int)));
    connect(propertiesPanel, SIGNAL(rowsChanged(int)), this, SLOT(updateRows(int)));
    connect(propertiesPanel, SIGNAL(offsetChanged(qreal)), this, SLOT(updateOffset(qreal)));
    connect(propertiesPanel, SIGNAL(bricksWidthChanged(qreal)), this, SLOT(updateBricksWidth(qreal)));
    connect(propertiesPanel, SIGNAL(bricksHeightChanged(qreal)), this, SLOT(updateBricksHeight(qreal)));
    connect(propertiesPanel, SIGNAL(maskChanged(qreal)), this, SLOT(updateMask(qreal)));
    connect(propertiesPanel, SIGNAL(smoothXChanged(qreal)), this, SLOT(updateSmoothX(qreal)));
    connect(propertiesPanel, SIGNAL(smoothYChanged(qreal)), this, SLOT(updateSmoothY(qreal)));
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

BricksNode::~BricksNode() {
    delete preview;
}

unsigned int &BricksNode::getPreviewTexture() {
    return preview->texture();
}

void BricksNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void BricksNode::operation() {
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
}

void BricksNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 31;
    json["columns"] = m_columns;
    json["rows"] = m_rows;
    json["offset"] = m_offset;
    json["width"] = m_width;
    json["height"] = m_height;
    json["smoothX"] = m_smoothX;
    json["smoothY"] = m_smoothY;
    json["mask"] = m_mask;
    json["seed"] = m_seed;
}

void BricksNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("columns")) {
        m_columns = json["columns"].toInt();
    }
    if(json.contains("rows")) {
        m_rows = json["rows"].toInt();
    }
    if(json.contains("offset")) {
        m_offset = json["offset"].toVariant().toFloat();
    }
    if(json.contains("width")) {
        m_width = json["width"].toVariant().toFloat();
    }
    if(json.contains("height")) {
        m_height = json["height"].toVariant().toFloat();
    }
    if(json.contains("smoothX")) {
        m_smoothX = json["smoothX"].toVariant().toFloat();
    }
    if(json.contains("smoothY")) {
        m_smoothY = json["smoothY"].toVariant().toFloat();
    }
    if(json.contains("mask")) {
        m_mask = json["mask"].toVariant().toFloat();
    }
    if(json.contains("seed")) {
        m_seed = json["seed"].toInt();
    }
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startOffset", m_offset);
    propertiesPanel->setProperty("startWidth", m_width);
    propertiesPanel->setProperty("startHeight", m_height);
    propertiesPanel->setProperty("startSmoothX", m_smoothX);
    propertiesPanel->setProperty("startSmothY", m_smoothY);
    propertiesPanel->setProperty("startMask", m_mask);
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

int BricksNode::columns() {
    return m_columns;
}

void BricksNode::setColumns(int columns) {
    m_columns = columns;
    columnsChanged(columns);
}

int BricksNode::rows() {
    return m_rows;
}

void BricksNode::setRows(int rows) {
    m_rows = rows;
    rowsChanged(rows);
}

float BricksNode::offset() {
    return m_offset;
}

void BricksNode::setOffset(float offset) {
    m_offset = offset;
    offsetChanged(offset);
}

float BricksNode::bricksWidth() {
    return m_width;
}

void BricksNode::setBricksWidth(float width) {
    m_width = width;
    bricksWidthChanged(width);
}

float BricksNode::bricksHeight() {
    return m_height;
}

void BricksNode::setBricksHeight(float height) {
    m_height = height;
    bricksHeightChanged(height);
}

float BricksNode::mask() {
    return m_mask;
}

void BricksNode::setMask(float mask) {
    m_mask = mask;
    maskChanged(mask);
}

float BricksNode::smoothX() {
    return m_smoothX;
}

void BricksNode::setSmoothX(float smooth) {
    m_smoothX = smooth;
    smoothXChanged(smooth);
}

float BricksNode::smoothY() {
    return m_smoothY;
}

void BricksNode::setSmoothY(float smooth) {
    m_smoothY = smooth;
    smoothYChanged(smooth);
}

int BricksNode::seed() {
    return m_seed;
}

void BricksNode::setSeed(int seed) {
    m_seed = seed;
    seedChanged(seed);
}

void BricksNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void BricksNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void BricksNode::updateColumns(int columns) {
    setColumns(columns);
    dataChanged();
}

void BricksNode::updateRows(int rows) {
    setRows(rows);
    dataChanged();
}

void BricksNode::updateOffset(qreal offset) {
    setOffset(offset);
    dataChanged();
}

void BricksNode::updateBricksWidth(qreal width) {
    setBricksWidth(width);
    dataChanged();
}

void BricksNode::updateBricksHeight(qreal height) {
    setBricksHeight(height);
    dataChanged();
}

void BricksNode::updateMask(qreal mask) {
    setMask(mask);
    dataChanged();
}

void BricksNode::updateSmoothX(qreal smooth) {
    setSmoothX(smooth);
    dataChanged();
}

void BricksNode::updateSmoothY(qreal smooth) {
    setSmoothY(smooth);
    dataChanged();
}

void BricksNode::updateSeed(int seed) {
    setSeed(seed);
    dataChanged();
}

void BricksNode::previewGenerated() {
    preview->generatedTex = true;
    preview->update();
}
