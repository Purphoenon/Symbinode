/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "scene.h"
#include <iostream>
#include "albedonode.h"
#include "metalnode.h"
#include "roughnode.h"
#include "noisenode.h"
#include "mixnode.h"
#include "normalmapnode.h"
#include "normalnode.h"
#include "heightnode.h"
#include "emissionnode.h"
#include "voronoinode.h"
#include "polygonnode.h"
#include "circlenode.h"
#include "transformnode.h"
#include "tilenode.h"
#include "warpnode.h"
#include "blurnode.h"
#include "inversenode.h"
#include "colorrampnode.h"
#include "colornode.h"
#include "coloringnode.h"
#include "mappingnode.h"
#include "mirrornode.h"
#include "brightnesscontrastnode.h"
#include "thresholdnode.h"
#include "grayscalenode.h"
#include <QtWidgets/QFileDialog>

Scene::Scene(QQuickItem *parent, QVector2D resolution): QQuickItem (parent), m_resolution(resolution)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    m_background = new BackgroundObject(this);
    m_preview3d = new Preview3DObject();
    m_undoStack = new QUndoStack(this);
    m_undoStack->setUndoLimit(32);   
    rectView = new QQuickView();
    setClip(true);
    connect(this, &Scene::resolutionUpdate, m_preview3d, &Preview3DObject::setTexResolution);
}

Scene::~Scene() {
    startSocket = nullptr;
    dragEdge = nullptr;
    cutLine = nullptr;
    m_selectedItem.clear();
    if(rectSelect) delete rectSelect;
    if(rectView) delete rectView;
    if(cutLine) delete cutLine;
    for(Node *node: m_nodes) {
        delete node;
    }
    m_nodes.clear();

    for(Edge *edge: m_edges) {
        delete edge;
    }
    m_edges.clear();

    for(Frame *frame: m_frames) {
        delete frame;
    }
    m_frames.clear();

    delete m_background;
    delete m_undoStack;
    delete m_preview3d;
}

QList<Node *> Scene::nodes() const {
    return m_nodes;
}

void Scene::setNodes(const QList<Node *> &nodes) {
    m_nodes = nodes;
}

QList<Edge *> Scene::edges() const {
    return m_edges;
}

void Scene::setEdges(const QList<Edge *> &edges) {
    m_edges = edges;
}

BackgroundObject *Scene::background() const {
    return m_background;
}

Preview3DObject *Scene::preview3d() const {
    return m_preview3d;
}

bool Scene::addSelected(QQuickItem *item) {
    if(m_selectedItem.contains(item)) return false;
    m_selectedItem.push_back(item);
    if(qobject_cast<Node*>(item)) {
        Node *n = qobject_cast<Node*>(item);
        n->setZ(5);
        //m_nodes.move(m_nodes.indexOf(n), 0);
        m_activeItem = n;
        activeItemChanged();
    }
    else if(qobject_cast<Frame*>(item)) {
        Frame *f = qobject_cast<Frame*>(item);
        f->setZ(1);
        //m_frames.move(m_frames.indexOf(f), 0);
        m_activeItem = f;
        activeItemChanged();
    }
    return true;
}

bool Scene::deleteSelected(QQuickItem *item) {
    if(m_selectedItem.indexOf(item) < 0) return false;
    m_selectedItem.removeOne(item);
    if(qobject_cast<Node*>(item) ) {
        item->setZ(4);
    }
    else if(qobject_cast<Frame*>(item)) {
        item->setZ(0);
    }
    if(item == m_activeItem) {
        m_activeItem = nullptr;
        activeItemChanged();
    }
    return true;
}

void Scene::clearSelected() {
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            n->setSelected(false);
            n->setZ(4);
        }
        else if(qobject_cast<Frame*>(item)) {
            Frame *f = qobject_cast<Frame*>(item);
            f->setSelected(false);
            f->setZ(0);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *e = qobject_cast<Edge*>(item);
            e->setSelected(false);
        }
    }
    m_selectedItem.clear();
    m_activeItem = nullptr;
    activeItemChanged();
}

int Scene::countSelected() {
    return m_selectedItem.count();
}

QQuickItem *Scene::atSelected(int idx) {
    return m_selectedItem.at(idx);
}

QQuickItem *Scene::activeItem() {
    return m_activeItem;
}

