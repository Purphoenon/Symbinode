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

#ifndef SCENE_H
#define SCENE_H
#include <QQuickItem>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QMenu>
#include "node.h"
#include "edge.h"
#include "backgroundobject.h"
#include "commands.h"
#include "clipboard.h"
#include "preview3d.h"

class Scene: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(BackgroundObject *background READ background)
    Q_PROPERTY(QQmlListProperty<Node> childrenNode READ childrenNode)
public:
    Scene(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    ~Scene();
    QQmlListProperty<Node> childrenNode();
    int nodesCount();
    Node* node(int idx);
    Node* activeNode();
    QList<Node*> nodes() const;
    void setNodes(const QList<Node*> &nodes);
    QList<Edge*> edges() const;
    void setEdges(const QList<Edge*> &edges);
    BackgroundObject *background() const;
    Preview3DObject *preview3d() const;
    void deleteNode(Node* node);
    void addNode(Node *node);
    void nodeDataChanged();
    void deleteEdge(Edge* edge);
    void addEdge(Edge* edge);
    QList<QQuickItem*> selectedList() const;
    bool addSelected(QQuickItem *item);
    bool deleteSelected(QQuickItem *item);
    int countSelected();
    QQuickItem *atSelected(int idx);
    void clearSelected();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
    void deleteItems();
    bool saveScene(QString fileName);
    bool loadScene(QString fileName);
    QString fileName();
    void undo();
    void redo();
    void cut();
    void movedNodes(QList<Node*> nodes, QVector2D vec);
    void addedEdge(Edge *edge);
    void addedNode(Node *node);
    void deletedItems(QList<QQuickItem*> items);
    void selectedItems(QList<QQuickItem*> items);
    void pastedItems(QList<QQuickItem*> items);
    bool albedoConnected();
    bool metalConnected();
    bool roughConnected();
    bool normalConnected();
    QVector2D resolution();
    void setResolution(QVector2D res);

    bool isEdgeDrag = false;
    Socket* startSocket = nullptr;
    Edge* dragEdge = nullptr;
    QQuickItem* rectSelect = nullptr;
    QQuickView* rectView = nullptr;
    QQuickItem* cutLine = nullptr;    
signals:
    void activeNodeChanged();
    void previewUpdate(QVariant previewData, bool useTexture);
    void fileNameUpdate(QString fileName, bool modified);
    void outputsSave(QString dir);
    void resolutionUpdate(QVector2D res);
private:
    static int nodesCount(QQmlListProperty<Node>* nodes);
    static Node* node(QQmlListProperty<Node>* nodes, int idx);
    BackgroundObject *m_background = nullptr;
    Preview3DObject *m_preview3d = nullptr;
    QList<Node*> m_nodes;
    QList<Edge*> m_edges;
    QList<QQuickItem*> m_selectedItem;
    Node *m_activeNode = nullptr;
    QString m_fileName = "";
    bool m_modified = false;
    QUndoStack *m_undoStack = nullptr;
    bool m_albedoConnected = false;
    bool m_metalConnected = false;
    bool m_roughConnected = false;
    bool m_normalConnected = false;
    QVector2D m_resolution;
};

#endif // SCENE_H
