#include "socket.h"
#include "scene.h"
#include "backgroundobject.h"
#include <iostream>

Socket::Socket(QQuickItem *parent):QQuickItem (parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    view = new QQuickView();
    view->setSource(QUrl(QStringLiteral("qrc:/qml/Socket.qml")));
    grSocket = qobject_cast<QQuickItem *>(view->rootObject());
    grSocket->setParentItem(this);
}

Socket::~Socket() {
    edges.clear();
    delete grSocket;
    delete view;   
}

socketType Socket::type() const {
    return m_type;
}

void Socket::setType(socketType t) {
    m_type = t;
    grSocket->setProperty("type", m_type);
}

int Socket::countEdge() {
    return edges.count();
}

QList<Edge*> Socket::getEdges() const {
    return edges;
}

void Socket::addEdge(Edge *edge) {    
    if(grSocket->property("additional").toBool())  {
        edge->setVisible(isVisible());
    }
    if(edges.contains(edge)) return;
    edges.append(edge);    
}

void Socket::deleteEdge(Edge *edge) {
    edges.removeOne(edge);
    if(m_type == INPUTS) setValue(0);
}

QVector2D Socket::globalPos() {
    return m_globalPos;
}

void Socket::setGlobalPos(QVector2D pos) {
    m_globalPos = pos;
    if(type() == INPUTS) {
        for(auto edge: edges) {
            edge->setEndPosition(pos);
        }
    }
    else {
        for(auto edge: edges) {
            edge->setStartPosition(pos);
        }
    }
    emit globalPosChanged(m_globalPos);
}

void Socket::hoverEnterEvent(QHoverEvent *event) {
    grSocket->setProperty("showTip", true);
}

void Socket::hoverLeaveEvent(QHoverEvent *event) {
     grSocket->setProperty("showTip", false);
}

void Socket::mousePressEvent(QMouseEvent *event) {
    Scene *scene = reinterpret_cast<Scene*>(parentItem()->parentItem());
    if(event->button() == Qt::LeftButton && !scene->isEdgeDrag) {
        scene->isEdgeDrag = true;
        scene->startSocket = this;
        bool output = m_type == OUTPUTS;
        if(!output && edges.count() > 0) {
            scene->dragEdge = edges[0];
        }
        else {
            Edge *edge = new Edge(scene);
            scene->dragEdge = edge;
            edge->setStartPosition(m_globalPos);
            edge->setEndPosition(m_globalPos);
        }
    }

}

void Socket::mouseMoveEvent(QMouseEvent *event) {
    Scene *scene = reinterpret_cast<Scene*>(parentItem()->parentItem());
    if(event->buttons() == Qt::LeftButton && scene->isEdgeDrag) {
        QPointF globalPos = mapToItem(scene, QPointF(event->pos().x(), event->pos().y()));
        if(scene->startSocket->type() == OUTPUTS || scene->startSocket->edges.count() > 0) {
            scene->dragEdge->setEndPosition(QVector2D(globalPos.x(), globalPos.y()));
        }
        else {
            scene->dragEdge->setStartPosition(QVector2D(globalPos.x(), globalPos.y()));
        }
    }
}

