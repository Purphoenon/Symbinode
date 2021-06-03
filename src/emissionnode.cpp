#include "emissionnode.h"

EmissionNode::EmissionNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 0);
    setTitle("Emission");
    m_socketsInput[0]->setTip("Emission");
    preview = new NormalObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &NormalObject::updatePreview, this, &EmissionNode::updatePreview);
    connect(preview, &NormalObject::updateNormal, this, &EmissionNode::emissionChanged);
    connect(this, &Node::changeScaleView, this, &EmissionNode::updateScale);
    connect(this, &Node::changeResolution, preview, &NormalObject::setResolution);
}

EmissionNode::~EmissionNode() {

}

void EmissionNode::operation() {
    preview->setNormalTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
}

unsigned int &EmissionNode::getPreviewTexture() {
    return preview->normalTexture();
}

void EmissionNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void EmissionNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 23;
}

void EmissionNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void EmissionNode::emissionSave(QString dir) {
    preview->saveTexture(dir.append("/emission.png"));
}
