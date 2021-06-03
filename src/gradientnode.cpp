#include "gradientnode.h"

GradientNode::GradientNode(QQuickItem *parent, QVector2D resolution, GradientParams linear,
                           GradientParams reflected, GradientParams angular, GradientParams radial,
                           QString gradientType): Node(parent, resolution), m_gradientType(gradientType),
    m_linear(linear), m_reflected(reflected), m_angular(angular), m_radial(radial)
{
    createSockets(1, 1);
    setTitle("Gradient");
    m_socketsInput[0]->setTip("Mask");
    preview = new GradientObject(grNode, m_resolution, m_gradientType, startX(), startY(), endX(), endY(), centerWidth(), tiling());
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &GradientNode::updateScale);
    connect(this, &Node::generatePreview, this, &GradientNode::previewGenerated);
    connect(this, &GradientNode::gradientTypeChanged, preview, &GradientObject::setGradientType);
    connect(this, &GradientNode::startXChanged, preview, &GradientObject::setStartX);
    connect(this, &GradientNode::startYChanged, preview, &GradientObject::setStartY);
    connect(this, &GradientNode::endXChanged, preview, &GradientObject::setEndX);
    connect(this, &GradientNode::endYChanged, preview, &GradientObject::setEndY);
    connect(this, &GradientNode::centerWidthChanged, preview, &GradientObject::setReflectedWidth);
    connect(this, &GradientNode::tilingChanged, preview, &GradientObject::setTiling);
    connect(preview, &GradientObject::changedTexture, this, &GradientNode::setOutput);
    connect(preview, &GradientObject::updatePreview, this, &GradientNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &GradientObject::setResolution);
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
    connect(propertiesPanel, SIGNAL(gradientTypeChanged(QString)), this, SLOT(updateGradientType(QString)));
    connect(propertiesPanel, SIGNAL(startXChanged(qreal)), this, SLOT(updateStartX(qreal)));
    connect(propertiesPanel, SIGNAL(startYChanged(qreal)), this, SLOT(updateStartY(qreal)));
    connect(propertiesPanel, SIGNAL(endXChanged(qreal)), this, SLOT(updateEndX(qreal)));
    connect(propertiesPanel, SIGNAL(endYChanged(qreal)), this, SLOT(updateEndY(qreal)));
    connect(propertiesPanel, SIGNAL(centerWidthChanged(qreal)), this, SLOT(updateCenterWidth(qreal)));
    connect(propertiesPanel, SIGNAL(tilingChanged(bool)), this, SLOT(updateTiling(bool)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

GradientNode::~GradientNode() {
    delete preview;
}

void GradientNode::operation() {
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
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
    }
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
}

float GradientNode::startX() {
    if(m_gradientType == "linear") return m_linear.startX;
    else if(m_gradientType == "reflected") return m_reflected.startX;
    else if(m_gradientType == "angular") return m_angular.startX;
    else if(m_gradientType == "radial") return m_radial.startX;
    return m_linear.startX;
}

void GradientNode::setStartX(float x) {
    if(m_gradientType == "linear") m_linear.startX = x;
    else if(m_gradientType == "reflected") m_reflected.startX = x;
    else if(m_gradientType == "angular") m_angular.startX = x;
    else if(m_gradientType == "radial") m_radial.startX = x;
    startXChanged(x);
}

float GradientNode::startY() {
    if(m_gradientType == "linear") return m_linear.startY;
    else if(m_gradientType == "reflected") return m_reflected.startY;
    else if(m_gradientType == "angular") return m_angular.startY;
    else if(m_gradientType == "radial") return m_radial.startY;
    return m_linear.startY;
}

void GradientNode::setStartY(float y) {
    if(m_gradientType == "linear") m_linear.startY = y;
    else if(m_gradientType == "reflected") m_reflected.startY = y;
    else if(m_gradientType == "angular") m_angular.startY = y;
    else if(m_gradientType == "radial") m_radial.startY = y;
    startYChanged(y);
}

float GradientNode::endX() {
    if(m_gradientType == "linear") return m_linear.endX;
    else if(m_gradientType == "reflected") return m_reflected.endX;
    else if(m_gradientType == "angular") return m_angular.endX;
    else if(m_gradientType == "radial") return m_radial.endX;
    return m_linear.endX;
}

void GradientNode::setEndX(float x) {
    if(m_gradientType == "linear") m_linear.endX = x;
    else if(m_gradientType == "reflected") m_reflected.endX = x;
    else if(m_gradientType == "angular") m_angular.endX = x;
    else if(m_gradientType == "radial") m_radial.endX = x;
    endXChanged(x);
}

float GradientNode::endY() {
    if(m_gradientType == "linear") return m_linear.endY;
    else if(m_gradientType == "reflected") return m_reflected.endY;
    else if(m_gradientType == "angular") return m_angular.endY;
    else if(m_gradientType == "radial") return m_radial.endY;
    return m_linear.endY;
}

void GradientNode::setEndY(float y) {
    if(m_gradientType == "linear") m_linear.endY = y;
    else if(m_gradientType == "reflected") m_reflected.endY = y;
    else if(m_gradientType == "angular") m_angular.endY = y;
    else if(m_gradientType == "radial") m_radial.endY = y;
    endYChanged(y);
}

float GradientNode::centerWidth() {
    if(m_gradientType == "reflected") return m_reflected.centerWidth;
    else return 0.0f;
}

void GradientNode::setCenterWidth(float width) {
    if(m_gradientType == "reflected") {
        m_reflected.centerWidth = width;
        centerWidthChanged(width);
    }
}

bool GradientNode::tiling() {
    if(m_gradientType == "linear") return m_linear.tiling;
    else if(m_gradientType == "reflected") return m_reflected.tiling;
    else return false;
}

void GradientNode::setTiling(bool tiling) {
    if(m_gradientType == "linear") {
        m_linear.tiling = tiling;
        tilingChanged(tiling);
    }
    else if(m_gradientType == "reflected") {
        m_reflected.tiling = tiling;
        tilingChanged(tiling);
    }
}

void GradientNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
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
