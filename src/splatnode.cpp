#include "splatnode.h"

SplatNode::SplatNode(QQuickItem *parent, QVector2D resolution, int size, int imagePerCell):
    Node(parent, resolution), m_size(size), m_imagePerCell(imagePerCell)
{
    preview = new SplatObject(grNode, m_resolution, m_size, m_imagePerCell);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &SplatNode::updateScale);
    connect(this, &Node::changeSelected, this, &SplatNode::updatePrev);
    connect(preview, &SplatObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &SplatObject::textureChanged, this, &SplatNode::setOutput);
    connect(this, &Node::changeResolution, preview, &SplatObject::setResolution);
    connect(this, &SplatNode::sizeChanged, preview, &SplatObject::setSize);
    connect(this, &SplatNode::imagePerCellChanged, preview, &SplatObject::setImagePerCell);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BombingProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(cellSizeChanged(int)), this, SLOT(updateSize(int)));
    connect(propertiesPanel, SIGNAL(imagePerCellChanged(int)), this, SLOT(updateImagePerCell(int)));
    propertiesPanel->setProperty("startSize", m_size);
    propertiesPanel->setProperty("startImagePerCell", m_imagePerCell);
    createSockets(2, 1);
    setTitle("Splat");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

SplatNode::~SplatNode() {
    delete preview;
}

void SplatNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    //m_socketOutput[0]->setValue(preview->texture());
}

void SplatNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 4;
    json["size"] = m_size;
    json["imagePerCell"] = m_imagePerCell;
}

void SplatNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("size")) {
        updateSize(json["size"].toVariant().toInt());
        propertiesPanel->setProperty("startSize", m_size);
    }
    if(json.contains("imagePerCell")) {
        updateImagePerCell(json["imagePerCell"].toVariant().toInt());
        propertiesPanel->setProperty("startImagePerCell", m_imagePerCell);
    }
}

int SplatNode::size() {
    return m_size;
}

void SplatNode::setSize(int size) {
    m_size = size;
    sizeChanged(size);
}

int SplatNode::imagePerCell() {
    return m_imagePerCell;
}

void SplatNode::setImagePerCell(int count) {
    m_imagePerCell = count;
    imagePerCellChanged(count);
}

void SplatNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void SplatNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void SplatNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void SplatNode::updateSize(int size) {
    setSize(size);
    dataChanged();
}

void SplatNode::updateImagePerCell(int count) {
    setImagePerCell(count);
    dataChanged();
}
