#include "grayscalenode.h"

GrayscaleNode::GrayscaleNode(QQuickItem *parent, QVector2D resolution, GLint bpc):
    Node(parent, resolution, bpc)
{
    createSockets(1, 1);
    setTitle("Grayscale");
    m_socketsInput[0]->setTip("Texture");
    preview = new GrayscaleObject(grNode, m_resolution, m_bpc);
    float s = scaleView();
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3*s);
    preview->setY(30*s);
    preview->setScale(s);
    connect(this, &Node::changeScaleView, this, &GrayscaleNode::updateScale);
    connect(this, &Node::generatePreview, this, &GrayscaleNode::previewGenerated);
    connect(this, &Node::changeResolution, preview, &GrayscaleObject::setResolution);
    connect(this, &Node::changeBPC, preview, &GrayscaleObject::setBPC);
    connect(preview, &GrayscaleObject::textureChanged, this, &GrayscaleNode::setOutput);
    connect(preview, &GrayscaleObject::updatePreview, this, &GrayscaleNode::updatePreview);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/BitsProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
    connect(propertiesPanel, SIGNAL(bitsChanged(int)), this, SLOT(bpcUpdate(int)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
}

GrayscaleNode::~GrayscaleNode() {
    delete preview;
}

void GrayscaleNode::operation() {
    preview->setSourceTexture(m_socketsInput[0]->value().toUInt());
    if(m_socketsInput[0]->countEdge() == 0) m_socketOutput[0]->setValue(0);
}

unsigned int &GrayscaleNode::getPreviewTexture() {
    return preview->texture();
}

void GrayscaleNode::saveTexture(QString fileName) {
    preview->saveTexture(fileName);
}

GrayscaleNode *GrayscaleNode::clone() {
    return new GrayscaleNode(parentItem(), m_resolution, m_bpc);
}

void GrayscaleNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 24;
}

void GrayscaleNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(m_bpc == GL_RGBA8) propertiesPanel->setProperty("startBits", 0);
    else if(m_bpc == GL_RGBA16) propertiesPanel->setProperty("startBits", 1);
}

void GrayscaleNode::updateScale(float scale) {
    preview->setX(3*scale);
    preview->setY(30*scale);
    preview->setScale(scale);
}

void GrayscaleNode::previewGenerated() {
    preview->grayscaledTex = true;
    preview->update();
}

void GrayscaleNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}
