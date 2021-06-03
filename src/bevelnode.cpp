#include "bevelnode.h"

BevelNode::BevelNode(QQuickItem *parent, QVector2D resolution, float distance, float smooth, bool useAlpha):
    Node(parent, resolution), m_dist(distance), m_smooth(smooth), m_alpha(useAlpha)
{
    createSockets(2, 1);
    setTitle("Bevel");
    m_socketsInput[0]->setTip("Height");
    m_socketsInput[1]->setTip("Mask");
    preview = new BevelObject(grNode, m_resolution, m_dist, m_smooth, m_alpha);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &BevelNode::updateScale);
    connect(preview, &BevelObject::textureChanged, this, &BevelNode::setOutput);
    connect(preview, &BevelObject::updatePreview, this, &BevelNode::updatePreview);
    connect(this, &BevelNode::distanceChanged, preview, &BevelObject::setDistance);
    connect(this, &BevelNode::smoothChanged, preview, &BevelObject::setSmooth);
    connect(this, &BevelNode::useAlphaChanged, preview, &BevelObject::setUseAlpha);
    connect(this, &Node::changeResolution, preview, &BevelObject::setResolution);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BevelProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startDistance", m_dist);
    propertiesPanel->setProperty("startSmooth", m_smooth);
    propertiesPanel->setProperty("startUseAlpha", m_alpha);
    connect(propertiesPanel, SIGNAL(distanceChanged(qreal)), this, SLOT(updateDistance(qreal)));
    connect(propertiesPanel, SIGNAL(bevelSmoothChanged(qreal)), this, SLOT(updateSmooth(qreal)));
    connect(propertiesPanel, SIGNAL(useAlphaChanged(bool)), this, SLOT(updateUseAlpha(bool)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

BevelNode::~BevelNode() {
    delete preview;
}

void BevelNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &BevelNode::getPreviewTexture() {
    return preview->texture();
}

void BevelNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
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
}

float BevelNode::distance() {
    return m_dist;
}

void BevelNode::setDistance(float dist) {
    m_dist = dist;
    distanceChanged(dist);
}

float BevelNode::smooth() {
    return m_smooth;
}

void BevelNode::setSmooth(float smooth) {
    m_smooth = smooth;
    smoothChanged(smooth);
}

bool BevelNode::useAlpha() {
    return m_alpha;
}

void BevelNode::setUseAlpha(bool use) {
    m_alpha = use;
    useAlphaChanged(use);
}

void BevelNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
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
