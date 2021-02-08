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

MoveCommand::MoveCommand(QList<QQuickItem *> nodes, QVector2D movVector, Frame *frame, QUndoCommand *parent):
QUndoCommand(parent), m_nodes(nodes), m_movVector(movVector), m_frame(frame)
{    
    for(auto item: m_nodes) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            m_newPos.push_back(QVector2D(node->baseX(), node->baseY()));
        }
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            m_newPos.push_back(QVector2D(frame->baseX(), frame->baseY()));
        }
    }
    if(m_frame) {
        m_oldFrameX = m_frame->baseX();
        m_oldFrameY = m_frame->baseY();
        m_oldFrameWidth = m_frame->baseWidth();
        m_oldFrameHeight = m_frame->baseHeight();
    }
}

MoveCommand::~MoveCommand() {
    m_nodes.clear();
}

void MoveCommand::undo() {
    for(auto item: m_nodes) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            if(m_frame && node->attachedFrame() == m_frame) m_frame->removeItem(node);
            node->setBaseX(node->baseX() - m_movVector.x());
            node->setBaseY(node->baseY() - m_movVector.y());
        }
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setBaseX(frame->baseX() - m_movVector.x());
            frame->setBaseY(frame->baseY() - m_movVector.y());
        }
    }
    if(m_frame) {
        m_frame->setBaseX(m_oldFrameX);
        m_frame->setBaseY(m_oldFrameY);
        m_frame->setBaseWidth(m_oldFrameWidth);
        m_frame->setBaseHeight(m_oldFrameHeight);
    }
}

void MoveCommand::redo() {
    QList<QQuickItem*> nodesToFrame;
    for(int i = 0; i < m_nodes.count(); ++i) {
        QQuickItem *item = m_nodes[i];
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            n->setBaseX(m_newPos[i].x());
            n->setBaseY(m_newPos[i].y());
            if(m_frame && !n->attachedFrame()) nodesToFrame.push_back(n);
        }
        else if(qobject_cast<Frame*>(item)) {
            Frame *f = qobject_cast<Frame*>(item);
            f->setBaseX(m_newPos[i].x());
            f->setBaseY(m_newPos[i].y());
        }
    }
    if(m_frame) m_frame->addNodes(nodesToFrame);
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

AddFrame::AddFrame(Frame *frame, Scene *scene, QUndoCommand *parent): QUndoCommand (parent), m_scene(scene), m_frame(frame)
{

}

AddFrame::~AddFrame() {
    m_frame = nullptr;
    m_scene = nullptr;
}

void AddFrame::undo() {
    m_scene->deleteFrame(m_frame);
    m_frame->setParentItem(nullptr);
}

void AddFrame::redo() {
    m_scene->addFrame(m_frame);
    m_frame->setParentItem(m_scene);
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            m_scene->addFrame(frame);
            frame->setParentItem(m_scene);
            m_scene->addSelected(frame);
            for(auto item: frame->contentList()) {
                if(qobject_cast<Node*>(item)) {
                    Node *node = qobject_cast<Node*>(item);
                    node->setAttachedFrame(frame);
                }
            }
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            m_scene->deleteFrame(frame);
            frame->setParentItem(nullptr);
            for(auto item: frame->contentList()) {
                if(qobject_cast<Node*>(item)) {
                    Node *node = qobject_cast<Node*>(item);
                    node->setAttachedFrame(nullptr);
                }
            }
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setSelected(false);
            m_scene->deleteSelected(frame);
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setSelected(true);
            m_scene->addSelected(frame);
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setSelected(false);
            m_scene->deleteSelected(frame);
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
        else if (qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setSelected(true);
            m_scene->addSelected(frame);
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setParentItem(nullptr);
            m_scene->deleteFrame(frame);
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
        else if(qobject_cast<Frame*>(item)) {
            Frame *frame = qobject_cast<Frame*>(item);
            frame->setParentItem(m_scene);
            frame->setSelected(true);
            m_scene->addFrame(frame);
            m_scene->addSelected(frame);
        }
    }
}

PropertyChangeCommand::PropertyChangeCommand(Node* node, const char* propName, QVariant newValue,
                                             QVariant oldValue, QUndoCommand *parent):
    QUndoCommand (parent), m_node(node), m_propName(propName), m_oldValue(oldValue), m_newValue(newValue) {

}

