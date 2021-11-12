#include "curvaturenode.h"

CurvatureNode::CurvatureNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float intensity, int offset):
    Node (parent, resolution, bpc), m_intensity(intensity), m_offset(offset)
{
    createSockets(2, 1);
    setTitle("Curvature");
    m_socketsInput[0]->setTip("Normal");
    m_socketsInput[1]->setTip("Mask");
    preview = new CurvatureObject(grNode, m_resolution, m_bpc, m_intensity, m_offset);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);
    connect(preview, &CurvatureObject::updatePreview, this, &Node::updatePreview);
    connect(this, &Node::changeBPC, preview, &CurvatureObject::setBPC);
    connect(this, &Node::changeResolution, preview, &CurvatureObject::setResolution);
    connect(preview, &CurvatureObject::textureChanged, this, &CurvatureNode::setOutput);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/CurvatureProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startOffset", m_offset);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(offsetChanged(int)), this, SLOT(updateOffset(int)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

CurvatureNode::~CurvatureNode() {
    delete preview;
}

void CurvatureNode::operation() {
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

unsigned int &CurvatureNode::getPreviewTexture() {
    return preview->texture();
}

void CurvatureNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 37;
    json["intensity"] = m_intensity;
    json["offset"] = m_offset;
}

void CurvatureNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("intensity")) {
        m_intensity = json["intensity"].toVariant().toFloat();
    }
    if(json.contains("offset")) {
        m_offset = json["offset"].toInt();
    }
    propertiesPanel->setProperty("startIntensity", m_intensity);
    propertiesPanel->setProperty("startOffset", m_offset);
    preview->setIntensity(m_intensity);
    preview->setOffset(m_offset);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

CurvatureNode *CurvatureNode::clone() {
    return new CurvatureNode(parentItem(), m_resolution, m_bpc, m_intensity, m_offset);
}

void CurvatureNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

float CurvatureNode::intensity() {
    return m_intensity;
}

void CurvatureNode::setIntensity(float intensity) {
    if(m_intensity == intensity) return;
    m_intensity = intensity;
    preview->setIntensity(m_intensity);
    preview->update();
}

int CurvatureNode::offset() {
    return m_offset;
}

void CurvatureNode::setOffset(int offset) {
    if(m_offset == offset) return;
    m_offset = offset;
    preview->setOffset(m_offset);
    preview->update();
}

void CurvatureNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void CurvatureNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    dataChanged();
}

void CurvatureNode::updateOffset(int offset) {
    setOffset(offset);
    dataChanged();
}
