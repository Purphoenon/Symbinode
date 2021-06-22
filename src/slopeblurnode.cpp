#include "slopeblurnode.h"

SlopeBlurNode::SlopeBlurNode(QQuickItem *parent, QVector2D resolution, GLint bpc, int mode, float intensity, int samples):
    Node(parent, resolution, bpc), m_mode(mode), m_intensity(intensity), m_samples(samples)
{
    createSockets(3, 1);
    setTitle("Slope Blur");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Slope");
    m_socketsInput[2]->setTip("Mask");
    preview = new SlopeBlurObject(grNode, m_resolution, m_bpc, m_mode, m_intensity, m_samples);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &SlopeBlurNode::updateScale);
    connect(preview, &SlopeBlurObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &SlopeBlurObject::textureChanged, this, &SlopeBlurNode::setOutput);
    connect(this, &SlopeBlurNode::modeChanged, preview, &SlopeBlurObject::setMode);
    connect(this, &SlopeBlurNode::intensityChanged, preview, &SlopeBlurObject::setIntensity);
    connect(this, &SlopeBlurNode::samplesChanged, preview, &SlopeBlurObject::setSamples);
    connect(this, &Node::changeResolution, preview, &SlopeBlurObject::setResolution);
    connect(this, &Node::changeBPC, preview, &SlopeBlurObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/SlopeBlurProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("mode", m_mode);
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startSamples", m_samples);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(blendModeChanged(int)), this, SLOT(updateMode(int)));
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(samplesChanged(int)), this, SLOT(updateSamples(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

SlopeBlurNode::~SlopeBlurNode() {
    delete preview;
}

void SlopeBlurNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode0 = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode0 && inputNode0->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
    }
    if(!m_socketsInput[2]->getEdges().isEmpty()) {
        Node *inputNode2 = static_cast<Node*>(m_socketsInput[2]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode2 && inputNode2->resolution() != m_resolution) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setSlopeTexture(m_socketsInput[1]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[2]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    preview->slopedTex = true;
    preview->update();
}

unsigned int &SlopeBlurNode::getPreviewTexture() {
    return preview->texture();
}

void SlopeBlurNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

void SlopeBlurNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 28;
    json["mode"] = m_mode;
    json["intensity"] = m_intensity;
    json["samples"] = m_samples;
}

void SlopeBlurNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("mode")) {
        m_mode = json["mode"].toInt();
    }
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    if(json.contains("samples")) {
        m_samples = json["samples"].toInt();
    }
    propertiesPanel->setProperty("mode", m_mode);
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startSamples", m_samples);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

int SlopeBlurNode::mode() {
    return m_mode;
}

void SlopeBlurNode::setMode(int mode) {
    m_mode = mode;
    modeChanged(mode);
}

float SlopeBlurNode::intensity() {
    return m_intensity;
}

void SlopeBlurNode::setIntensity(float intensity) {
    m_intensity = intensity;
    intensityChanged(intensity);
}

int SlopeBlurNode::samples() {
    return m_samples;
}

void SlopeBlurNode::setSamples(int samples) {
    m_samples = samples;
    samplesChanged(samples);
}

void SlopeBlurNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void SlopeBlurNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void SlopeBlurNode::updateMode(int mode) {
    setMode(mode);
    dataChanged();
}

void SlopeBlurNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    dataChanged();
}

void SlopeBlurNode::updateSamples(int samples) {
    setSamples(samples);
    dataChanged();
}
