#include "voronoinode.h"

VoronoiNode::VoronoiNode(QQuickItem *parent, QVector2D resolution, VoronoiParams crystals, VoronoiParams borders, VoronoiParams solid, VoronoiParams worley, QString voronoiType): Node(parent, resolution),
      m_voronoiType(voronoiType), m_crystals(crystals), m_borders(borders), m_solid(solid), m_worley(worley)
{
    preview = new VoronoiObject(grNode, m_resolution, m_voronoiType, voronoiScale(), scaleX(), scaleY(), jitter(), inverse(), intensity(), bordersSize());
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &VoronoiNode::updateScale);
    connect(this, &VoronoiNode::changeSelected, this, &VoronoiNode::updatePrev);
    connect(this, &VoronoiNode::generatePreview, this, &VoronoiNode::previewGenerated);
    connect(preview, &VoronoiObject::changedTexture, this, &VoronoiNode::setOutput);
    connect(preview, &VoronoiObject::updatePreview, this, &VoronoiNode::updatePreview);
    connect(this, &Node::changeResolution, preview, &VoronoiObject::setResolution);
    connect(this, &VoronoiNode::voronoiTypeChanged, preview, &VoronoiObject::setVoronoiType);
    connect(this, &VoronoiNode::voronoiScaleChanged, preview, &VoronoiObject::setVoronoiScale);
    connect(this, &VoronoiNode::scaleXChanged, preview, &VoronoiObject::setScaleX);
    connect(this, &VoronoiNode::scaleYChanged, preview, &VoronoiObject::setScaleY);
    connect(this, &VoronoiNode::jitterChanged, preview, &VoronoiObject::setJitter);
    connect(this, &VoronoiNode::inverseChanged, preview, &VoronoiObject::setInverse);
    connect(this, &VoronoiNode::intensityChanged, preview, &VoronoiObject::setIntensity);
    connect(this, &VoronoiNode::bordersSizeChanged, preview, &VoronoiObject::setBordersSize);
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Voronoi");
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/VoronoiProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    int index = 0;
    if(m_voronoiType == "borders") index = 1;
    else if(m_voronoiType == "solid") index = 2;
    else if(m_voronoiType == "worley") index = 3;
    propertiesPanel->setProperty("type", index);
    propertiesPanel->setProperty("startScale", voronoiScale());
    propertiesPanel->setProperty("startScaleX", scaleX());
    propertiesPanel->setProperty("startScaleY", scaleY());
    propertiesPanel->setProperty("startJitter", jitter());
    propertiesPanel->setProperty("startIntensity", intensity());
    propertiesPanel->setProperty("startInverse", inverse());
    propertiesPanel->setProperty("startBorders", bordersSize());
    connect(propertiesPanel, SIGNAL(voronoiTypeChanged(QString)), this, SLOT(updateVoronoiType(QString)));
    connect(propertiesPanel, SIGNAL(voronoiScaleChanged(int)), this, SLOT(updateVoronoiScale(int)));
    connect(propertiesPanel, SIGNAL(scaleXChanged(int)), this, SLOT(updateScaleX(int)));
    connect(propertiesPanel, SIGNAL(scaleYChanged(int)), this, SLOT(updateScaleY(int)));
    connect(propertiesPanel, SIGNAL(jitterChanged(qreal)), this, SLOT(updateJitter(qreal)));
    connect(propertiesPanel, SIGNAL(inverseChanged(bool)), this, SLOT(updateInverse(bool)));
    connect(propertiesPanel, SIGNAL(intensityChanged(qreal)), this, SLOT(updateIntensity(qreal)));
    connect(propertiesPanel, SIGNAL(bordersChanged(qreal)), this, SLOT(updateBordersSize(qreal)));
}

VoronoiNode::~VoronoiNode() {
    delete preview;
}

void VoronoiNode::operation() {
    preview->selectedItem = selected();
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
}

void VoronoiNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 13;
    QJsonObject crystalsParam;
    crystalsParam["scale"] = m_crystals.scale;
    crystalsParam["scaleX"] = m_crystals.scaleX;
    crystalsParam["scaleY"] = m_crystals.scaleY;
    crystalsParam["jitter"] = m_crystals.jitter;
    crystalsParam["inverse"] = m_crystals.inverse;
    crystalsParam["intensity"] = m_crystals.intensity;
    json["crystalsParam"] = crystalsParam;
    QJsonObject bordersParam;
    bordersParam["scale"] = m_borders.scale;
    bordersParam["scaleX"] = m_borders.scaleX;
    bordersParam["scaleY"] = m_borders.scaleY;
    bordersParam["jitter"] = m_borders.jitter;
    bordersParam["inverse"] = m_borders.inverse;
    bordersParam["intensity"] = m_borders.intensity;
    bordersParam["width"] = m_borders.borders;
    json["bordersParam"] = bordersParam;
    QJsonObject solidParam;
    solidParam["scale"] = m_solid.scale;
    solidParam["scaleX"] = m_solid.scaleX;
    solidParam["scaleY"] = m_solid.scaleY;
    solidParam["jitter"] = m_solid.jitter;
    solidParam["inverse"] = m_solid.inverse;
    solidParam["intensity"] = m_solid.intensity;
    json["solidParam"] = solidParam;
    QJsonObject worleyParam;
    worleyParam["scale"] = m_worley.scale;
    worleyParam["scaleX"] = m_worley.scaleX;
    worleyParam["scaleY"] = m_worley.scaleY;
    worleyParam["jitter"] = m_worley.jitter;
    worleyParam["inverse"] = m_worley.inverse;
    worleyParam["intensity"] = m_worley.intensity;
    json["worleyParam"] = worleyParam;
    json["voronoiType"] = m_voronoiType;
}

void VoronoiNode::deserialize(const QJsonObject &json) {
    Node::deserialize(json);
    if(json.contains("crystalsParam")) {
        QJsonObject crystalsParam = json["crystalsParam"].toObject();
        if(crystalsParam.contains("scale")) {
            m_crystals.scale = crystalsParam["scale"].toVariant().toInt();
        }
        if(crystalsParam.contains("scaleX")) {
            m_crystals.scaleX = crystalsParam["scaleX"].toInt();
        }
        if(crystalsParam.contains("scaleY")) {
            m_crystals.scaleY = crystalsParam["scaleY"].toInt();
        }
        if(crystalsParam.contains("jitter")) {
            m_crystals.jitter = crystalsParam["jitter"].toVariant().toFloat();
        }
        if(crystalsParam.contains("inverse")) {
            m_crystals.inverse = crystalsParam["inverse"].toVariant().toBool();
        }
        if(crystalsParam.contains("intensity")) {
            m_crystals.intensity = crystalsParam["intensity"].toVariant().toFloat();
        }
    }
    if(json.contains("bordersParam")) {
        QJsonObject bordersParam = json["bordersParam"].toObject();
        if(bordersParam.contains("scale")) {
            m_borders.scale = bordersParam["scale"].toVariant().toInt();
        }
        if(bordersParam.contains("scaleX")) {
            m_borders.scaleX = bordersParam["scaleX"].toInt();
        }
        if(bordersParam.contains("scaleY")) {
            m_borders.scaleY = bordersParam["scaleY"].toInt();
        }
        if(bordersParam.contains("jitter")) {
            m_borders.jitter = bordersParam["jitter"].toVariant().toFloat();
        }
        if(bordersParam.contains("inverse")) {
            m_borders.inverse = bordersParam["inverse"].toVariant().toBool();
        }
        if(bordersParam.contains("intensity")) {
            m_borders.intensity = bordersParam["intensity"].toVariant().toFloat();
        }
        if(bordersParam.contains("width")) {
            m_borders.borders = bordersParam["width"].toVariant().toFloat();
        }
    }
    if(json.contains("solidParam")) {
        QJsonObject solidParam = json["solidParam"].toObject();
        if(solidParam.contains("scale")) {
            m_solid.scale = solidParam["scale"].toVariant().toInt();
        }
        if(solidParam.contains("scaleX")) {
            m_solid.scaleX = solidParam["scaleX"].toInt();
        }
        if(solidParam.contains("scaleY")) {
            m_solid.scaleY = solidParam["scaleY"].toInt();
        }
        if(solidParam.contains("jitter")) {
            m_solid.jitter = solidParam["jitter"].toVariant().toFloat();
        }
        if(solidParam.contains("inverse")) {
            m_solid.inverse = solidParam["inverse"].toVariant().toBool();
        }
        if(solidParam.contains("intensity")) {
            m_solid.intensity = solidParam["intensity"].toVariant().toFloat();
        }
    }
    if(json.contains("worleyParam")) {
        QJsonObject worleyParam = json["worleyParam"].toObject();
        if(worleyParam.contains("scale")) {
            m_worley.scale = worleyParam["scale"].toVariant().toInt();
        }
        if(worleyParam.contains("scaleX")) {
            m_worley.scaleX = worleyParam["scaleX"].toInt();
        }
        if(worleyParam.contains("scaleY")) {
            m_worley.scaleY = worleyParam["scaleY"].toInt();
        }
        if(worleyParam.contains("jitter")) {
            m_worley.jitter = worleyParam["jitter"].toVariant().toFloat();
        }
        if(worleyParam.contains("inverse")) {
            m_worley.inverse = worleyParam["inverse"].toVariant().toBool();
        }
        if(worleyParam.contains("intensity")) {
            m_worley.intensity = worleyParam["intensity"].toVariant().toFloat();
        }
    }
    if(json.contains("voronoiType")) {
        m_voronoiType = json["voronoiType"].toVariant().toString();
        voronoiTypeChanged(m_voronoiType);
        int index = 0;
        if(m_voronoiType == "borders") index = 1;
        else if(m_voronoiType == "solid") index = 2;
        else if(m_voronoiType == "worley") index = 3;
        propertiesPanel->setProperty("type", index);
        propertiesPanel->setProperty("startScale", voronoiScale());
        propertiesPanel->setProperty("startScaleX", scaleX());
        propertiesPanel->setProperty("startScaleY", scaleY());
        propertiesPanel->setProperty("startJitter", jitter());
        propertiesPanel->setProperty("startInverse", inverse());
        propertiesPanel->setProperty("startIntensity", intensity());
        propertiesPanel->setProperty("startBorders", bordersSize());
    }
}

