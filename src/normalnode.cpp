#include "normalnode.h"
#include <iostream>

NormalNode::NormalNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 0);
    setTitle("Normal");
    m_socketsInput[0]->setTip("Normal");
    preview = new NormalObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &NormalObject::updatePreview, this, &NormalNode::updatePreview);
    connect(this, &NormalNode::changeSelected, this, &NormalNode::updatePrev);
    connect(this, &Node::changeScaleView, this, &NormalNode::updateScale);
    connect(preview, &NormalObject::updateNormal, this, &NormalNode::normalChanged);
    connect(this, &Node::changeResolution, preview, &NormalObject::setResolution);
}

NormalNode::~NormalNode() {
    delete preview;
}

void NormalNode::operation() {
    preview->setNormalTexture(m_socketsInput[0]->value().toUInt());
    preview->selectedItem = selected();
    preview->update();
}

void NormalNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 12;
}

void NormalNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->normalTexture(), true);
    }
    preview->selectedItem = selected();
}

void NormalNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void NormalNode::saveNormal(QString dir) {
    preview->texSaving = true;
    preview->saveName = dir.append("/normal.png");
    preview->update();
}
