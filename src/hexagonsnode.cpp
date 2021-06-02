#include "hexagonsnode.h"

HexagonsNode::HexagonsNode(QQuickItem *parent, QVector2D resolution, int columns, int rows, float size,
                           float smooth): Node(parent, resolution), m_columns(columns), m_rows(rows),
    m_size(size), m_smooth(smooth)
{
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Hexagons");
    preview = new HexagonsObject(grNode, m_resolution, m_columns, m_rows, m_size, m_smooth);
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
    connect(this, &Node::changeResolution, preview, &HexagonsObject::setResolution);
    connect(this, &Node::generatePreview, this, &HexagonsNode::previewGenerated);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/HexagonsProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startSize", m_size);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    connect(propertiesPanel, SIGNAL(columnsChanged(int)), this, SLOT(updateColumns(int)));
    connect(propertiesPanel, SIGNAL(rowsChanged(int)), this, SLOT(updateRows(int)));
    connect(propertiesPanel, SIGNAL(hexSizeChanged(qreal)), this, SLOT(updateHexSize(qreal)));
    connect(propertiesPanel, SIGNAL(hexSmoothChanged(qreal)), this, SLOT(updateHexSmooth(qreal)));
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

void HexagonsNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 32;
    json["columns"] = m_columns;
    json["rows"] = m_rows;
    json["size"] = m_size;
    json["smooth"] = m_smooth;
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
    propertiesPanel->setProperty("startColumns", m_columns);
    propertiesPanel->setProperty("startRows", m_rows);
    propertiesPanel->setProperty("startSize", m_size);
    propertiesPanel->setProperty("startSmooth", m_smooth);
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

void HexagonsNode::previewGenerated() {
    preview->generatedTex = true;
    preview->update();
}