PropertyChangeCommand::~PropertyChangeCommand() {
    m_node = nullptr;
    m_propName = nullptr;
}

void PropertyChangeCommand::undo() {
    m_node->setPropertyOnPanel(m_propName, m_oldValue);
}

void PropertyChangeCommand::redo() {
    m_node->setPropertyOnPanel(m_propName, m_newValue);
}

MoveEdgeCommand::MoveEdgeCommand(Edge *edge, Socket *oldEndSocket, Socket *newEndSocket,
                                 QUndoCommand *parent): QUndoCommand (parent), m_edge(edge),
    m_oldSocket(oldEndSocket), m_newSocket(newEndSocket){

}

MoveEdgeCommand::~MoveEdgeCommand() {
    m_edge = nullptr;
    m_oldSocket = nullptr;
    m_newSocket = nullptr;
}

void MoveEdgeCommand::undo() {
    m_newSocket->deleteEdge(m_edge);
    m_edge->setEndSocket(m_oldSocket);
    m_oldSocket->addEdge(m_edge);
    m_edge->setEndPosition(m_oldSocket->globalPos());
    m_oldSocket->setValue(m_edge->startSocket()->value());
}

void MoveEdgeCommand::redo() {
    m_oldSocket->deleteEdge(m_edge);
    m_edge->setEndSocket(m_newSocket);
    m_newSocket->addEdge(m_edge);
    m_edge->setEndPosition(m_newSocket->globalPos());
    m_newSocket->setValue(m_edge->startSocket()->value());
}

DetachFromFrameCommand::DetachFromFrameCommand(QList<QPair<QQuickItem *, Frame *> > data, QUndoCommand *parent):
    QUndoCommand (parent), m_data(data){

}

DetachFromFrameCommand::~DetachFromFrameCommand() {
    m_data.clear();
}

void DetachFromFrameCommand::undo() {
    for(auto pair: m_data) {
        pair.second->addNodes(QList<QQuickItem*>({pair.first}));
    }
}

void DetachFromFrameCommand::redo() {
    for(auto pair: m_data) {
        pair.second->removeItem(pair.first);
    }
}

ResizeFrameCommand::ResizeFrameCommand(Frame *frame, float offsetX, float offsetY, float offsetWidth,
                                       float offsetHeight): m_frame(frame), m_offsetX(offsetX),
    m_offsetY(offsetY), m_offsetWidth(offsetWidth), m_offsetHeight(offsetHeight) {

}

ResizeFrameCommand::~ResizeFrameCommand() {
    m_frame = nullptr;
}

void ResizeFrameCommand::undo() {
    m_frame->setBaseX(m_frame->baseX() - m_offsetX);
    m_frame->setBaseY(m_frame->baseY() - m_offsetY);
    m_frame->setBaseWidth(m_frame->baseWidth() - m_offsetWidth);
    m_frame->setBaseHeight(m_frame->baseHeight() - m_offsetHeight);
}

void ResizeFrameCommand::redo() {
    m_frame->setBaseX(m_frame->baseX() + m_offsetX);
    m_frame->setBaseY(m_frame->baseY() + m_offsetY);
    m_frame->setBaseWidth(m_frame->baseWidth() + m_offsetWidth);
    m_frame->setBaseHeight(m_frame->baseHeight() + m_offsetHeight);
}

bool ResizeFrameCommand::mergeWith(const QUndoCommand *command) {
    if(id() == command->id()) {
        const ResizeFrameCommand *other = static_cast<const ResizeFrameCommand*>(command);
        if(m_frame != other->m_frame) return false;
        m_offsetX += other->m_offsetX;
        m_offsetY += other->m_offsetY;
        m_offsetWidth += other->m_offsetWidth;
        m_offsetHeight += other->m_offsetHeight;
        return true;
    }
    return false;
}

int ResizeFrameCommand::id() const {
    return 1;
}

ChangeTitleCommand::ChangeTitleCommand(Frame *frame, QString newTitle, QString oldTitle): m_frame(frame),
    m_newTitle(newTitle), m_oldTitle(oldTitle) {

}

ChangeTitleCommand::~ChangeTitleCommand() {
    m_frame = nullptr;
}

void ChangeTitleCommand::undo() {
    m_frame->setTitle(m_oldTitle);
}

void ChangeTitleCommand::redo() {
    m_frame->setTitle(m_newTitle);
}
