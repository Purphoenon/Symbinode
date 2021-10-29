#include "floodfilltograyscalenode.h"

FloodFillToGrayscaleNode::FloodFillToGrayscaleNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int seed):
    Node (parent, resolution, bpc), m_seed(seed)
{
    createSockets(1, 1);
    setTitle("Flood Fill to Grayscale");
    m_socketsInput[0]->setTip("Flood Fill");
    preview = new FloodFillToGrayscaleObject(grNode, m_resolution, m_bpc);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);
    connect(preview, &FloodFillToGrayscaleObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &FloodFillToGrayscaleObject::textureChanged, this, &FloodFillToGrayscaleNode::setOutput);
    connect(this, &Node::changeResolution, preview, &FloodFillToGrayscaleObject::setResolution);
    connect(this, &Node::changeBPC, preview, &FloodFillToGrayscaleObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/FloodFillToGrayscaleProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

FloodFillToGrayscaleNode::~FloodFillToGrayscaleNode() {
    delete preview;
}

void FloodFillToGrayscaleNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    if(deserializing) deserializing = false;
}

unsigned int &FloodFillToGrayscaleNode::getPreviewTexture() {
    return preview->texture();
}

void FloodFillToGrayscaleNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 35;
    json["seed"] = m_seed;
}

void FloodFillToGrayscaleNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("seed")) {
        m_seed = json["seed"].toInt();
    }
    propertiesPanel->setProperty("startSeed", m_seed);
    preview->setSeed(m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

FloodFillToGrayscaleNode *FloodFillToGrayscaleNode::clone() {
    return new FloodFillToGrayscaleNode(parentItem(), m_resolution, m_bpc, m_seed);
}

void FloodFillToGrayscaleNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

int FloodFillToGrayscaleNode::seed() {
    return m_seed;
}

void FloodFillToGrayscaleNode::setSeed(int seed) {
    if(m_seed == seed) return;
    m_seed = seed;
    preview->setSeed(m_seed);
    preview->update();
}

void FloodFillToGrayscaleNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void FloodFillToGrayscaleNode::updateSeed(int seed) {
    setSeed(seed);
    dataChanged();
}
