#include "heightnode.h"
#include <iostream>

HeightNode::HeightNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 0);
    setTitle("Height");
    m_socketsInput[0]->setTip("Height");
    preview = new NormalObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &NormalObject::updatePreview, this, &HeightNode::updatePreview);
    connect(preview, &NormalObject::updateNormal, this, &HeightNode::heightChanged);
    connect(this, &Node::changeScaleView, this, &HeightNode::updateScale);
    connect(this, &Node::changeResolution, preview, &NormalObject::setResolution);
}

HeightNode::~HeightNode() {
    delete preview;
}

void HeightNode::operation() {
    preview->setNormalTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
}

unsigned int &HeightNode::getPreviewTexture() {
    return preview->normalTexture();
}

void HeightNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void HeightNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 4;
}

void HeightNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void HeightNode::heightSave(QString dir) {
    preview->saveTexture(dir.append("/height.png"));
}