VoronoiParams VoronoiNode::crystalsParam() {
    return m_crystals;
}

VoronoiParams VoronoiNode::bordersParam() {
    return m_borders;
}

VoronoiParams VoronoiNode::solidParam() {
    return m_solid;
}

VoronoiParams VoronoiNode::worleyParam() {
    return m_worley;
}

QString VoronoiNode::voronoiType() {
    return m_voronoiType;
}

void VoronoiNode::setVoronoiType(QString type) {
    m_voronoiType = type;
    voronoiTypeChanged(type);
    propertiesPanel->setProperty("startScale", voronoiScale());
    propertiesPanel->setProperty("startScaleX", scaleX());
    propertiesPanel->setProperty("startScaleY", scaleY());
    propertiesPanel->setProperty("startJitter", jitter());
    propertiesPanel->setProperty("startIntensity", intensity());
    propertiesPanel->setProperty("startInverse", inverse());
    propertiesPanel->setProperty("startBorders", bordersSize());
}

int VoronoiNode::voronoiScale() {
    if(m_voronoiType == "crystals") return m_crystals.scale;
    else if(m_voronoiType == "borders") return m_borders.scale;
    else if(m_voronoiType == "solid") return m_solid.scale;
    else if(m_voronoiType == "worley") return m_worley.scale;
    else return m_crystals.scale;
}

void VoronoiNode::setVoronoiScale(int scale) {
    if(m_voronoiType == "crystals") m_crystals.scale = scale;
    else if(m_voronoiType == "borders") m_borders.scale = scale;
    else if(m_voronoiType == "solid") m_solid.scale = scale;
    else if(m_voronoiType == "worley") m_worley.scale = scale;
    voronoiScaleChanged(scale);
}

int VoronoiNode::scaleX() {
    if(m_voronoiType == "crystals") return m_crystals.scaleX;
    else if(m_voronoiType == "borders") return m_borders.scaleX;
    else if(m_voronoiType == "solid") return m_solid.scaleX;
    else if(m_voronoiType == "worley") return m_worley.scaleX;
    return m_crystals.scaleX;
}

void VoronoiNode::setScaleX(int scale) {
    if(m_voronoiType == "crystals") m_crystals.scaleX = scale;
    else if(m_voronoiType == "borders") m_borders.scaleX = scale;
    else if(m_voronoiType == "solid") m_solid.scaleX = scale;
    else if(m_voronoiType == "worley") m_worley.scaleX = scale;
    scaleXChanged(scale);
}

