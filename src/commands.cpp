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

#include "commands.h"
#include "node.h"
#include "edge.h"
#include "scene.h"
#include <iostream>

MoveCommand::MoveCommand(QList<Node*> nodes, QVector2D movVector, QUndoCommand *parent):
QUndoCommand(parent), m_nodes(nodes), m_movVector(movVector)
{
    for(auto n: m_nodes) {
        m_newPos.append(QVector2D(n->baseX(), n->baseY()));
    }
}

MoveCommand::~MoveCommand() {
    m_nodes.clear();
}

void MoveCommand::undo() {
    for(auto n: m_nodes) {
        n->setBaseX(n->baseX() - m_movVector.x());
        n->setBaseY(n->baseY() - m_movVector.y());
    }
}

void MoveCommand::redo() {
    for(int i = 0; i < m_nodes.count(); ++i) {
        Node *n = m_nodes[i];
        n->setBaseX(m_newPos[i].x());
        n->setBaseY(m_newPos[i].y());
    }
}

AddNode::AddNode(Node *node, Scene *scene, QUndoCommand *parent): QUndoCommand (parent), m_scene(scene), m_node(node)
{
}

AddNode::~AddNode() {
    m_node = nullptr;
    m_scene = nullptr;
}

void AddNode::undo() {
    m_scene->deleteNode(m_node);
    m_node->setParentItem(nullptr);
}

void AddNode::redo() {
    m_scene->addNode(m_node);
    m_node->setParentItem(m_scene);
    m_node->generatePreview();
}

AddEdge::AddEdge(Edge *edge, Scene *scene, QUndoCommand *parent): QUndoCommand (parent), m_scene(scene), m_edge(edge)
{
}

AddEdge::~AddEdge() {
    m_edge = nullptr;
    m_scene = nullptr;
}

void AddEdge::undo() {
    m_scene->deleteEdge(m_edge);
    m_edge->startSocket()->deleteEdge(m_edge);
    m_edge->endSocket()->deleteEdge(m_edge);
    m_edge->setParentItem(nullptr);
}

void AddEdge::redo() {
    m_scene->addEdge(m_edge);
    m_edge->startSocket()->addEdge(m_edge);
    m_edge->endSocket()->addEdge(m_edge);
    m_edge->setStartPosition(m_edge->startSocket()->globalPos());
    m_edge->setEndPosition(m_edge->endSocket()->globalPos());
    m_edge->setParentItem(m_scene);
    m_edge->endSocket()->setValue(m_edge->startSocket()->value());
}

DeleteCommand::DeleteCommand(QList<QQuickItem*> items, Scene *scene, QUndoCommand *parent): QUndoCommand (parent),
    m_scene(scene), m_items(items){
}

DeleteCommand::~DeleteCommand() {
    m_items.clear();
    m_scene = nullptr;
}

void DeleteCommand::undo() {
    m_scene->clearSelected();
    for(auto item: m_items) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            m_scene->addNode(node);
            node->setParentItem(m_scene);
            m_scene->addSelected(node);
            node->generatePreview();
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            m_scene->addEdge(edge);
            edge->startSocket()->addEdge(edge);
            edge->endSocket()->addEdge(edge);
            edge->setStartPosition(edge->startSocket()->globalPos());
            edge->setEndPosition(edge->endSocket()->globalPos());
            edge->setParentItem(m_scene);
            edge->endSocket()->setValue(edge->startSocket()->value());
        }
    }
}

void DeleteCommand::redo() {
    for(auto item: m_items) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            m_scene->deleteNode(node);
            node->setParentItem(nullptr);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            m_scene->deleteEdge(edge);
            edge->startSocket()->deleteEdge(edge);
            edge->endSocket()->deleteEdge(edge);
            edge->setSelected(false);
            edge->setParentItem(nullptr);
        }
    }
}

SelectCommand::SelectCommand(QList<QQuickItem*> items, QList<QQuickItem*> oldSelected, Scene *scene, QUndoCommand *parent): QUndoCommand (parent),
    m_scene(scene), m_items(items), m_oldSelected(oldSelected){
}

SelectCommand::~SelectCommand() {
    m_items.clear();
    m_oldSelected.clear();
    m_scene = nullptr;
}

void SelectCommand::undo() {
    m_scene->clearSelected();
    for(auto item: m_items) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setSelected(false);
            m_scene->deleteSelected(node);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setSelected(false);
            m_scene->deleteSelected(edge);
        }
    }
    for(auto item: m_oldSelected) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setSelected(true);
            m_scene->addSelected(node);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setSelected(true);
            m_scene->addSelected(edge);
        }
    }
}

void SelectCommand::redo() {
    m_scene->clearSelected();
    for(auto item: m_oldSelected) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setSelected(false);
            m_scene->deleteSelected(node);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setSelected(false);
            m_scene->deleteSelected(edge);
        }
    }
    for(auto item: m_items) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setSelected(true);
            m_scene->addSelected(node);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setSelected(true);
            m_scene->addSelected(edge);
        }
    }
}

PasteCommand::PasteCommand(QList<QQuickItem*> items, Scene *scene, QUndoCommand *parent):QUndoCommand (parent),
    m_scene(scene), m_pastedItems(items){
}

PasteCommand::~PasteCommand(){
    m_pastedItems.clear();
    m_scene = nullptr;
}

void PasteCommand::undo() {
    for(auto item: m_pastedItems) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setParentItem(nullptr);
            m_scene->deleteNode(node);
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setParentItem(nullptr);
            m_scene->deleteEdge(edge);
        }
    }
    m_scene->clearSelected();
}

void PasteCommand::redo() {
    m_scene->clearSelected();
    for(auto item: m_pastedItems) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            node->setParentItem(m_scene);
            node->setSelected(true);
            m_scene->addNode(node);
            m_scene->addSelected(node);
            node->generatePreview();
        }
        else if(qobject_cast<Edge*>(item)) {
            Edge *edge = qobject_cast<Edge*>(item);
            edge->setParentItem(m_scene);
            m_scene->addEdge(edge);
        }
    }
}
