#include "grayscalenode.h"

GrayscaleNode::GrayscaleNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 1);
    setTitle("Grayscale");
    m_socketsInput[0]->setTip("Texture");
    preview = new GrayscaleObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &GrayscaleNode::updateScale);
    connect(this, &Node::generatePreview, this, &GrayscaleNode::previewGenerated);
    connect(this, &Node::changeResolution, preview, &GrayscaleObject::setResolution);
    connect(preview, &GrayscaleObject::textureChanged, this, &GrayscaleNode::setOutput);
    connect(preview, &GrayscaleObject::updatePreview, this, &GrayscaleNode::updatePreview);
}

GrayscaleNode::~GrayscaleNode() {
    delete preview;
}

void GrayscaleNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &GrayscaleNode::getPreviewTexture() {
    return preview->texture();
}

void GrayscaleNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void GrayscaleNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 24;
}

void GrayscaleNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void GrayscaleNode::previewGenerated() {
    preview->grayscaledTex = true;
    preview->update();
}

void GrayscaleNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
