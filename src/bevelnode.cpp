#include "bevelnode.h"

BevelNode::BevelNode(QQuickItem *parent, QVector2D resolution, GLint bpc, float distance, float smooth,
                     bool useAlpha): Node(parent, resolution, bpc), m_dist(distance), m_smooth(smooth),
    m_alpha(useAlpha)
{
    createSockets(2, 1);
    setTitle("Bevel");
    m_socketsInput[0]->setTip("Height");
    m_socketsInput[1]->setTip("Mask");
    preview = new BevelObject(grNode, m_resolution, m_bpc, m_dist, m_smooth, m_alpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &BevelObject::textureChanged, this, &BevelNode::setOutput);
    connect(preview, &BevelObject::updatePreview, this, &BevelNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &BevelObject::setResolution);
    connect(this, &Node::changeBPC, preview, &BevelObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BevelProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startDistance", m_dist);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_alpha);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(distanceChanged(qreal)), this, SLOT(updateDistance(qreal)));
    connect(propertiesPanel, SIGNAL(bevelSmoothChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

BevelNode::~BevelNode() {
    delete preview;
}

void BevelNode::operation() {
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
    preview->update();
    deserializing = false;
}

unsigned int &BevelNode::getPreviewTexture() {
    return preview->texture();
}

void BevelNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

BevelNode *BevelNode::clone() {
    return new BevelNode(parentItem(), m_resolution, m_bpc, m_dist, m_smooth, m_alpha);
}

void BevelNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 29;
    json["distance"] = m_dist;
    json["smooth"] = m_smooth;
    json["useAlpha"] = m_alpha;
}

void BevelNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("distance")) {
        m_dist = json["distance"].toVariant().toFloat();
    }
    if(json.contains("smooth")) {
        m_smooth = json["smooth"].toVariant().toFloat();
    }
    if(json.contains("useAlpha")) {
        m_alpha = json["useAlpha"].toBool();
    }
    propertiesPanel->setProperty("startDistance", m_dist);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_alpha);

    preview->setDistance(m_dist);
    preview->setSmooth(m_smooth);
    preview->setUseAlpha(m_alpha);

    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);

    preview->update();
}

float BevelNode::distance() {
    return m_dist;
}

void BevelNode::setDistance(float dist) {
    if(m_dist == dist) return;
    m_dist = dist;
    distanceChanged(dist);
    preview->setDistance(dist);
    preview->update();
}

float BevelNode::smooth() {
    return m_smooth;
}

void BevelNode::setSmooth(float smooth) {
    if(m_smooth == smooth) return;
    m_smooth = smooth;
    smoothChanged(smooth);
    preview->setSmooth(smooth);
    preview->update();
}

bool BevelNode::useAlpha() {
    return m_alpha;
}

void BevelNode::setUseAlpha(bool use) {
    if(m_alpha == use) return;
    m_alpha = use;
    useAlphaChanged(use);
    preview->setUseAlpha(use);
    preview->update();
}

void BevelNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void BevelNode::updateDistance(qreal dist) {
    setDistance(dist);
    dataChanged();
}

void BevelNode::updateSmooth(qreal smooth) {
    setSmooth(smooth);
    dataChanged();
}

void BevelNode::updateUseAlpha(bool use) {
    setUseAlpha(use);
    dataChanged();
}
