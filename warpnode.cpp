#include "warpnode.h"

WarpNode::WarpNode(QQuickItem *parent, QVector2D resolution, float intensity): Node(parent, resolution),
    m_intensity(intensity)
{
    preview = new WarpObject(grNode, m_resolution, m_intensity);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &WarpNode::updateScale);
    connect(this, &WarpNode::changeSelected, this, &WarpNode::updatePrev);
    connect(preview, &WarpObject::changedTexture, this, &WarpNode::setOutput);
    connect(this, &Node::changeResolution, preview, &WarpObject::setResolution);
    connect(this, &WarpNode::intensityChanged, preview, &WarpObject::setIntensity);
    connect(preview, &WarpObject::updatePreview, this, &WarpNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/WarpProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    createSockets(3, 1);
    setTitle("Warp");
    m_socketsInput[0]->setTip("Source");
    m_socketsInput[1]->setTip("Factor");
    m_socketsInput[2]->setTip("Mask");
}

WarpNode::~WarpNode() {
    delete preview;
}

void WarpNode::operation() {
    preview->selectedItem = selected();
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setWarpTexture(m_socketsInput[1]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[2]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void WarpNode::serialize(QJsonObject &json) const{
    Node::serialize(json);
    json["type"] = 18;
    json["intensity"] = m_intensity;
}

void WarpNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    propertiesPanel->setProperty("startIntensity", m_intensity);
}

float WarpNode::intensity() {
    return m_intensity;
}

void WarpNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

void WarpNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void WarpNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void WarpNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void WarpNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    operation();
    dataChanged();
}
