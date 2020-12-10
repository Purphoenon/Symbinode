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

#ifndef EDGE_H
#define EDGE_H
#include <QQuickItem>
#include <QQuickView>
#include <QJsonObject>
#include "cubicbezier.h"

class Socket;
class Scene;

class Edge: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector2D startPosition READ startPosition WRITE setStartPosition NOTIFY startPositionChanged)
    Q_PROPERTY(QVector2D endPosition READ endPosition WRITE setEndPosition NOTIFY endPositionChanged)
    Q_PROPERTY(bool selected READ selected WRITE setSelected)
public:
    Edge(QQuickItem *parent = nullptr);
    Edge(const Edge &edge);
    ~Edge();
    QVector2D startPosition();
    void setStartPosition(QVector2D pos);
    QVector2D endPosition();
    void setEndPosition(QVector2D pos);
    Socket *startSocket();
    void setStartSocket(Socket *socket);
    Socket *endSocket();
    void setEndSocket(Socket *socket);
    Socket *findSockets(Scene* scene, float x, float y);
    bool selected();
    void setSelected(bool selected);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json);
signals:
    void startPositionChanged(QVector2D pos);
    void endPositionChanged(QVector2D pos);
public slots:
    void updateScale(float scale);
    void pressedEdge(bool controlModifier);
private:
    CubicBezier *grEdge;
    QVector2D m_startPos;
    QVector2D m_endPos;
    Socket *m_startSocket = nullptr;
    Socket *m_endSocket = nullptr;
    float m_scale = 1.0f;
    bool m_selected = false;
};

#endif // EDGE_H
