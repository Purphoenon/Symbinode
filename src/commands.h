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

#ifndef COMMANDS_H
#define COMMANDS_H
#include <QtWidgets/QUndoCommand>
#include <QVector2D>
#include <QQuickItem>

class Node;
class Edge;
class Frame;
class Scene;
class Socket;

class MoveCommand: public QUndoCommand {
public:
    MoveCommand(QList<QQuickItem*> nodes, QVector2D movVector, Frame *frame = nullptr, QUndoCommand *parent = nullptr);
    ~MoveCommand();
    void undo();
    void redo();
private:
    QList<QQuickItem*> m_nodes;
    QList<QVector2D> m_newPos;
    QVector2D m_movVector;
    Frame *m_frame;
    float m_oldFrameX;
    float m_oldFrameY;
    float m_oldFrameWidth;
    float m_oldFrameHeight;
};

class AddNode: public QUndoCommand {
public:
    AddNode(Node *node, Scene *scene, QUndoCommand *parent = nullptr);
    ~AddNode();
    void undo();
    void redo();
private:
    Scene *m_scene;
    Node *m_node;
};

class AddEdge: public QUndoCommand {
public:
    AddEdge(Edge *edge, Scene *scene, QUndoCommand *parent = nullptr);
    ~AddEdge();
    void undo();
    void redo();
private:
    Scene *m_scene;
    Edge *m_edge;
};

class AddFrame: public QUndoCommand {
public:
    AddFrame(Frame *frame, Scene *scene, QUndoCommand *parent = nullptr);
    ~AddFrame();
    void undo();
    void redo();
private:
    Scene *m_scene;
    Frame *m_frame;
};

class DeleteCommand: public QUndoCommand {
public:
    DeleteCommand(QList<QQuickItem*> items, Scene *scene, QUndoCommand *parent = nullptr);
    ~DeleteCommand();
    void undo();
    void redo();
private:
    Scene *m_scene;
    QList<QQuickItem*> m_items;
};

class SelectCommand: public QUndoCommand {
public:
    SelectCommand(QList<QQuickItem*> items, QList<QQuickItem*> oldSelected, Scene *scene, QUndoCommand *parent = nullptr);
    ~SelectCommand();
    void undo();
    void redo();
private:
    Scene *m_scene;
    QList<QQuickItem*> m_items;
    QList<QQuickItem*> m_oldSelected;
};

class PasteCommand: public QUndoCommand {
public:
    PasteCommand(QList<QQuickItem*> items, Scene *scene, QUndoCommand *parent = nullptr);
    ~PasteCommand();
    void undo();
    void redo();
private:
    Scene *m_scene;
    QList<QQuickItem*> m_pastedItems;
};

class PropertyChangeCommand: public QUndoCommand {
public:
    PropertyChangeCommand(QQuickItem* item, const char *propName, QVariant newValue, QVariant oldValue, QUndoCommand *parent = nullptr);
    ~PropertyChangeCommand();
    void undo();
    void redo();
private:
    QQuickItem *m_item = nullptr;
    const char *m_propName;
    QVariant m_oldValue;
    QVariant m_newValue;
};

class MoveEdgeCommand: public QUndoCommand {
public:
    MoveEdgeCommand(Edge *edge, Socket *oldEndSocket, Socket *newEndSocket, QUndoCommand *parent = nullptr);
    ~MoveEdgeCommand();
    void undo();
    void redo();
private:
    Edge *m_edge = nullptr;
    Socket *m_oldSocket = nullptr;
    Socket *m_newSocket = nullptr;
};

class DetachFromFrameCommand: public QUndoCommand {
public:
    DetachFromFrameCommand(QList<QPair<QQuickItem*, Frame*>> data, QUndoCommand *parent = nullptr);
    ~DetachFromFrameCommand();
    void undo();
    void redo();
private:
    QList<QPair<QQuickItem*, Frame*>> m_data;
};

class ResizeFrameCommand: public QUndoCommand {
public:
    ResizeFrameCommand(Frame *frame, float offsetX, float offsetY, float offsetWidth, float offsetHeight);
    ~ResizeFrameCommand();
    void undo();
    void redo();
    bool mergeWith(const QUndoCommand *command);
    int id() const;
private:
    Frame *m_frame;
    float m_offsetX;
    float m_offsetY;
    float m_offsetWidth;
    float m_offsetHeight;
};

class ChangeTitleCommand: public QUndoCommand {
public:
    ChangeTitleCommand(Frame *frame, QString newTitle, QString oldTitle);
    ~ChangeTitleCommand();
    void undo();
    void redo();
private:
    Frame *m_frame;
    QString m_newTitle;
    QString m_oldTitle;
};

#endif // COMMANDS_H
