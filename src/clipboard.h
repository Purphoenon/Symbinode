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