int VoronoiNode::scaleY() {
    if(m_voronoiType == "crystals") return m_crystals.scaleY;
    else if(m_voronoiType == "borders") return m_borders.scaleY;
    else if(m_voronoiType == "solid") return m_solid.scaleY;
    else if(m_voronoiType == "worley") return m_worley.scaleY;
    return m_crystals.scaleY;
}

void VoronoiNode::setScaleY(int scale) {
    if(m_voronoiType == "crystals") m_crystals.scaleY = scale;
    else if(m_voronoiType == "borders") m_borders.scaleY = scale;
    else if(m_voronoiType == "solid") m_solid.scaleY = scale;
    else if(m_voronoiType == "worley") m_worley.scaleY = scale;
    scaleYChanged(scale);
}

float VoronoiNode::jitter() {
    if(m_voronoiType == "crystals") return m_crystals.jitter;
    else if(m_voronoiType == "borders") return m_borders.jitter;
    else if(m_voronoiType == "solid") return m_solid.jitter;
    else if(m_voronoiType == "worley") return m_worley.jitter;
    else return m_crystals.jitter;
}

void VoronoiNode::setJitter(float jitter) {
    if(m_voronoiType == "crystals") m_crystals.jitter = jitter;
    else if(m_voronoiType == "borders") m_borders.jitter = jitter;
    else if(m_voronoiType == "solid") m_solid.jitter = jitter;
    else if(m_voronoiType == "worley") m_worley.jitter = jitter;
    jitterChanged(jitter);
}

bool VoronoiNode::inverse() {
    if(m_voronoiType == "crystals") return m_crystals.inverse;
    else if(m_voronoiType == "borders") return m_borders.inverse;
    else if(m_voronoiType == "solid") return m_solid.inverse;
    else if(m_voronoiType == "worley") return m_worley.inverse;
    else return m_crystals.inverse;
}

void VoronoiNode::setInverse(bool inverse) {
    if(m_voronoiType == "crystals") m_crystals.inverse = inverse;
    else if(m_voronoiType == "borders") m_borders.inverse = inverse;
    else if(m_voronoiType == "solid") m_solid.inverse = inverse;
    else if(m_voronoiType == "worley") m_worley.inverse = inverse;
    inverseChanged(inverse);
}

float VoronoiNode::intensity() {
    if(m_voronoiType == "crystals") return m_crystals.intensity;
    else if(m_voronoiType == "borders") return m_borders.intensity;
    else if(m_voronoiType == "solid") return m_solid.intensity;
    else if(m_voronoiType == "worley") return m_worley.intensity;
    else return m_crystals.intensity;
}

void  VoronoiNode::setIntensity(float intensity) {
    if(m_voronoiType == "crystals") m_crystals.intensity = intensity;
    else if(m_voronoiType == "borders") m_borders.intensity = intensity;
    else if(m_voronoiType == "solid") m_solid.intensity = intensity;
    else if(m_voronoiType == "worley") m_worley.intensity = intensity;
    intensityChanged(intensity);
}

float VoronoiNode::bordersSize() {
    if(m_voronoiType == "borders") return m_borders.borders;
    else return 0.0f;
}

void VoronoiNode::setBordersSize(float size) {
    if(m_voronoiType == "borders") {
        m_borders.borders = size;
        bordersSizeChanged(size);
    }
}

void VoronoiNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void VoronoiNode::updatePrev(bool sel) {
    preview->selectedItem = sel;
    if(sel) {
         updatePreview(preview->texture(), true);
    }
}

void VoronoiNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void VoronoiNode::previewGenerated() {
    preview->generatedVoronoi = true;
    preview->update();
}

void VoronoiNode::updateVoronoiType(QString type) {
    setVoronoiType(type);
    dataChanged();
}

void VoronoiNode::updateVoronoiScale(int scale) {
    setVoronoiScale(scale);
    dataChanged();
}

void VoronoiNode::updateScaleX(int scale) {
    setScaleX(scale);
    dataChanged();
}

void VoronoiNode::updateScaleY(int scale) {
    setScaleY(scale);
    dataChanged();
}

void VoronoiNode::updateJitter(qreal jitter) {
    setJitter(jitter);
    dataChanged();
}

void VoronoiNode::updateInverse(bool inverse) {
    setInverse(inverse);
    dataChanged();
}

void VoronoiNode::updateIntensity(qreal intensity) {
    setIntensity(intensity);
    dataChanged();
}

void VoronoiNode::updateBordersSize(qreal size) {
    setBordersSize(size);
    dataChanged();
}
