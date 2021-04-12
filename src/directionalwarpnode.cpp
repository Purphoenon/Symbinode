#include "directionalwarpnode.h"

DirectionalWarpNode::DirectionalWarpNode(QQuickItem *parent, QVector2D resolution, float intensity, int angle):
    Node (parent, resolution), m_intensity(intensity), m_angle(angle)
{
    createSockets(3, 1);
    setTitle("Directional Warp");
    m_socketsInput[0]->setTip("Source");
    m_socketsInput[1]->setTip("Factor");
    m_socketsInput[2]->setTip("Mask");
    preview = new DirectionalWarpObject(grNode, m_resolution, m_intensity, m_angle);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &DirectionalWarpNode::updateScale);
    connect(this, &DirectionalWarpNode::intensityChanged, preview, &DirectionalWarpObject::setIntensity);
    connect(this, &DirectionalWarpNode::angleChanged, preview, &DirectionalWarpObject::setAngle);
    connect(preview, &DirectionalWarpObject::changedTexture, this, &DirectionalWarpNode::setOutput);
    connect(preview, &DirectionalWarpObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeResolution, preview, &DirectionalWarpObject::setResolution);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/DirectionalWarpProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startAngle", m_angle);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(angleChanged(int)), this, SLOT(updateAngle(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

DirectionalWarpNode::~DirectionalWarpNode() {
    delete preview;
}

void DirectionalWarpNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setWarpTexture(m_socketsInput[1]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[2]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &DirectionalWarpNode::getPreviewTexture() {
    return preview->texture();
}

void DirectionalWarpNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void DirectionalWarpNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 26;
    json["intensity"] = m_intensity;
    json["angle"] = m_angle;
}

void DirectionalWarpNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    if(json.contains("angle")) {
        m_angle = json["angle"].toInt();
    }
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startAngle", m_angle);
}

float DirectionalWarpNode::intensity() {
    return m_intensity;
}

void DirectionalWarpNode::setIntnsity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

int DirectionalWarpNode::angle() {
    return m_angle;
}

void DirectionalWarpNode::setAngle(int angle) {
    m_angle = angle;
    angleChanged(angle);
}

void DirectionalWarpNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void DirectionalWarpNode::updateIntensity(qreal intensity) {
    setIntnsity(intensity);
    dataChanged();
}

void DirectionalWarpNode::updateAngle(int angle) {
    setAngle(angle);
    dataChanged();
}

void DirectionalWarpNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