void Socket::mouseReleaseEvent(QMouseEvent *event) {
    Scene *scene = reinterpret_cast<Scene*>(parentItem()->parentItem());
    if(event->button() == Qt::LeftButton && scene->isEdgeDrag) {
        bool dragAccepted = false;
        bool connectedNodes = false;
        QPointF childPos = mapToItem(scene, QPointF(event->pos().x(), event->pos().y()));
        QQuickItem *item = scene->childAt(childPos.x(), childPos.y());
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            QPointF nodePos = mapToItem(n, QPointF(event->pos().x(), event->pos().y()));
            QQuickItem *child = n->childAt(nodePos.x(), nodePos.y());
            Node *parentNode = qobject_cast<Node*>(parentItem());
            connectedNodes = parentNode->checkConnected(n, m_type);
            if(qobject_cast<Socket*>(child)) {
                Socket *s = qobject_cast<Socket*>(child);
                QPointF globalPos = QPointF(s->globalPos().x(), s->globalPos().y());
                std::cout << globalPos.x() << " " << globalPos.y() << std::endl;
                bool existEdge = scene->startSocket->type() == INPUTS &&
                        scene->startSocket->edges.count() > 0;
                dragAccepted = (existEdge && s->type() == INPUTS &&
                                s->parentItem() != scene->dragEdge->startSocket()->parentItem() ) ||
                               (!existEdge && (scene->startSocket->type() != s->type() &&
                                scene->startSocket->parentItem() != s->parentItem()) &&
                                !connectedNodes);
                if(dragAccepted) {
                    if(existEdge) {
                        scene->dragEdge->setEndPosition(QVector2D(globalPos.x(), globalPos.y()));
                        if(s != scene->startSocket) {      
                            scene->startSocket->edges.removeOne(scene->dragEdge);
                            if(s->edges.count() > 0) {
                                QList<QQuickItem*> deletedEdge;
                                deletedEdge.append(s->edges[0]);
                                scene->deletedItems(deletedEdge);
                            }
                            s->edges.push_back(scene->dragEdge);
                            scene->dragEdge->setEndSocket(s);
                        }
                    }
                    else {
                        if(scene->startSocket->type() == OUTPUTS) {
                            scene->dragEdge->setEndPosition(QVector2D(globalPos.x(), globalPos.y()));
                            if(s->edges.count() > 0) {
                                QList<QQuickItem*> deletedEdge;
                                deletedEdge.append(s->edges[0]);
                                scene->deletedItems(deletedEdge);
                            }
                            scene->startSocket->edges.push_back(scene->dragEdge);
                            s->edges.push_back(scene->dragEdge);
                            scene->dragEdge->setStartSocket(scene->startSocket);
                            scene->dragEdge->setEndSocket(s);                          
                        }
                        else {
                            scene->startSocket->edges.push_back(scene->dragEdge);
                            s->edges.push_back(scene->dragEdge);
                            scene->dragEdge->setStartPosition(QVector2D(globalPos.x(), globalPos.y()));
                            scene->dragEdge->setStartSocket(s);
                            scene->dragEdge->setEndSocket(scene->startSocket);
                        }
                    }
                    scene->addEdge(scene->dragEdge);
                    scene->addedEdge(scene->dragEdge);
                }
            }
            else {
                qDebug("edge decline");
            }
        }
        else {
            qDebug("edge decline");
        }

        if(!dragAccepted) {
            if(scene->dragEdge->startSocket()) {
                scene->dragEdge->startSocket()->edges.removeOne(scene->dragEdge);
            }
            if(scene->dragEdge->endSocket()) {
                scene->dragEdge->endSocket()->edges.removeOne(scene->dragEdge);
            }         
            if(scene->dragEdge->startSocket() && scene->dragEdge->endSocket()) {
                QList<QQuickItem*> deletedEdge;
                deletedEdge.append(scene->dragEdge);
                scene->deletedItems(deletedEdge);
            }
            else {
                scene->dragEdge->destroyed();
                delete scene->dragEdge;
            }
        }

        scene->isEdgeDrag = false;
    }
}

void Socket::serialize(QJsonObject &json) const{
    json["name"] = objectName();
}

void Socket::setTip(QString text) {
    textTip = text;
    grSocket->setProperty("textTip", textTip);
    if(textTip == "Mask") grSocket->setProperty("mask", true);
}

void Socket::setAdditional(bool additional) {
    grSocket->setProperty("additional", additional);
}

void Socket::setValue(const QVariant &value) {
    m_value = value;
    if(m_type == INPUTS) {
        Node *node = qobject_cast<Node*>(parentItem());
        node->operation();
    }
    else {
        for(auto edge: edges) {
            edge->endSocket()->setValue(m_value);
        }
    }
}

QVariant Socket::value() {
    return m_value;
}

void Socket::reset() {
    m_value = 0;
}

void Socket::updateScale(float scale) {
    m_scale = scale;
    setWidth(16*scale);
    setHeight(16*scale);
    grSocket->setProperty("scaleView", scale);
}