int Scene::nodesCount() {
    return m_nodes.count();
}

Node *Scene::node(int idx) {
    return m_nodes.at(idx);
}

void Scene::deleteNode(Node *node) {
    if(m_activeItem == node) {
        m_activeItem = nullptr;
        activeItemChanged();
    }
    m_nodes.removeOne(node);
    if(qobject_cast<AlbedoNode*>(node)) {
        AlbedoNode * albedoNode = qobject_cast<AlbedoNode*>(node);
        disconnect(albedoNode, &AlbedoNode::albedoChanged, m_preview3d, &Preview3DObject::updateAlbedo);
        disconnect(this, &Scene::outputsSave, albedoNode, &AlbedoNode::saveAlbedo);
        m_albedoConnected = false;
        m_preview3d->updateAlbedo(QVector3D(1.0f, 1.0f, 1.0f), false);
    }
    else if(qobject_cast<MetalNode*>(node)) {
        MetalNode *metalNode = qobject_cast<MetalNode*>(node);
        disconnect(metalNode, &MetalNode::metalChanged, m_preview3d, &Preview3DObject::updateMetal);
        disconnect(this, &Scene::outputsSave, metalNode, &MetalNode::saveMetal);
        m_metalConnected = false;
        m_preview3d->updateMetal(0.0f, false);
    }
    else if(qobject_cast<RoughNode*>(node)) {
        RoughNode *roughNode = qobject_cast<RoughNode*>(node);
        disconnect(roughNode, &RoughNode::roughChanged, m_preview3d, &Preview3DObject::updateRough);
        disconnect(this, &Scene::outputsSave, roughNode, &RoughNode::saveRough);
        m_roughConnected = false;
        m_preview3d->updateRough(0.2f, false);
    }
    else if(qobject_cast<NormalNode*>(node)) {
        NormalNode *normNode = qobject_cast<NormalNode*>(node);
        disconnect(normNode, &NormalNode::normalChanged, m_preview3d, &Preview3DObject::updateNormal);
        disconnect(this, &Scene::outputsSave, normNode, &NormalNode::saveNormal);
        m_normalConnected = false;
        m_preview3d->updateNormal(0);
    }
    else if(qobject_cast<HeightNode*>(node)) {
        HeightNode *heightNode = qobject_cast<HeightNode*>(node);
        disconnect(heightNode, &HeightNode::heightChanged, m_preview3d, &Preview3DObject::updateHeight);
        disconnect(this, &Scene::outputsSave, heightNode, &HeightNode::heightSave);
        m_heightConnected = false;
        m_preview3d->updateHeight(0);
    }
    else if(qobject_cast<EmissionNode*>(node)) {
        EmissionNode *emissionNode = qobject_cast<EmissionNode*>(node);
        disconnect(emissionNode, &EmissionNode::emissionChanged, m_preview3d, &Preview3DObject::updateEmission);
        disconnect(this, &Scene::outputsSave, emissionNode, &EmissionNode::emissionSave);
        m_emissionConnected = false;
        m_preview3d->updateEmission(0);
    }
    disconnect(node, &Node::dataChanged, this, &Scene::nodeDataChanged);
    disconnect(m_background, &BackgroundObject::scaleChanged, node, &Node::scaleUpdate);
    disconnect(m_background, &BackgroundObject::panChanged, node, &Node::setPan);
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::addNode(Node *node) {
    if(m_nodes.contains(node)) return;
    m_nodes.append(node);
    if(qobject_cast<AlbedoNode*>(node)) {
        AlbedoNode * albedoNode = qobject_cast<AlbedoNode*>(node);
        connect(albedoNode, &AlbedoNode::albedoChanged, m_preview3d, &Preview3DObject::updateAlbedo);
        connect(this, &Scene::outputsSave, albedoNode, &AlbedoNode::saveAlbedo);
        m_albedoConnected = true;
    }
    else if(qobject_cast<MetalNode*>(node)) {
        MetalNode *metalNode = qobject_cast<MetalNode*>(node);
        connect(metalNode, &MetalNode::metalChanged, m_preview3d, &Preview3DObject::updateMetal);
        connect(this, &Scene::outputsSave, metalNode, &MetalNode::saveMetal);
        m_metalConnected = true;
    }
    else if(qobject_cast<RoughNode*>(node)) {
        RoughNode *roughNode = qobject_cast<RoughNode*>(node);
        connect(roughNode, &RoughNode::roughChanged, m_preview3d, &Preview3DObject::updateRough);
        connect(this, &Scene::outputsSave, roughNode, &RoughNode::saveRough);
        m_roughConnected = true;
    }
    else if(qobject_cast<NormalNode*>(node)) {
        NormalNode *normNode = qobject_cast<NormalNode*>(node);
        connect(normNode, &NormalNode::normalChanged, m_preview3d, &Preview3DObject::updateNormal);
        connect(this, &Scene::outputsSave, normNode, &NormalNode::saveNormal);
        m_normalConnected = true;
    }
    else if(qobject_cast<HeightNode*>(node)) {
        HeightNode *heightNode = qobject_cast<HeightNode*>(node);
        connect(heightNode, &HeightNode::heightChanged, m_preview3d, &Preview3DObject::updateHeight);
        connect(this, &Scene::outputsSave, heightNode, &HeightNode::heightSave);
        m_heightConnected = true;
    }
    else if(qobject_cast<EmissionNode*>(node)) {
        EmissionNode *emissionNode = qobject_cast<EmissionNode*>(node);
        connect(emissionNode, &EmissionNode::emissionChanged, m_preview3d, &Preview3DObject::updateEmission);
        connect(this, &Scene::outputsSave, emissionNode, &EmissionNode::emissionSave);
        m_emissionConnected = true;
    }
    connect(node, &Node::dataChanged, this, &Scene::nodeDataChanged);
    connect(this, &Scene::resolutionUpdate, node, &Node::setResolution);
    connect(m_background, &BackgroundObject::scaleChanged, node, &Node::scaleUpdate);
    connect(m_background, &BackgroundObject::panChanged, node, &Node::setPan);
    node->scaleUpdate(m_background->viewScale());
    node->setPan(m_background->viewPan());
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
    std::cout << "scene add node" << std::endl;
}

Node *Scene::nodeAt(float x, float y) {
    for(Node *node: m_nodes) {
        if(node->x() <= x && node->x() + node->width() >= x && node->y() <= y && node->y() + node->height() >= y) {
            return node;
        }
    }
    return nullptr;
}

void Scene::nodeDataChanged() {
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::deleteEdge(Edge *edge) {
    m_edges.removeOne(edge);
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::addEdge(Edge *edge) {
    if(m_edges.contains(edge)) return;
    m_edges.append(edge);
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::deleteFrame(Frame *frame) {
    if(m_activeItem == frame) {
        m_activeItem = nullptr;
        activeItemChanged();
    }
    m_frames.removeOne(frame);
    disconnect(m_background, &BackgroundObject::panChanged, frame, &Frame::setPan);
    disconnect(m_background, &BackgroundObject::scaleChanged, frame, &Frame::setScaleView);
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::addFrame(Frame *frame) {
    if(m_frames.contains(frame)) return;
    m_frames.insert(0, frame);
    connect(m_background, &BackgroundObject::panChanged, frame, &Frame::setPan);
    connect(m_background, &BackgroundObject::scaleChanged, frame, &Frame::setScaleView);
    frame->setScaleView(m_background->viewScale());
    frame->setPan(m_background->viewPan());
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

Frame *Scene::frameAt(float x, float y) {
    for(Frame *frame: m_frames) {
        if(frame->selected()) continue;
        if((frame->x() <= x && frame->x() + frame->width() >= x) && (frame->y() <= y && frame->y() + frame->height() >= y)){
            return frame;
        }
    }
    return nullptr;
}

QList<QQuickItem*> Scene::selectedList() const {
    return m_selectedItem;
}

void Scene::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        if(!m_selectedItem.empty()) {
            QList<QQuickItem*> selected = m_selectedItem;
            clearSelected();
            selectedItems(selected);
        }
    }
    else {
        event->setAccepted(false);
    }
}

void Scene::mouseMoveEvent(QMouseEvent *event) {
    /*if(event->buttons() == Qt::LeftButton && isEdgeDrag) {
        if(startSocket->type() == OUTPUTS && startSocket->countEdge() > 0) {
            dragEdge->setEndPosition(QVector2D(event->pos().x(), event->pos().y()));
        }
        else {
            dragEdge->setStartPosition(QVector2D(event->pos().x(), event->pos().y()));
        }
    }*/
    if(event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        if(cutLine) {
            cutLine->addPoint(event->localPos());
            cutLine->update();
        }
        else {
            cutLine = new CutLine(this);
            cutLine->setWidth(width());
            cutLine->setHeight(height());
            cutLine->addPoint(event->localPos());
            cutLine->update();
        }
    }
    else if(event->buttons() == Qt::LeftButton) {
        if(!rectSelect) {
           // rectView = new QQuickView();
            rectView->setSource(QUrl(QStringLiteral("qrc:/qml/RectSelection.qml")));
            rectSelect = qobject_cast<QQuickItem*>(rectView->rootObject());
            rectSelect->setParentItem(this);
            rectSelect->setProperty("rotCenterX", event->pos().x());
            rectSelect->setProperty("rotCenterY", event->pos().y());
            rectSelect->setX(event->pos().x());
            rectSelect->setY(event->pos().y());
            rectSelect->setZ(5);
        }
        float difX = event->pos().x() - rectSelect->property("rotCenterX").toFloat();
        float difY = event->pos().y() - rectSelect->property("rotCenterY").toFloat();
        if(difX < 0) {
            rectSelect->setX(event->pos().x());
        }
        else {
            rectSelect->setX(rectSelect->property("rotCenterX").toReal());
        }
        if(difY < 0) {
            rectSelect->setY(event->pos().y());
        }
        else {
            rectSelect->setY(rectSelect->property("rotCenterY").toReal());
        }
        rectSelect->setWidth(abs(difX));
        rectSelect->setHeight(abs(difY));

        for(auto n: m_nodes) {
            if(rectSelect->x() + rectSelect->width() < n->x() + 6 || rectSelect->x() > n->x() + n->width() - 6 ||
               rectSelect->y() + rectSelect->height() < n->y() || rectSelect->y() > n->y() + n->height()) {
                if(n->selected()) {
                    n->setSelected(false);
                    m_selectedItem.removeOne(n);
                }
            }
            else {
                if(!n->selected()) {
                    n->setSelected(true);
                    m_selectedItem.push_back(n);
                }
            }
        }
        for(auto f: m_frames) {
            if(rectSelect->x() + rectSelect->width() < f->x() || rectSelect->x() > f->x() + f->width() ||
               rectSelect->y() + rectSelect->height() < f->y() || rectSelect->y() > f->y() + 45.0f*m_background->viewScale()) {
                if(f->selected()) {
                    f->setSelected(false);
                    m_selectedItem.removeOne(f);
                }
            }
            else {
                if(!f->selected()) {
                    f->setSelected(true);
                    m_selectedItem.push_back(f);
                }
            }
        }
    }
}

void Scene::mouseReleaseEvent(QMouseEvent *event) {
    if(rectSelect) {
        delete rectSelect;
        //delete rectView;
        rectSelect = nullptr;
        //rectView = nullptr;
        selectedItems(QList<QQuickItem*>());
    }
    if(cutLine) {
        QList<QQuickItem*> intersectedEdges;
        int cutLineSize = cutLine->pointCount();
        for(auto e: m_edges) {
            for(int i = 0; i < cutLineSize - 1; ++i) {
                bool intersected = e->intersectWith(cutLine->pointAt(i), cutLine->pointAt(i + 1));
                if(intersected) {
                    intersectedEdges.append(e);
                    break;
                }
            }
        }
        if(intersectedEdges.size() > 0) deletedItems(intersectedEdges);
        delete cutLine;
        cutLine = nullptr;
    }
}

void Scene::serialize(QJsonObject &json) const {
    QJsonArray framesArray;
    for(auto f: m_frames) {
        QJsonObject frameObject;
        f->serialize(frameObject);
        framesArray.append(frameObject);
    }
    json["frames"] = framesArray;
    QJsonArray nodesArray;
    for(auto n: m_nodes) {
        if(n->attachedFrame()) continue;
        QJsonObject nodeObject;
        n->serialize(nodeObject);
        nodesArray.append(nodeObject);
    }
    json["nodes"] = nodesArray;
    QJsonArray edgesArray;
    for(auto e: m_edges) {
        QJsonObject edgeObject;
        e->serialize(edgeObject);
        edgesArray.append(edgeObject);
    }

    json["edges"] = edgesArray;
    json["panX"] = background()->viewPan().x();
    json["panY"] = background()->viewPan().y();
    json["scale"] = background()->viewScale();
    json["resX"] = m_resolution.x();
    json["resY"] = m_resolution.y();
}

void Scene::deserialize(const QJsonObject &json) {
    background()->setViewScale(1.0f);
    background()->setViewPan(QVector2D(0, 0));
    if(json.contains("resX") && json.contains("resY")) {
        m_resolution = QVector2D(json["resX"].toInt(), json["resY"].toInt());
        m_preview3d->setTexResolution(m_resolution);
    }

    QHash<QUuid, Socket*> socketsHash;
    if(json.contains("frames") && json["frames"].isArray()) {
        QJsonArray frames = json["frames"].toArray();
        for(int i = 0; i < frames.size(); ++i) {
            QJsonObject framesObject = frames[i].toObject();
            Frame *frame = new Frame(this);
            addFrame(frame);
            frame->deserialize(framesObject, socketsHash);
        }
    }

    if(json.contains("nodes") && json["nodes"].isArray()) {
        QJsonArray nodes = json["nodes"].toArray();
        for(int i = 0; i < nodes.size(); ++i) {
            QJsonObject nodesObject = nodes[i].toObject();
            if(nodesObject.contains("type")) {
                Node *node = deserializeNode(nodesObject);
                if(node) {
                    addNode(node);
                    node->deserialize(nodesObject, socketsHash);
                }
            }
        }
    }

    if(json.contains("edges") && json["edges"].isArray()) {
        QJsonArray edges = json["edges"].toArray();
        for(int i = 0; i < edges.size(); ++i) {
            QJsonObject edgesObject = edges[i].toObject();
            Edge *e = new Edge(this);
            e->deserialize(edgesObject, socketsHash);
            if(e->startSocket() && e->endSocket()) m_edges.append(e);
            else delete e;
        }
    }
}

Node *Scene::deserializeNode(const QJsonObject &json) {
    int nodeType = json["type"].toInt();
    Node *node = nullptr;
    switch (nodeType) {
    case 0:
        node = new ColorRampNode(this, m_resolution);
        break;
    case 1:
        node = new ColorNode(this, m_resolution);
        break;
    case 2:
        node = new ColoringNode(this, m_resolution);
        break;
    case 3:
        node = new MappingNode(this, m_resolution);
        break;
    case 4:
        node = new HeightNode(this, m_resolution);
        break;
    case 5:
        node = new MirrorNode(this, m_resolution);
        break;
    case 6:
        node = new NoiseNode(this, m_resolution);
        break;
    case 7:
        node = new MixNode(this, m_resolution);
        break;
    case 8:
        node = new AlbedoNode(this, m_resolution);
        break;
    case 9:
        node = new MetalNode(this, m_resolution);
        break;
    case 10:
        node = new RoughNode(this, m_resolution);
        break;
    case 11:
        node = new NormalMapNode(this, m_resolution);
        break;
    case 12:
        node = new NormalNode(this, m_resolution);
        break;
    case 13:
        node = new VoronoiNode(this, m_resolution);
        break;
    case 14:
        node = new PolygonNode(this, m_resolution);
        break;
    case 15:
        node = new CircleNode(this, m_resolution);
        break;
    case 16:
        node = new TransformNode(this, m_resolution);
        break;
    case 17:
        node = new TileNode(this, m_resolution);
        break;
    case 18:
        node = new WarpNode(this, m_resolution);
        break;
    case 19:
        node = new BlurNode(this, m_resolution);
        break;
    case 20:
        node = new InverseNode(this, m_resolution);
        break;
    case 21:
        node = new BrightnessContrastNode(this, m_resolution);
        break;
    case 22:
        node = new ThresholdNode(this, m_resolution);
        break;
    case 23:
        node = new EmissionNode(this, m_resolution);
        break;
    case 24:
        node = new GrayscaleNode(this, m_resolution);
        break;
    default:
        std::cout << "nonexistent type" << std::endl;
    }
    return node;
}

void Scene::deleteItems() {
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            QList<Edge*> edges = node->getEdges();
            for(auto edge: edges) {
                m_selectedItem.append(edge);
            }
        }
    }
    deletedItems(m_selectedItem);
    m_selectedItem.clear();
}

bool Scene::saveScene(QString fileName) {
    QString name = fileName;
    if(fileName.isEmpty()) {
        name = QFileDialog::getSaveFileName(nullptr,
                tr("Save Node Scene"), "",
                tr("Node Scene (*.sne);"));
    }
    QFile saveFile(name);
    if(!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn`t open save file.");
        return false;
    }
    QJsonObject sceneObject;
    serialize(sceneObject);
    QJsonDocument saveDoc(sceneObject);
    saveFile.write(saveDoc.toJson());
    m_fileName = name;
    m_modified = false;
    fileNameUpdate(m_fileName, false);
    return true;
}

bool Scene::loadScene(QString fileName) {
    QFile loadFile(fileName);
    if(!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn`t open save file.");
        return false;
    }
    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    deserialize(loadDoc.object());
    m_modified = false;
    m_fileName = fileName;
    fileNameUpdate(m_fileName, false);
    return true;
}

QString Scene::fileName() {
    return m_fileName;
}

bool Scene::isModified() {
    return m_modified;
}

void Scene::undo() {
    m_undoStack->undo();
}

void Scene::redo() {
    m_undoStack->redo();
}

void Scene::cut() {
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            QList<Edge*> edges = node->getEdges();
            for(auto edge: edges) {
                m_selectedItem.append(edge);
            }
        }
    }
    deletedItems(m_selectedItem);
    clearSelected();
}

