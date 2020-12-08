#include "normalmapnode.h"

NormalMapNode::NormalMapNode(QQuickItem *parent, QVector2D resolution, float strenght): Node(parent, resolution), m_strenght(strenght)
{
    createSockets(1, 1);
    setTitle("Normal Map");
    m_socketsInput[0]->setTip("Height");
    preview = new NormalMapObject(grNode, m_resolution, m_strenght);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    propView = new QQuickView();
    connect(this, &NormalMapNode::changeSelected, this, &NormalMapNode::updatePrev);
    connect(preview, &NormalMapObject::textureChanged, this, &NormalMapNode::setOutput);
    connect(this, &NormalMapNode::generatePreview, this, &NormalMapNode::previewGenerated);
    connect(this, &NormalMapNode::strenghtChanged, preview, &NormalMapObject::setStrenght);
    connect(this, &Node::changeScaleView, this, &NormalMapNode::updateScale);
    connect(preview, &NormalMapObject::updatePreview, this, &NormalMapNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &NormalMapObject::setResolution);
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/NormalMapProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(strenghtChanged(qreal)), this, SLOT(updateStrenght(qreal)));
    propertiesPanel->setProperty("startStrenght", m_strenght/30.0f);
}

NormalMapNode::~NormalMapNode() {
    delete preview;
}

void NormalMapNode::operation() {
    preview->setGrayscaleTexture(m_socketsInput[0]->value().toUInt());
    preview->selectedItem = selected();
    preview->update();
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

float NormalMapNode::strenght() {
    return m_strenght;
}

void NormalMapNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 11;
    json["strength"] = m_strenght;
}

void NormalMapNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("strength")) {
        updateStrenght(json["strength"].toVariant().toFloat());
        propertiesPanel->setProperty("startStrenght", m_strenght/30.0f);
    }
}

void NormalMapNode::updateStrenght(qreal strenght) {
    m_strenght = static_cast<float>(strenght);
    strenghtChanged(m_strenght);
    operation();
    dataChanged();
}

void NormalMapNode::setOutput() {
    m_socketOutput[0]->setValue(preview->normalTexture());
}

void NormalMapNode::previewGenerated() {
    preview->normalGenerated = true;
    preview->update();
}

void NormalMapNode::updatePrev(bool sel) {
    if(sel) {
        updatePreview(preview->normalTexture(), true);
    }
    preview->selectedItem = sel;
}

void NormalMapNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}
