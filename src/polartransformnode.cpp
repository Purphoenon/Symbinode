#include "polartransformnode.h"

PolarTransformNode::PolarTransformNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float radius,
                                       bool clamp, int angle): Node (parent, resolution, bpc),
    m_radius(radius), m_clamp(clamp), m_angle(angle)
{
    createSockets(2, 1);
    setTitle("Polar Transform");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    preview = new PolarTransformObject(grNode, m_resolution, m_bpc, m_radius, m_clamp, m_angle);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &PolarTransformNode::updateScale);
    connect(preview, &PolarTransformObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &PolarTransformObject::textureChanged, this, &PolarTransformNode::setOutput);
    connect(this, &PolarTransformNode::radiusChanged, preview, &PolarTransformObject::setRadius);
    connect(this, &PolarTransformNode::clampChanged, preview, &PolarTransformObject::setClamp);
    connect(this, &PolarTransformNode::angleChanged, preview, &PolarTransformObject::setAngle);
    connect(this, &Node::changeResolution, preview, &PolarTransformObject::setResolution);
    connect(this, &Node::changeBPC, preview, &PolarTransformObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/PolarTransformProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startClamp", m_clamp);
    propertiesPanel->setProperty("startRotation", m_angle);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(radiusChanged(qreal)), this, SLOT(updateRadius(qreal)));
    connect(propertiesPanel, SIGNAL(clampChanged(bool)), this, SLOT(updateClamp(bool)));
    connect(propertiesPanel, SIGNAL(angleChanged(int)), this, SLOT(updateAngle(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

PolarTransformNode::~PolarTransformNode() {
    delete preview;
}

void PolarTransformNode::operation() {
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
    preview->polaredTex = true;
    preview->update();
}

unsigned int &PolarTransformNode::getPreviewTexture() {
    return preview->texture();
}

void PolarTransformNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

PolarTransformNode *PolarTransformNode::clone() {
    return new PolarTransformNode(parentItem(), m_resolution, m_bpc, m_radius, m_clamp, m_angle);
}

void PolarTransformNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 30;
    json["radius"] = m_radius;
    json["clamp"] = m_clamp;
    json["angle"] = m_angle;
}

void PolarTransformNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("radius")) {
        m_radius = json["radius"].toVariant().toFloat();
    }
    if(json.contains("clamp")) {
        m_clamp = json["clamp"].toBool();
    }
    if(json.contains("angle")) {
        m_angle = json["angle"].toInt();
    }
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startClamp", m_clamp);
    propertiesPanel->setProperty("startRotation", m_angle);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

float PolarTransformNode::radius() {
    return m_radius;
}

void PolarTransformNode::setRadius(float radius) {
    m_radius = radius;
    radiusChanged(radius);
}

bool PolarTransformNode::clamp() {
    return m_clamp;
}

void PolarTransformNode::setClamp(bool clamp) {
    m_clamp = clamp;
    clampChanged(clamp);
}

int PolarTransformNode::angle() {
    return m_angle;
}

void PolarTransformNode::setAngle(int angle) {
    m_angle = angle;
    angleChanged(angle);
}

void PolarTransformNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void PolarTransformNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void PolarTransformNode::updateRadius(qreal radius) {
    setRadius(radius);
    dataChanged();
}

void PolarTransformNode::updateClamp(bool clamp) {
    setClamp(clamp);
    dataChanged();
}

void PolarTransformNode::updateAngle(int angle) {
    setAngle(angle);
    dataChanged();
}
