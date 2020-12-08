#ifndef CLIPBOARD_H
#define CLIPBOARD_H
#include <QList>
#include <QQuickItem>

class Scene;
class Node;
class Edge;

class Clipboard
{
public:
    Clipboard();
    ~Clipboard();
    void cut(Scene *scene);
    void copy(Scene *scene);
    void paste(float posX, float posY, Scene *scene);
    void duplicate(Scene *scene);
    void clear();
private:
    QVector2D center;
    QList<Node*> clipboard_nodes;
    QList<Edge*> clipboard_edges;
};

#endif // CLIPBOARD_H
