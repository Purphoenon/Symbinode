#include "roughnode.h"

RoughNode::RoughNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    createSockets(1, 0);
    setTitle("Roughness");
    m_socketsInput[0]->setTip("Roughness");
    preview = new OneChanelObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    preview->setValue(0.2f);
    connect(this, &RoughNode::changeSelected, this, &RoughNode::updatePrev);
    connect(this, &Node::changeScaleView, this, &RoughNode::updateScale);
    connect(preview, &OneChanelObject::updatePreview, this, &RoughNode::updatePreview);
    connect(preview, &OneChanelObject::updateValue, this, &RoughNode::roughChanged);
    connect(this, &Node::changeResolution, preview, &OneChanelObject::setResolution);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/RoughProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(roughChanged(qreal)), this, SLOT(updateRough(qreal)));
}

RoughNode::~RoughNode() {
    delete preview;
}

void RoughNode::operation() {
    if(m_socketsInput[0]->countEdge() > 0) {
        preview->setValue(m_socketsInput[0]->value());
        preview->useTex = true;
    }
    else {
        preview->setValue(m_rough);
        preview->useTex = false;
    }
    preview->selectedItem = selected();
    preview->update();
}

void RoughNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 10;
    json["rough"] = m_rough;
}

void RoughNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("rough")) {
        updateRough(json["rough"].toVariant().toFloat());
        propertiesPanel->setProperty("startRough", m_rough);
    }
}

void RoughNode::updateRough(qreal rough) {
    m_rough = rough;
    if(m_socketsInput[0]->countEdge() == 0) {
        operation();
    }
    dataChanged();
}

void RoughNode::updatePrev(bool sel) {
    if(sel) {
        if(m_socketsInput[0]->countEdge() > 0) {
            updatePreview(m_socketsInput[0]->value(), true);
        }
        else {
            updatePreview(QVector3D(m_rough, m_rough, m_rough), false);
        }
    }
}

void RoughNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void RoughNode::saveRough(QString dir) {
    preview->texSaving = true;
    preview->saveName = dir.append("/roughness.png");
    preview->update();
}
