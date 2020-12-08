#include "blurnode.h"
#include <iostream>

BlurNode::BlurNode(QQuickItem *parent, QVector2D resolution, float intensity): Node(parent, resolution),
    m_intensity(intensity)
{
    preview = new BlurObject(grNode, m_resolution, m_intensity);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &BlurNode::updateScale);
    connect(this, &BlurNode::changeSelected, this, &BlurNode::updatePrev);
    connect(preview, &BlurObject::textureChanged, this, &BlurNode::setOutput);
    connect(this, &Node::changeResolution, preview, &BlurObject::setResolution);
    connect(this, &BlurNode::intensityChanged, preview, &BlurObject::setIntensity);
    connect(preview, &BlurObject::updatePreview, this, &BlurNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BlurProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    createSockets(2, 1);
    setTitle("Blur");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

BlurNode::~BlurNode() {
    delete preview;
}

void BlurNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void BlurNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 19;
    json["intensity"] = m_intensity;
}

void BlurNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
        propertiesPanel->setProperty("startIntensity", m_intensity);
    }
}

float BlurNode::intensity() {
    return m_intensity;
}

void BlurNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

void BlurNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void BlurNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void BlurNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void BlurNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    operation();
    dataChanged();
}
