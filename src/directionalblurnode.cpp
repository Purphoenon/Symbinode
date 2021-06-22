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
    connect(this, &Node::changeScaleView, this, &DirectionalBlurNode::updateScale);
    connect(preview, &DirectionalBlurObject::updatePreview, this, &Node::updatePreview);
    connect(this, &DirectionalBlurNode::intensityChanged, preview, &DirectionalBlurObject::setIntensity);
    connect(this, &DirectionalBlurNode::angleChanged, preview, &DirectionalBlurObject::setAngle);
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
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    preview->bluredTex = true;
    preview->update();
}

unsigned int &DirectionalBlurNode::getPreviewTexture() {
    return preview->texture();
}

void DirectionalBlurNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
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
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

float DirectionalBlurNode::intensity() {
    return m_intensity;
}

void DirectionalBlurNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

int DirectionalBlurNode::angle() {
    return m_angle;
}

void DirectionalBlurNode::setAngle(int angle) {
    m_angle = angle;
    angleChanged(angle);
}

void DirectionalBlurNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
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
