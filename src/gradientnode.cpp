#include "gradientnode.h"

GradientNode::GradientNode(QQuickItem *parent, QVector2D resolution, GLint bpc, GradientParams linear,
                           GradientParams reflected, GradientParams angular, GradientParams radial,
                           QString gradientType): Node(parent, resolution, bpc),
    m_gradientType(gradientType), m_linear(linear), m_reflected(reflected), m_angular(angular),
    m_radial(radial)
{
    createSockets(1, 1);
    setTitle("Gradient");
    m_socketsInput[0]->setTip("Mask");
    preview = new GradientObject(grNode, m_resolution, m_bpc, m_gradientType, startX(), startY(), endX(), endY(), centerWidth(), tiling());
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::generatePreview, this, &GradientNode::previewGenerated);
    connect(this, &GradientNode::gradientTypeChanged, preview, &GradientObject::setGradientType);
    connect(preview, &GradientObject::changedTexture, this, &GradientNode::setOutput);
    connect(preview, &GradientObject::updatePreview, this, &GradientNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &GradientObject::setResolution);
    connect(this, &Node::changeBPC, preview, &GradientObject::setBPC);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/GradientProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    int index = 0;
    if(m_gradientType == "reflected") index = 1;
    else if (m_gradientType == "angular") index = 2;
    else if(m_gradientType == "radial") index = 3;
    propertiesPanel->setProperty("type", index);
    propertiesPanel->setProperty("startStartX", startX());
    propertiesPanel->setProperty("startStartY", startY());
    propertiesPanel->setProperty("startEndX", endX());
    propertiesPanel->setProperty("startEndY", endY());
    propertiesPanel->setProperty("startCenterWidth", centerWidth());
    propertiesPanel->setProperty("startTiling", tiling());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(gradientTypeChanged(QString)), this, SLOT(updateGradientType(QString)));
    connect(propertiesPanel, SIGNAL(startXChanged(qreal)), this, SLOT(updateStartX(qreal)));
    connect(propertiesPanel, SIGNAL(startYChanged(qreal)), this, SLOT(updateStartY(qreal)));
    connect(propertiesPanel, SIGNAL(endXChanged(qreal)), this, SLOT(updateEndX(qreal)));
    connect(propertiesPanel, SIGNAL(endYChanged(qreal)), this, SLOT(updateEndY(qreal)));
    connect(propertiesPanel, SIGNAL(centerWidthChanged(qreal)), this, SLOT(updateCenterWidth(qreal)));
    connect(propertiesPanel, SIGNAL(tilingChanged(bool)), this, SLOT(updateTiling(bool)));
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

GradientNode::~GradientNode() {
    delete preview;
}

void GradientNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
    deserializing = false;
}

GradientNode *GradientNode::clone() {
    return new GradientNode(parentItem(), m_resolution, m_bpc, m_linear, m_reflected, m_angular, m_radial,
                            m_gradientType);
}

void GradientNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 25;
    QJsonObject linearParam;
    linearParam["startX"] = m_linear.startX;
    linearParam["startY"] = m_linear.startY;
    linearParam["endX"] = m_linear.endX;
    linearParam["endY"] = m_linear.endY;
    linearParam["tiling"] = m_linear.tiling;
    json["linearParam"] = linearParam;
    QJsonObject reflectedParam;
    reflectedParam["startX"] = m_reflected.startX;
    reflectedParam["startY"] = m_reflected.startY;
    reflectedParam["endX"] = m_reflected.endX;
    reflectedParam["endY"] = m_reflected.endY;
    reflectedParam["centerWidth"] = m_reflected.centerWidth;
    reflectedParam["tiling"] = m_reflected.tiling;
    json["reflectedParam"] = reflectedParam;
    QJsonObject angularParam;
    angularParam["startX"] = m_angular.startX;
    angularParam["startY"] = m_angular.startY;
    angularParam["endX"] = m_angular.endX;
    angularParam["endY"] = m_angular.endY;
    json["angularParam"] = angularParam;
    QJsonObject radialParam;
    radialParam["startX"] = m_radial.startX;
    radialParam["startY"] = m_radial.startY;
    radialParam["endX"] = m_radial.endX;
    radialParam["endY"] = m_radial.endY;
    json["radialParam"] = radialParam;
    json["gradientType"] = m_gradientType;
}

void GradientNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("linearParam")) {
        QJsonObject linearParam = json["linearParam"].toObject();
        if(linearParam.contains("startX")) {
            m_linear.startX = linearParam["startX"].toVariant().toFloat();
        }
        if(linearParam.contains("startY")) {
            m_linear.startY = linearParam["startY"].toVariant().toFloat();
        }
        if(linearParam.contains("endX")) {
            m_linear.endX = linearParam["endX"].toVariant().toFloat();
        }
        if(linearParam.contains("endY")) {
            m_linear.endY = linearParam["endY"].toVariant().toFloat();
        }
        if(linearParam.contains("tiling")) {
            m_linear.tiling = linearParam["tiling"].toBool();
        }
    }
    if(json.contains("reflectedParam")) {
        QJsonObject reflectedParam = json["reflectedParam"].toObject();
        if(reflectedParam.contains("startX")) {
            m_reflected.startX = reflectedParam["startX"].toVariant().toFloat();
        }
        if(reflectedParam.contains("startY")) {
            m_reflected.startY = reflectedParam["startY"].toVariant().toFloat();
        }
        if(reflectedParam.contains("endX")) {
            m_reflected.endX = reflectedParam["endX"].toVariant().toFloat();
        }
        if(reflectedParam.contains("endY")) {
            m_reflected.endY = reflectedParam["endY"].toVariant().toFloat();
        }
        if(reflectedParam.contains("centerWidth")) {
            m_reflected.centerWidth = reflectedParam["centerWidth"].toVariant().toFloat();
        }
        if(reflectedParam.contains("tiling")) {
            m_reflected.tiling = reflectedParam["tiling"].toBool();
        }
    }
    if(json.contains("angularParam")) {
        QJsonObject angularParam = json["angularParam"].toObject();
        if(angularParam.contains("startX")) {
            m_angular.startX = angularParam["startX"].toVariant().toFloat();
        }
        if(angularParam.contains("startY")) {
            m_angular.startY = angularParam["startY"].toVariant().toFloat();
        }
        if(angularParam.contains("endX")) {
            m_angular.endX = angularParam["endX"].toVariant().toFloat();
        }
        if(angularParam.contains("endY")) {
            m_angular.endY = angularParam["endY"].toVariant().toFloat();
        }
    }
    if(json.contains("radialParam")) {
        QJsonObject radialParam = json["radialParam"].toObject();
        if(radialParam.contains("startX")) {
            m_radial.startX = radialParam["startX"].toVariant().toFloat();
        }
        if(radialParam.contains("startY")) {
            m_radial.startY = radialParam["startY"].toVariant().toFloat();
        }
        if(radialParam.contains("endX")) {
            m_radial.endX = radialParam["endX"].toVariant().toFloat();
        }
        if(radialParam.contains("endY")) {
            m_radial.endY = radialParam["endY"].toVariant().toFloat();
        }
    }
    if(json.contains("gradientType")) {
        m_gradientType = json["gradientType"].toString();
        gradientTypeChanged(m_gradientType);
        int index = 0;
        if(m_gradientType == "reflected") index = 1;
        else if (m_gradientType == "angular") index = 2;
        else if(m_gradientType == "radial") index = 3;
        propertiesPanel->setProperty("type", index);
        propertiesPanel->setProperty("startStartX", startX());
        propertiesPanel->setProperty("startStartY", startY());
        propertiesPanel->setProperty("startEndX", endX());
        propertiesPanel->setProperty("startEndY", endY());
        propertiesPanel->setProperty("startCenterWidth", centerWidth());
        propertiesPanel->setProperty("startTiling", tiling());

        preview->setStartX(startX());
        preview->setStartY(startY());
        preview->setEndX(endX());
        preview->setEndY(endY());
        preview->setReflectedWidth(centerWidth());
        preview->setTiling(tiling());
    }
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    preview->update();
}

unsigned int &GradientNode::getPreviewTexture() {
    return preview->texture();
}

void GradientNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

GradientParams GradientNode::linearParam() {
    return m_linear;
}

GradientParams GradientNode::reflectedParam() {
    return m_reflected;
}

GradientParams GradientNode::angularParam() {
    return m_angular;
}

GradientParams GradientNode::radialParam() {
    return m_radial;
}

QString GradientNode::gradientType() {
    return m_gradientType;
}

void GradientNode::setGradientType(QString type) {
    m_gradientType = type;
    propertiesPanel->setProperty("startStartX", startX());
    propertiesPanel->setProperty("startStartY", startY());
    propertiesPanel->setProperty("startEndX", endX());
    propertiesPanel->setProperty("startEndY", endY());
    propertiesPanel->setProperty("startCenterWidth", centerWidth());
    propertiesPanel->setProperty("startTiling", tiling());
    gradientTypeChanged(type);

    preview->setStartX(startX());
    preview->setStartY(startY());
    preview->setEndX(endX());
    preview->setEndY(endY());
    preview->setReflectedWidth(centerWidth());
    preview->setTiling(tiling());
    preview->update();
}

float GradientNode::startX() {
    if(m_gradientType == "linear") return m_linear.startX;
    else if(m_gradientType == "reflected") return m_reflected.startX;
    else if(m_gradientType == "angular") return m_angular.startX;
    else if(m_gradientType == "radial") return m_radial.startX;
    return m_linear.startX;
}

