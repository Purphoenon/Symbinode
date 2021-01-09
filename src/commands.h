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
class Scene;

class MoveCommand: public QUndoCommand {
public:
    MoveCommand(QList<Node*> nodes, QVector2D movVector, QUndoCommand *parent = nullptr);
    ~MoveCommand();
    void undo();
    void redo();
private:
    QList<Node*> m_nodes;
    QList<QVector2D> m_newPos;
    QVector2D m_movVector;
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
    PropertyChangeCommand(Node* node, const char *propName, QVariant newValue, QVariant oldValue, QUndoCommand *parent = nullptr);
    ~PropertyChangeCommand();
    void undo();
    void redo();
private:
    Node *m_node = nullptr;
    const char *m_propName;
    QVariant m_oldValue;
    QVariant m_newValue;
};

#endif // COMMANDS_H
