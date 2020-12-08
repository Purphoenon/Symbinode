#include "scene.h"
#include <iostream>
#include "albedonode.h"
#include "metalnode.h"
#include "roughnode.h"
#include "noisenode.h"
#include "mixnode.h"
#include "normalmapnode.h"
#include "normalnode.h"
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
#include "splatnode.h"
#include "mirrornode.h"
#include "brightnesscontrastnode.h"
#include "thresholdnode.h"
#include <QtWidgets/QFileDialog>

Scene::Scene(QQuickItem *parent, QVector2D resolution): QQuickItem (parent), m_resolution(resolution)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    m_background = new BackgroundObject(this);
    m_preview3d = new Preview3DObject();
    m_undoStack = new QUndoStack(this);
    m_undoStack->setUndoLimit(32);   
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
    if(m_selectedItem.indexOf(item) >= 0) return false;
    m_selectedItem.push_back(item);
    if(qobject_cast<Node*>(item)) {
        m_activeNode = qobject_cast<Node*>(item);
        activeNodeChanged();
    }
    return true;
}

bool Scene::deleteSelected(QQuickItem *item) {
    if(m_selectedItem.indexOf(item) < 0) return false;
    m_selectedItem.removeOne(item);
    if(qobject_cast<Node*>(item) && qobject_cast<Node*>(item) == m_activeNode) {
        m_activeNode = nullptr;
        activeNodeChanged();
    }
    return true;
}

void Scene::clearSelected() {
    for(auto item: m_selectedItem) {
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            n->setSelected(false);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *e = qobject_cast<Edge*>(item);
            e->setSelected(false);
        }
    }
    m_selectedItem.clear();
    m_activeNode = nullptr;
    activeNodeChanged();
}

int Scene::countSelected() {
    return m_selectedItem.count();
}

QQuickItem *Scene::atSelected(int idx) {
    return m_selectedItem.at(idx);
}

QQmlListProperty<Node> Scene::childrenNode() {
    return {this, this,
        &Scene::nodesCount,
        &Scene::node
    };
}

int Scene::nodesCount(QQmlListProperty<Node> *nodes) {
    return reinterpret_cast<Scene*>(nodes->data)->nodesCount();
}

Node *Scene::node(QQmlListProperty<Node> *nodes, int idx) {
    return reinterpret_cast<Scene*>(nodes->data)->node(idx);
}

Node *Scene::activeNode() {
    return m_activeNode;
}

int Scene::nodesCount() {
    return m_nodes.count();
}

Node *Scene::node(int idx) {
    return m_nodes.at(idx);
}

void Scene::deleteNode(Node *node) {
    if(m_activeNode == node) {
        m_activeNode = nullptr;
        activeNodeChanged();
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
    disconnect(node, &Node::updatePreview, this, &Scene::previewUpdate);
    disconnect(node, &Node::dataChanged, this, &Scene::nodeDataChanged);
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
    connect(node, &Node::updatePreview, this, &Scene::previewUpdate);
    connect(node, &Node::dataChanged, this, &Scene::nodeDataChanged);
    connect(this, &Scene::resolutionUpdate, node, &Node::setResolution);
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
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
    if(event->buttons() == Qt::LeftButton && isEdgeDrag) {
        if(startSocket->type() == OUTPUTS && startSocket->countEdge() > 0) {
            dragEdge->setEndPosition(QVector2D(event->pos().x(), event->pos().y()));
        }
        else {
            dragEdge->setStartPosition(QVector2D(event->pos().x(), event->pos().y()));
        }
    }
    else if(event->buttons() == Qt::LeftButton) {
        if(!rectSelect) {
            rectView = new QQuickView();
            rectView->setSource(QUrl(QStringLiteral("qrc:/qml/RectSelection.qml")));
            rectSelect = qobject_cast<QQuickItem*>(rectView->rootObject());
            rectSelect->setParentItem(this);
            rectSelect->setProperty("rotCenterX", event->pos().x());
            rectSelect->setProperty("rotCenterY", event->pos().y());
            rectSelect->setX(event->pos().x());
            rectSelect->setY(event->pos().y());
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
    }
}

void Scene::mouseReleaseEvent(QMouseEvent *event) {
    if(rectSelect) {
        delete rectSelect;
        delete rectView;
        rectSelect = nullptr;
        rectView = nullptr;
        selectedItems(QList<QQuickItem*>());
    }
}

void Scene::serialize(QJsonObject &json) const {
    QJsonArray nodesArray;
    for(auto n: m_nodes) {
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
    }

    if(json.contains("nodes") && json["nodes"].isArray()) {
        QJsonArray nodes = json["nodes"].toArray();
        for(auto n: m_nodes) {
            delete n;
        }
        m_nodes.clear();
        for(int i = 0; i < nodes.size(); ++i) {
            QJsonObject nodesObject = nodes[i].toObject();
            if(nodesObject.contains("type")) {
                int nodeType = nodesObject["type"].toInt();
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
                /*case 4:
                    node = new SplatNode(this, m_resolution);
                    break;*/
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
                default:
                    std::cout << "nonexistent type" << std::endl;
                }
                if(node) {
                    addNode(node);
                    node->deserialize(nodesObject);     
                }
            }
        }
    }

    if(json.contains("edges") && json["edges"].isArray()) {
        QJsonArray edges = json["edges"].toArray();
        for(auto e: m_edges) {
            delete e;
        }
        m_edges.clear();
        for(int i = 0; i < edges.size(); ++i) {
            QJsonObject edgesObject = edges[i].toObject();
            Edge *e = new Edge(this);
            e->deserialize(edgesObject);
            m_edges.append(e);
        }
    }
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

void Scene::movedNodes(QList<Node *> nodes, QVector2D vec) {
    m_undoStack->push(new MoveCommand(nodes, vec));
    if(!m_modified) {
        m_modified = true;
        fileNameUpdate(m_fileName, m_modified);
    }
}

void Scene::addedNode(Node *node) {
    m_undoStack->push(new AddNode(node, this));
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

QVector2D Scene::resolution() {
    return m_resolution;
}

void Scene::setResolution(QVector2D res) {
    m_resolution = res;
    emit resolutionUpdate(res);
}
