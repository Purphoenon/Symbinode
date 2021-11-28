#include "ambientocclusionnode.h"

AmbientOcclusionNode::AmbientOcclusionNode(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                           float radius, int samples, float strength, float smooth):
    Node(parent, resolution, bpc), m_radius(radius), m_samples(samples), m_strength(strength),
    m_smooth(smooth)
{
    createSockets(2, 1);
    setTitle("Ambient Occlusion");
    m_socketsInput[0]->setTip("Height");
    m_socketsInput[1]->setTip("Mask");
    preview = new AmbientOcclusionObject(grNode, m_resolution, m_bpc, m_radius, m_samples, m_strength,
                                         m_smooth);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);
    connect(preview, &AmbientOcclusionObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeBPC, preview, &AmbientOcclusionObject::setBPC);
    connect(this, &Node::changeResolution, preview, &AmbientOcclusionObject::setResolution);
    connect(preview, &AmbientOcclusionObject::textureChanged, this, &AmbientOcclusionNode::setOutput);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/AmbientOcclusionProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSamples", m_samples);
    propertiesPanel->setProperty("startStrength", m_strength);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(radiusChanged(qreal)), this, SLOT(updateRadius(qreal)));
    connect(propertiesPanel, SIGNAL(samplesChanged(int)), this, SLOT(updateSamples(int)));
    connect(propertiesPanel, SIGNAL(strengthChanged(qreal)), this, SLOT(updateStrength(qreal)));
    connect(propertiesPanel, SIGNAL(ambientSmoothChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

AmbientOcclusionNode::~AmbientOcclusionNode() {
    delete preview;
}

void AmbientOcclusionNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    if(!m_socketsInput[1]->getEdges().isEmpty()) {
        Node *inputNode1 = static_cast<Node*>(m_socketsInput[1]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode1 && inputNode1->resolution() != m_resolution) return;
        if(m_socketsInput[1]->value() == 0 && deserializing) return;
    }
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    preview->update();
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
    if(deserializing) deserializing = false;
}

unsigned int &AmbientOcclusionNode::getPreviewTexture() {
    return preview->texture();
}

void AmbientOcclusionNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 39;
    json["radius"] = m_radius;
    json["samples"] = m_samples;
    json["strength"] = m_strength;
    json["smooth"] = m_smooth;
}

void AmbientOcclusionNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("radius")) {
        m_radius = json["radius"].toVariant().toFloat();
    }
    if(json.contains("samples")) {
        m_samples = json["samples"].toInt();
    }
    if(json.contains("strength")) {
        m_strength = json["strength"].toVariant().toFloat();
    }
    if(json.contains("smooth")) {
        m_smooth = json["smooth"].toVariant().toFloat();
    }
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startSamples", m_samples);
    propertiesPanel->setProperty("startStrength", m_strength);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    preview->setRadius(m_radius);
    preview->setSamples(m_samples);
    preview->setStrength(m_strength);
    preview->setSmooth(m_smooth);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

AmbientOcclusionNode *AmbientOcclusionNode::clone() {
    return new AmbientOcclusionNode(parentItem(), m_resolution, m_bpc, m_radius, m_samples, m_strength,
                                    m_smooth);
}

void AmbientOcclusionNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

float AmbientOcclusionNode::radius() {
    return m_radius;
}

void AmbientOcclusionNode::setRadius(float radius) {
    if(m_radius == radius) return;
    m_radius = radius;
    preview->setRadius(m_radius);
    preview->update();
}

int AmbientOcclusionNode::samples() {
    return m_samples;
}

void AmbientOcclusionNode::setSamples(int samples) {
    if(m_samples == samples) return;
    m_samples = samples;
    preview->setSamples(m_samples);
    preview->update();
}

float AmbientOcclusionNode::strength() {
    return m_strength;
}

void AmbientOcclusionNode::setStrength(float strength) {
    if(m_strength == strength) return;
    m_strength = strength;
    preview->setStrength(m_strength);
    preview->update();
}

float AmbientOcclusionNode::ambientSmooth() {
    return m_smooth;
}

void AmbientOcclusionNode::setSmooth(float smooth) {
    if(m_smooth == smooth) return;
    m_smooth = smooth;
    preview->setSmooth(m_smooth);
    preview->update();
}

void AmbientOcclusionNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void AmbientOcclusionNode::updateRadius(qreal radius) {
    setRadius(radius);
    dataChanged();
}

void AmbientOcclusionNode::updateSamples(int samples) {
    setSamples(samples);
    dataChanged();
}

void AmbientOcclusionNode::updateStrength(qreal strength) {
    setStrength(strength);
    dataChanged();
}

void AmbientOcclusionNode::updateSmooth(qreal smooth) {
    setSmooth(smooth);
    dataChanged();
}
