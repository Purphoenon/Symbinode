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

#ifndef SOCKET_H
#define SOCKET_H
#include <QQuickItem>
#include <QQuickView>
#include <QJsonObject>
#include "edge.h"

enum socketType {INPUTS, OUTPUTS};

class Socket:public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QVector2D globalPos READ globalPos WRITE setGlobalPos NOTIFY globalPosChanged)
public:
    Socket(QQuickItem *parent);
    ~Socket();
    socketType type() const;
    void setType(socketType t);
    int countEdge();
    QList<Edge*> getEdges() const;
    void addEdge(Edge *edge);
    void deleteEdge(Edge *edge);
    QVector2D globalPos();
    void setGlobalPos(QVector2D pos);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    void serialize(QJsonObject &json) const;
    void setTip(QString text);
    void setAdditional(bool additional);
    void setValue(const QVariant &value);
    QVariant value();
    void reset();
    void updateScale(float scale);
signals:
    void globalPosChanged(QVector2D pos);
private:
    QVariant m_value = 0;
    QQuickView *view;
    QQuickItem *grSocket;
    socketType m_type;
    QList<Edge*> edges;
    QVector2D m_globalPos;
    float m_scale = 1.0f;
    QString textTip = "Socket";
};

#endif // SOCKET_H
