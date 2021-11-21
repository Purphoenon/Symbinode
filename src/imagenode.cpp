#include <QtWidgets/QFileDialog>
#include <QRegularExpression>
#include <QMimeData>
#include <iostream>
#include "imagenode.h"

ImageNode::ImageNode(QQuickItem *parent, QVector2D resolution, QString file, float transX, float transY):
    Node(parent, resolution), m_filePath(file), m_transX(transX), m_transY(transY)
{
    setFlag(ItemAcceptsDrops, true);
    createSockets(1, 1);
    m_socketsInput[0]->setTip("Mask");
    setTitle("Image");
    preview = new ImageObject(grNode, m_resolution, m_transX, m_transY);
    preview->setTransformOrigin(TopLeft);
    preview->setWidth(174);
    preview->setHeight(174);
    preview->setX(3);
    preview->setY(30);    
    connect(preview, &ImageObject::updatePreview, this, &Node::updatePreview);
    connect(preview, &ImageObject::textureChanged, this, &ImageNode::setOutput);
    connect(this, &Node::changeResolution, preview, &ImageObject::setResolution);
    connect(this, &Node::generatePreview, this, &ImageNode::previewGenerated);
    propView = new QQuickView();
    propView->setSource(QUrl(QStringLiteral("qrc:/qml/ImageProperty.qml")));
    propertiesPanel = qobject_cast<QQuickItem*>(propView->rootObject());
    propertiesPanel->setProperty("startTransX", m_transX);
    propertiesPanel->setProperty("startTransY", m_transY);
    connect(propertiesPanel, SIGNAL(openFile()), this, SLOT(openFile()));
    connect(this, SIGNAL(fileChosed(QString)), propertiesPanel, SIGNAL(fileChosed(QString)));
    connect(propertiesPanel, SIGNAL(loadTexture(QString)), this, SLOT(loadTexture(QString)));
    connect(propertiesPanel, SIGNAL(transXChanged(qreal)), this, SLOT(updateTransX(qreal)));
    connect(propertiesPanel, SIGNAL(transYChanged(qreal)), this, SLOT(updateTransY(qreal)));
    connect(propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    if(!file.isEmpty()) {
        if(file.endsWith("png")) {
            propertiesPanel->setProperty("path", file);
        }
    }
}

ImageNode::~ImageNode() {
    delete preview;
}

unsigned int &ImageNode::getPreviewTexture() {
    return preview->texture();
}

void ImageNode::serialize(QJsonObject &json) const {
    Node::serialize(json);
    json["type"] = 38;
    json["path"] = m_filePath;
    json["transX"] = m_transX;
    json["transY"] = m_transY;
}

void ImageNode::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    Node::deserialize(json, hash);
    if(json.contains("transX")) {
        m_transX = json["transX"].toVariant().toFloat();
    }
    if(json.contains("transY")) {
        m_transY = json["transY"].toVariant().toFloat();
    }
    propertiesPanel->setProperty("startTransX", m_transX);
    propertiesPanel->setProperty("startTransY", m_transY);
    preview->setTranslateX(m_transX);
    preview->setTranslateY(m_transY);
    if(json.contains("path")) {
        QString path = json["path"].toString();
        if(path.endsWith("png")) {
            loadTexture(path);
        }
    }
}

void ImageNode::operation() {
    if(!m_socketsInput[0]->getEdges().isEmpty()) {
        Node *inputNode = static_cast<Node*>(m_socketsInput[0]->getEdges()[0]->startSocket()->parentItem());
        if(inputNode && inputNode->resolution() != m_resolution) return;
        if(m_socketsInput[0]->value() == 0 && deserializing) return;
    }
    preview->setMaskTexture(m_socketsInput[0]->value().toUInt());
    preview->update();
    if(deserializing) deserializing = false;
}

ImageNode *ImageNode::clone() {
    return new ImageNode(parentItem(), m_resolution, m_filePath, m_transX, m_transY);
}

void ImageNode::loadTexture(QString path) {
    m_filePath = path;
    preview->loadTexture(path);
    int idx = m_filePath.length() - m_filePath.lastIndexOf(QRegularExpression("/|\\\\")) - 1;
    m_fileName = m_filePath.right(idx);
    if(m_fileName.isEmpty()) m_fileName = "Open";
    propertiesPanel->setProperty("fileName", m_fileName);
}

void ImageNode::openFile() {
    QString fileName = QFileDialog::getOpenFileName(nullptr,
            tr("Open Texture"), "",
            tr("Texture (*.png);"));
    if(fileName.isEmpty()) return;
    fileChosed(fileName);
}

float ImageNode::translationX() {
    return m_transX;
}

void ImageNode::setTranslationX(float x) {
    if(m_transX == x) return;
    m_transX = x;
    preview->setTranslateX(m_transX);
    preview->update();
}

float ImageNode::translationY() {
    return m_transY;
}

void ImageNode::setTranslationY(float y) {
    if(m_transY == y) return;
    m_transY = y;
    preview->setTranslateY(m_transY);
    preview->update();
}

void ImageNode::setOutput() {
    m_socketOutput[0]->setValue(preview->texture());
}

void ImageNode::updateTransX(qreal x) {
    setTranslationX(x);
    dataChanged();
}

void ImageNode::updateTransY(qreal y) {
    setTranslationY(y);
    dataChanged();
}

void ImageNode::previewGenerated() {
    preview->loadTex = true;
    preview->update();
}
