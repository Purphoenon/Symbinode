#include "directionalblurnode.h"

DirectionalBlurNode::DirectionalBlurNode(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                         float intensity, int angle): Node(parent, resolution, bpc),
    m_intensity(intensity), m_angle(angle)
{
    createSockets(2, 1);
    setTitle("Directional Blur");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    preview = new DirectionalBlurObject(grNode, m_resolution, m_bpc, m_intensity, m_angle);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &DirectionalBlurObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &DirectionalBlurObject::textureChanged, this, &DirectionalBlurNode::setOutput);
    connect(this, &DirectionalBlurNode::changeResolution, preview, &DirectionalBlurObject::setResolution);
    connect(this, &Node::changeBPC, preview, &DirectionalBlurObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/DirectionalBlurProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startAngle", m_angle);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(angleChanged(int)), this, SLOT(updateAngle(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

DirectionalBlurNode::~DirectionalBlurNode() {
    delete preview;
}

void DirectionalBlurNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
        if(m_socketsInput[1]->value() == 0 && deserializing) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    preview->bluredTex = true;
    preview->update();
    if(deserializing) deserializing = false;
}

unsigned int &DirectionalBlurNode::getPreviewTexture() {
    return preview->texture();
}

void DirectionalBlurNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

DirectionalBlurNode *DirectionalBlurNode::clone() {
    return new DirectionalBlurNode(parentItem(), m_resolution, m_bpc, m_intensity, m_angle);
}

void DirectionalBlurNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 27;
    json["intensity"] = m_intensity;
    json["angle"] = m_angle;
}

void DirectionalBlurNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    if(json.contains("angle")) {
        m_angle = json["angle"].toInt();
    }
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startAngle", m_angle);

    preview->setIntensity(m_intensity);
    preview->setAngle(m_angle);

    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);

    preview->update();
}

float DirectionalBlurNode::intensity() {
    return m_intensity;
}

void DirectionalBlurNode::setIntensity(float intensity) {
    if(m_intensity == intensity) return;
    m_intensity = intensity;
    intensityChanged(intensity);
    preview->setIntensity(intensity);
    preview->update();
}

int DirectionalBlurNode::angle() {
    return m_angle;
}

void DirectionalBlurNode::setAngle(int angle) {
    if(m_angle == angle) return;
    m_angle = angle;
    angleChanged(angle);
    preview->setAngle(angle);
    preview->update();
}

void DirectionalBlurNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    dataChanged();
}

void DirectionalBlurNode::updateAngle(int angle) {
    setAngle(angle);
    dataChanged();
}

void DirectionalBlurNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