void Scene::removeFromFrame() {
    QList<QPair<QQuickItem*, Frame*>> data;
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            if(node->attachedFrame()) data.push_back(QPair<QQuickItem*, Frame*>(node, node->attachedFrame()));
        }
    }
    detachedFromFrame(data);
}

void Scene::addToFrame() {
    bool addFrame = false;
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            addFrame = true;
            break;
        }
    }
    if(addFrame) {
        Frame *frame = new Frame(this);
        addedFrame(frame);
    }
}

void Scene::movedNodes(QList<QQuickItem *> nodes, QVector2D vec, Frame *frame) {
    m_undoStack->push(new MoveCommand(nodes, vec, frame));
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::addedNode(Node *node) {
    m_undoStack->push(new AddNode(node, this));
}

void Scene::addedFrame(Frame *frame) {
    m_undoStack->push(new AddFrame(frame, this));
}

void Scene::addedEdge(Edge *edge) {
    m_undoStack->push(new AddEdge(edge, this));
}

void Scene::deletedItems(QList<QQuickItem *> items) {
    m_undoStack->push(new DeleteCommand(items, this));
}

void Scene::selectedItems(QList<QQuickItem *> items) {
    m_undoStack->push(new SelectCommand(m_selectedItem, items, this));
}

void Scene::pastedItems(QList<QQuickItem *> items) {
     m_undoStack->push(new PasteCommand(items, this));
}

void Scene::movedEdge(Edge *edge, Socket *oldEndSocket, Socket *newEndSocket) {
    m_undoStack->push(new MoveEdgeCommand(edge, oldEndSocket, newEndSocket));
}

void Scene::itemPropertyChanged(QQuickItem *item, const char *propName, QVariant newValue, QVariant oldValue) {
    m_undoStack->push(new PropertyChangeCommand(item, propName, newValue, oldValue));
}

void Scene::detachedFromFrame(QList<QPair<QQuickItem *, Frame *> > data) {
    m_undoStack->push(new DetachFromFrameCommand(data));
}

void Scene::resizedFrame(Frame *frame, float offsetX, float offsetY, float offsetWidth, float offsetHeight) {
    m_undoStack->push(new ResizeFrameCommand(frame, offsetX, offsetY, offsetWidth, offsetHeight));
}

void Scene::changedTitle(Frame *frame, QString newTitle, QString oldTitle) {
    m_undoStack->push(new ChangeTitleCommand(frame, newTitle, oldTitle));
}

bool Scene::albedoConnected() {
    return m_albedoConnected;
}

bool Scene::metalConnected() {
    return m_metalConnected;
}

bool Scene::roughConnected() {
    return m_roughConnected;
}

bool Scene::normalConnected() {
    return m_normalConnected;
}

bool Scene::heightConnected() {
    return m_heightConnected;
}

QVector2D Scene::resolution() {
    return m_resolution;
}

void Scene::setResolution(QVector2D res) {
    m_resolution = res;
    emit resolutionUpdate(res);
}
