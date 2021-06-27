#include "hexagonsnode.h"

HexagonsNode::HexagonsNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int columns, int rows,
                           float size, float smooth, float mask, int seed): Node(parent, resolution, bpc),
    m_columns(columns), m_rows(rows), m_size(size), m_smooth(smooth), m_mask(mask), m_seed(seed)
{
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Hexagons");
    preview = new HexagonsObject(grNode, m_resolution, m_bpc, m_columns, m_rows, m_size, m_smooth, m_mask,
                                 m_seed);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &HexagonsNode::updateScale);
    connect(preview, &HexagonsObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &HexagonsObject::changedTexture, this, &HexagonsNode::setOutput);
    connect(this, &HexagonsNode::columnsChanged, preview, &HexagonsObject::setColumns);
    connect(this, &HexagonsNode::rowsChanged, preview, &HexagonsObject::setRows);
    connect(this, &HexagonsNode::hexSizeChanged, preview, &HexagonsObject::setHexSize);
    connect(this, &HexagonsNode::hexSmoothChanged, preview, &HexagonsObject::setHexSmooth);
    connect(this, &HexagonsNode::maskChanged, preview, &HexagonsObject::setMask);
    connect(this, &HexagonsNode::seedChanged, preview, &HexagonsObject::setSeed);
    connect(this, &Node::changeResolution, preview, &HexagonsObject::setResolution);
    connect(this, &Node::changeBPC, preview, &HexagonsObject::setBPC);
    connect(this, &Node::generatePreview, this, &HexagonsNode::previewGenerated);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/HexagonsProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startSize", m_size);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startMask", m_mask);
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(columnsChanged(int)), this, SLOT(updateColumns(int)));
    connect(propertiesPanel, SIGNAL(rowsChanged(int)), this, SLOT(updateRows(int)));
    connect(propertiesPanel, SIGNAL(hexSizeChanged(qreal)), this, SLOT(updateHexSize(qreal)));
    connect(propertiesPanel, SIGNAL(hexSmoothChanged(qreal)), this, SLOT(updateHexSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(maskChanged(qreal)), this, SLOT(updateMask(qreal)));
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

HexagonsNode::~HexagonsNode() {
    delete preview;
}

unsigned int &HexagonsNode::getPreviewTexture() {
    return preview->texture();
}

void HexagonsNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void HexagonsNode::operation() {
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
}

HexagonsNode *HexagonsNode::clone() {
    return new HexagonsNode(parentItem(), m_resolution, m_bpc, m_columns, m_rows, m_size, m_smooth, m_mask,
                            m_seed);
}

void HexagonsNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 32;
    json["columns"] = m_columns;
    json["rows"] = m_rows;
    json["size"] = m_size;
    json["smooth"] = m_smooth;
    json["mask"] = m_mask;
    json["seed"] = m_seed;
}

void HexagonsNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("columns")) {
        m_columns = json["columns"].toInt();
    }
    if(json.contains("rows")) {
        m_rows = json["rows"].toInt();
    }
    if(json.contains("size")) {
        m_size = json["size"].toVariant().toFloat();
    }
    if(json.contains("smooth")) {
        m_smooth = json["smooth"].toVariant().toFloat();
    }
    if(json.contains("mask")) {
        m_mask = json["mask"].toVariant().toFloat();
    }
    if(json.contains("seed")) {
        m_seed = json["seed"].toInt();
    }
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startSize", m_size);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startMask", m_mask);
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

int HexagonsNode::columns() {
    return m_columns;
}

void HexagonsNode::setColumns(int columns) {
    m_columns = columns;
    columnsChanged(columns);
}

int HexagonsNode::rows() {
    return m_rows;
}

void HexagonsNode::setRows(int rows) {
    m_rows = rows;
    rowsChanged(rows);
}

float HexagonsNode::hexSize() {
    return m_size;
}

void HexagonsNode::setHexSize(float size) {
    m_size = size;
    hexSizeChanged(size);
}

float HexagonsNode::hexSmooth() {
    return m_smooth;
}

void HexagonsNode::setHexSmooth(float smooth) {
    m_smooth = smooth;
    hexSmoothChanged(smooth);
}

float HexagonsNode::mask() {
    return m_mask;
}

void HexagonsNode::setMask(float mask) {
    m_mask = mask;
    maskChanged(mask);
}

int HexagonsNode::seed() {
    return m_seed;
}

void HexagonsNode::setSeed(int seed) {
    m_seed = seed;
    seedChanged(seed);
}

void HexagonsNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void HexagonsNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void HexagonsNode::updateColumns(int columns) {
    setColumns(columns);
    dataChanged();
}

void HexagonsNode::updateRows(int rows) {
    setRows(rows);
    dataChanged();
}

void HexagonsNode::updateHexSize(qreal size) {
    setHexSize(size);
    dataChanged();
}

void HexagonsNode::updateHexSmooth(qreal smooth) {
    setHexSmooth(smooth);
    dataChanged();
}

void HexagonsNode::updateMask(qreal mask) {
    setMask(mask);
    dataChanged();
}

void HexagonsNode::updateSeed(int seed) {
    setSeed(seed);
    dataChanged();
}

void HexagonsNode::previewGenerated() {
    preview->generatedTex = true;
    preview->update();
}
