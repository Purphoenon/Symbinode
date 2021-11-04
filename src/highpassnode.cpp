#include "highpassnode.h"

HighPassNode::HighPassNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float radius, float contrast):
    Node (parent, resolution, bpc), m_radius(radius), m_contrast(contrast)
{
    createSockets(2, 1);
    setTitle("High Pass");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
    preview = new HighPassObject(grNode, m_resolution, m_bpc, m_radius, m_contrast);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);
    connect(preview, &HighPassObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeBPC, preview, &HighPassObject::setBPC);
    connect(preview, &HighPassObject::textureChanged, this, &HighPassNode::setOutput);
    connect(this, &Node::changeResolution, preview, &HighPassObject::setResolution);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/HighPassProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startContrast", m_contrast);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(radiusChanged(qreal)), this, SLOT(updateRadius(qreal)));
    connect(propertiesPanel, SIGNAL(contrastChanged(qreal)), this, SLOT(updateContrast(qreal)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

HighPassNode::~HighPassNode() {
    delete preview;
}

void HighPassNode::operation() {
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

unsigned int &HighPassNode::getPreviewTexture() {
    return preview->texture();
}

void HighPassNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 36;
    json["radius"] = m_radius;
    json["contrast"] = m_contrast;
}

void HighPassNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("radius")) {
        m_radius = json["radius"].toVariant().toFloat();
    }
    if(json.contains("contrast")) {
        m_contrast = json["contrast"].toVariant().toFloat();
    }
    propertiesPanel->setProperty("startRadius", m_radius);
    propertiesPanel->setProperty("startContrast", m_contrast);
    preview->setRadius(m_radius);
    preview->setContrast(m_contrast);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

HighPassNode *HighPassNode::clone() {
    return new HighPassNode(parentItem(), m_resolution, m_bpc, m_radius, m_contrast);
}

void HighPassNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

float HighPassNode::radius() {
    return m_radius;
}

void HighPassNode::setRadius(float radius) {
    if(m_radius == radius) return;
    m_radius = radius;
    preview->setRadius(m_radius);
    preview->update();
 }

float HighPassNode::contrast() {
    return m_contrast;
}

void HighPassNode::setContrast(float contrast) {
    if(m_contrast == contrast) return;
    m_contrast = contrast;
    preview->setContrast(m_contrast);
    preview->update();
}

void HighPassNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void HighPassNode::updateRadius(qreal radius) {
    setRadius(radius);
    dataChanged();
}

void HighPassNode::updateContrast(qreal contrast) {
    setContrast(contrast);
    dataChanged();
}


