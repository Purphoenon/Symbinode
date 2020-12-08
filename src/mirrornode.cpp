#include "mirrornode.h"

MirrorNode::MirrorNode(QQuickItem *parent, QVector2D resolution, int dir): Node(parent, resolution),
    m_direction(dir)
{
    preview = new MirrorObject(grNode, m_resolution, m_direction);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &MirrorNode::updateScale);
    connect(this, &Node::changeSelected, this, &MirrorNode::updatePrev);
    connect(preview, &MirrorObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &MirrorObject::textureChanged, this, &MirrorNode::setOutput);
    connect(this, &Node::changeResolution, preview, &MirrorObject::setResolution);
    connect(this, &MirrorNode::directionChanged, preview, &MirrorObject::setDirection);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/MirrorProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startDirection", m_direction);
    connect(propertiesPanel, SIGNAL(directionChanged(int)), this, SLOT(updateDirection(int)));
    createSockets(2, 1);
    setTitle("Mirror");
    m_socketsInput[0]->setTip("Texture");
    m_socketsInput[1]->setTip("Mask");
}

MirrorNode::~MirrorNode() {
    delete preview;
}

void MirrorNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    preview->setMaskTexture(m_socketsInput[1]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

void MirrorNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 5;
    json["direction"] = m_direction;
}

void MirrorNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("direction")) {
        updateDirection(json["direction"].toInt());
        propertiesPanel->setProperty("startDirection", m_direction);
    }
}

int MirrorNode::direction() {
    return m_direction;
}

void MirrorNode::setDirection(int dir) {
    m_direction = dir;
    directionChanged(dir);
}

void MirrorNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void MirrorNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
        updatePreview(preview->texture(), true);
    }
}

void MirrorNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void MirrorNode::updateDirection(int dir) {
    setDirection(dir);
    dataChanged();
}