void GradientNode::setStartX(float x) {
    if(m_gradientType == "linear") {
        if(m_linear.startX == x) return;
        m_linear.startX = x;
    }
    else if(m_gradientType == "reflected") {
        if(m_reflected.startX == x) return;
        m_reflected.startX = x;
    }
    else if(m_gradientType == "angular") {
        if(m_angular.startX == x) return;
        m_angular.startX = x;
    }
    else if(m_gradientType == "radial") {
        if(m_radial.startX == x) return;
        m_radial.startX = x;
    }
    startXChanged(x);
    preview->setStartX(x);
    preview->update();
}

float GradientNode::startY() {
    if(m_gradientType == "linear") return m_linear.startY;
    else if(m_gradientType == "reflected") return m_reflected.startY;
    else if(m_gradientType == "angular") return m_angular.startY;
    else if(m_gradientType == "radial") return m_radial.startY;
    return m_linear.startY;
}

void GradientNode::setStartY(float y) {
    if(m_gradientType == "linear") {
        if(m_linear.startY == y) return;
        m_linear.startY = y;
    }
    else if(m_gradientType == "reflected") {
        if(m_reflected.startY == y) return;
        m_reflected.startY = y;
    }
    else if(m_gradientType == "angular") {
        if(m_angular.startY == y) return;
        m_angular.startY = y;
    }
    else if(m_gradientType == "radial") {
        if(m_radial.startY == y) return;
        m_radial.startY = y;
    }
    startYChanged(y);
    preview->setStartY(y);
    preview->update();
}

float GradientNode::endX() {
    if(m_gradientType == "linear") return m_linear.endX;
    else if(m_gradientType == "reflected") return m_reflected.endX;
    else if(m_gradientType == "angular") return m_angular.endX;
    else if(m_gradientType == "radial") return m_radial.endX;
    return m_linear.endX;
}

void GradientNode::setEndX(float x) {
    if(m_gradientType == "linear") {
        if(m_linear.endX == x) return;
        m_linear.endX = x;
    }
    else if(m_gradientType == "reflected") {
        if(m_reflected.endX == x) return;
        m_reflected.endX = x;
    }
    else if(m_gradientType == "angular") {
        if(m_angular.endX == x) return;
        m_angular.endX = x;
    }
    else if(m_gradientType == "radial") {
        if(m_radial.endX == x) return;
        m_radial.endX = x;
    }
    endXChanged(x);
    preview->setEndX(x);
    preview->update();
}

float GradientNode::endY() {
    if(m_gradientType == "linear") return m_linear.endY;
    else if(m_gradientType == "reflected") return m_reflected.endY;
    else if(m_gradientType == "angular") return m_angular.endY;
    else if(m_gradientType == "radial") return m_radial.endY;
    return m_linear.endY;
}

void GradientNode::setEndY(float y) {
    if(m_gradientType == "linear") {
        if(m_linear.endY == y) return;
        m_linear.endY = y;
    }
    else if(m_gradientType == "reflected") {
        if(m_reflected.endY == y) return;
        m_reflected.endY = y;
    }
    else if(m_gradientType == "angular") {
        if(m_angular.endY == y) return;
        m_angular.endY = y;
    }
    else if(m_gradientType == "radial") {
        if(m_radial.endY == y) return;
        m_radial.endY = y;
    }
    endYChanged(y);
    preview->setEndY(y);
    preview->update();
}

float GradientNode::centerWidth() {
    if(m_gradientType == "reflected") return m_reflected.centerWidth;
    else return 0.0f;
}

void GradientNode::setCenterWidth(float width) {
    if(m_gradientType == "reflected") {
        if(m_reflected.centerWidth == width) return;
        m_reflected.centerWidth = width;
        centerWidthChanged(width);
        preview->setReflectedWidth(width);
        preview->update();
    }
}

bool GradientNode::tiling() {
    if(m_gradientType == "linear") return m_linear.tiling;
    else if(m_gradientType == "reflected") return m_reflected.tiling;
    else return false;
}

void GradientNode::setTiling(bool tiling) {
    if(m_gradientType == "linear") {
        if(m_linear.tiling == tiling) return;
        m_linear.tiling = tiling;
    }
    else if(m_gradientType == "reflected") {
        if(m_reflected.tiling == tiling) return;
        m_reflected.tiling = tiling;        
    }
    tilingChanged(tiling);
    preview->setTiling(tiling);
    preview->update();
}

void GradientNode::updateGradientType(QString type) {
    setGradientType(type);
    dataChanged();
}

void GradientNode::updateStartX(qreal x) {
    setStartX(x);
    dataChanged();
}

void GradientNode::updateStartY(qreal y) {
    setStartY(y);
    dataChanged();
}

void GradientNode::updateEndX(qreal x) {
    setEndX(x);
    dataChanged();
}

void GradientNode::updateEndY(qreal y) {
    setEndY(y);
    dataChanged();
}

void GradientNode::updateCenterWidth(qreal width) {
    setCenterWidth(width);
    dataChanged();
}

void GradientNode::updateTiling(bool tiling) {
    setTiling(tiling);
    dataChanged();
}

void GradientNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void GradientNode::previewGenerated() {
    preview->generatedGradient = true;
    preview->update();
}
