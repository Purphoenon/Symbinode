#include "albedonode.h"
#include <iostream>

AlbedoNode::AlbedoNode(QQuickItem *parent, QVector2D resolution): Node(parent, resolution)
{
    preview = new AlbedoObject(grNode, m_resolution);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(preview, &AlbedoObject::updatePreview, this, &AlbedoNode::updatePreview);
    connect(this, &AlbedoNode::changeSelected, this, &AlbedoNode::updatePrev);
    connect(this, &Node::changeScaleView, this, &AlbedoNode::updateScale);
    connect(preview, &AlbedoObject::updateAlbedo, this, &AlbedoNode::albedoChanged);
    connect(this, &Node::changeResolution, preview, &AlbedoObject::setResolution);
    createSockets(1, 0);
    setTitle("Albedo");
    m_socketsInput[0]->setTip("Albedo");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/AlbedoProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    connect(propertiesPanel, SIGNAL(albedoChanged(QVector3D)), this, SLOT(updateAlbedo(QVector3D)));
}

AlbedoNode::~AlbedoNode() {
    delete preview;
}

void AlbedoNode::operation() {
    if(m_socketsInput[0]->countEdge() > 0) {
        preview->useAlbedoTex = true;
        preview->setAlbedo(m_socketsInput[0]->value());
    }
    else {
        preview->useAlbedoTex = false;
        preview->setAlbedo(m_albedo);
    }
    preview->selectedItem = selected();
    preview->update();
}

void AlbedoNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 8;
    QJsonArray albedo;
    albedo.append(m_albedo.x());
    albedo.append(m_albedo.y());
    albedo.append(m_albedo.z());
    json["albedo"] = albedo;
}

void AlbedoNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("albedo") && json["albedo"].isArray()) {
        QJsonArray albedo = json["albedo"].toArray();
        QVector3D color = QVector3D(albedo[0].toVariant().toFloat(), albedo[1].toVariant().toFloat(),
                             albedo[2].toVariant().toFloat());
        updateAlbedo(color);
        propertiesPanel->setProperty("startColor", color);
    }
}

void AlbedoNode::updateAlbedo(QVector3D color) {
    m_albedo = color;
    if(m_socketsInput[0]->countEdge() == 0) {
        operation();
    }
    dataChanged();
}

void AlbedoNode::updatePrev(bool sel) {
    if(sel) {
        if(m_socketsInput[0]->countEdge() > 0) {
            updatePreview(m_socketsInput[0]->value(), true);
        }
        else {
            updatePreview(m_albedo, false);
        }
    }
}

void AlbedoNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void AlbedoNode::saveAlbedo(QString dir) {
    qDebug("save albedo");
    preview->texSaving = true;
    preview->saveName = dir;
    preview->update();
}
