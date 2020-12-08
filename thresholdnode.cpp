#include "thresholdnode.h"

ThresholdNode::ThresholdNode(QQuickItem *parent, QVector2D resolution, float threshold):
    Node(parent, resolution), m_threshold(threshold)
{
    preview = new ThresholdObject(grNode, m_resolution, m_threshold);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &ThresholdNode::updateScale);
    connect(this, &Node::changeSelected, this, &ThresholdNode::updatePrev);
    connect(preview, &ThresholdObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &ThresholdObject::textureChanged, this, &ThresholdNode::setOutput);
    connect(this, &ThresholdNode::thresholdChanged, preview, &ThresholdObject::setThreshold);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/ThresholdProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startThreshold", m_threshold);
    connect(propertiesPanel, SIGNAL(thresholdChanged(qreal)), this, SLOT(updateThreshold(qreal)));
    createSockets(2, 1);
    setTitle("Threshold");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

ThresholdNode::~ThresholdNode() {
    delete preview;
}

void ThresholdNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void ThresholdNode::serialize(QJsonObject &json) const{
    Node::serialize(json);
    json["type"] = 22;
    json["threshold"] = m_threshold;
}

void ThresholdNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("threshold")) {
        updateThreshold(json["threshold"].toVariant().toReal());
        propertiesPanel->setProperty("startThreshold", m_threshold);
    }
}

float ThresholdNode::threshold() {
    return m_threshold;
}

void ThresholdNode::setThreshold(float value) {
    m_threshold = value;
    thresholdChanged(value);
}

void ThresholdNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void ThresholdNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) updatePreview(preview->texture(), true);
}

void ThresholdNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void ThresholdNode::updateThreshold(qreal value) {
    setThreshold(value);
    dataChanged();
}
