#include "floodfilltogradientnode.h"

FloodFillToGradientNode::FloodFillToGradientNode(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                                 int rotation, float randomizing, int seed): Node (parent,
                                                 resolution, bpc), m_rotation(rotation),
    m_randomizing(randomizing), m_seed(seed)
{
    createSockets(1, 1);
    setTitle("Flood Fill to Gradient");
    m_socketsInput[0]->setTip("Flood Fill");
    preview = new FloodFillToGradientObject(grNode, m_resolution, m_bpc, m_rotation, m_randomizing, m_seed);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);
    connect(preview, &FloodFillToGradientObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &FloodFillToGradientObject::textureChanged, this, &FloodFillToGradientNode::setOutput);
    connect(this, &Node::changeResolution, preview, &FloodFillToGradientObject::setResolution);
    connect(this, &Node::changeBPC, preview, &FloodFillToGradientObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/FloodFillToGradientProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startRotation", m_rotation);
    propertiesPanel->setProperty("startRandomizing", m_randomizing);
    propertiesPanel->setProperty("startSeed", m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(rotationAngleChanged(int)), this, SLOT(updateRotation(int)));
    connect(propertiesPanel, SIGNAL(randomizingChanged(qreal)), this, SLOT(updateRandomizing(qreal)));
    connect(propertiesPanel, SIGNAL(seedChanged(int)), this, SLOT(updateSeed(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

FloodFillToGradientNode::~FloodFillToGradientNode() {
    delete preview;
}

void FloodFillToGradientNode::operation() {
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

unsigned int &FloodFillToGradientNode::getPreviewTexture() {
    return preview->texture();
}

void FloodFillToGradientNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 34;
    json["rotation"] = m_rotation;
    json["randomizing"] = m_randomizing;
    json["seed"] = m_seed;
}

void FloodFillToGradientNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("rotation")) {
        m_rotation = json["rotation"].toInt();
    }
    if(json.contains("randomizing")) {
        m_randomizing = json["randomizing"].toVariant().toFloat();
    }
    if(json.contains("seed")) {
        m_seed = json["seed"].toInt();
    }
    propertiesPanel->setProperty("startRotation", m_rotation);
    propertiesPanel->setProperty("startRandomizing", m_randomizing);
    propertiesPanel->setProperty("startSeed", m_seed);
    preview->setRotation(m_rotation);
    preview->setRandomizing(m_randomizing);
    preview->setSeed(m_seed);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

FloodFillToGradientNode *FloodFillToGradientNode::clone() {
    return new FloodFillToGradientNode(parentItem(), m_resolution, m_bpc, m_rotation, m_randomizing, m_seed);
}

void FloodFillToGradientNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

int FloodFillToGradientNode::rotation() {
    return m_rotation;
}

void FloodFillToGradientNode::setRotation(int angle) {
    if(m_rotation == angle) return;
    m_rotation = angle;
    preview->setRotation(m_rotation);
    preview->update();
}

float FloodFillToGradientNode::randomizing() {
    return m_randomizing;
}

void FloodFillToGradientNode::setRandomizing(float rand) {
    if(m_randomizing == rand) return;
    m_randomizing = rand;
    preview->setRandomizing(m_randomizing);
    preview->update();
}

int FloodFillToGradientNode::seed() {
    return m_seed;
}

void FloodFillToGradientNode::setSeed(int seed) {
    if(m_seed == seed) return;
    m_seed = seed;
    preview->setSeed(m_seed);
    preview->update();
}

void FloodFillToGradientNode::updateRotation(int angle) {
    setRotation(angle);
    dataChanged();
}

void FloodFillToGradientNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void FloodFillToGradientNode::updateRandomizing(qreal rand) {
    setRandomizing(rand);
    dataChanged();
}

void FloodFillToGradientNode::updateSeed(int seed) {
    setSeed(seed);
    dataChanged();
}
