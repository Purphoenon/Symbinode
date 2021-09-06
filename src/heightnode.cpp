#include "heightnode.h"
#include <iostream>

HeightNode::HeightNode(QQuickItem *parent, QVector2D resolution, GLint bpc): Node(parent, resolution, bpc)
{
    createSockets(1, 0);
    setTitle("Height");
    m_socketsInput[0]->setTip("Height");
    preview = new NormalObject(grNode, m_resolution, m_bpc);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &NormalObject::updatePreview, this, &HeightNode::updatePreview);
    connect(preview, &NormalObject::updateNormal, this, &HeightNode::heightChanged);
    connect(this, &Node::changeResolution, preview, &NormalObject::setResolution);
    connect(this, &Node::changeBPC, preview, &NormalObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BitsProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

HeightNode::~HeightNode() {
    delete preview;
}

void HeightNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setNormalTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
    if(deserializing) deserializing = false;
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

void HeightNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void HeightNode::heightSave(QString dir) {
    preview->saveTexture(dir.append("/height.png"));
}
