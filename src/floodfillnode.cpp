#include "floodfillnode.h"

FloodFillNode::FloodFillNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 1);
    setTitle("Flood Fill");
    m_socketsInput[0]->setTip("Texture");
    preview = new FloodFillObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &FloodFillObject::updatePreview, this, &FloodFillNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &FloodFillObject::setResolution);
    connect(preview, &FloodFillObject::textureChanged, this, &FloodFillNode::setOutput);
}

FloodFillNode::~FloodFillNode() {
    delete preview;
}

void FloodFillNode::operation() {
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

unsigned int &FloodFillNode::getPreviewTexture() {
    return preview->texture();
}

void FloodFillNode::serialize(QJsonObject &json) const{
    Node::serialize(json);
    json["type"] = 33;
}

FloodFillNode *FloodFillNode::clone() {
    return new FloodFillNode(parentItem(), m_resolution);
}

void FloodFillNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void FloodFillNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
