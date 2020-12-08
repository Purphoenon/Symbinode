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

#endif // COMMANDS_H
