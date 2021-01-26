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

#include "node.h"
#include "scene.h"
#include <iostream>
#include <QQmlProperty>


Node::Node(QQuickItem *parent, QVector2D resolution): QQuickItem (parent), m_resolution(resolution)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    view = new QQuickView();
    view->setSource(QUrl(QStringLiteral("qrc:/qml/Node.qml")));
    grNode = qobject_cast<QQuickItem *>(view->rootObject());
    grNode->setParentItem(this);
    grNode->setX(8);
    setZ(1);
}

Node::Node(const Node &node):Node() {
    setBaseX(node.m_baseX);
    setBaseY(node.m_baseY);
    createSockets(node.m_socketsInput.count(), node.m_socketOutput.count());
    createAdditionalInputs(node.m_additionalInputs.count());
}

Node::~Node() {
    for(auto s: m_socketsInput) {
        delete s;
    }

    for(auto s: m_socketOutput) {
        delete s;
    }

    for(auto s: m_additionalInputs) {
        delete s;
    }

    delete grNode;
    delete view;
    delete propertiesPanel;
    delete propView;
}

float Node::baseX() {
    return m_baseX;
}

void Node::setBaseX(float value) {
    m_baseX = value;
    setX(m_baseX*m_scale - m_pan.x());
    for(auto s: m_socketsInput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 6, s->y() + 6));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_socketOutput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 6, s->y() + 6));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_additionalInputs) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    emit changeBaseX(value);
}

float Node::baseY() {
    return m_baseY;
}

void Node::setBaseY(float value) {
    m_baseY = value;
    setY(m_baseY*m_scale - m_pan.y());
    for(auto s: m_socketsInput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_socketOutput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_additionalInputs) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    emit changeBaseY(value);
}

QVector2D Node::pan() {
    return m_pan;
}

void Node::setPan(QVector2D pan) {
    m_pan = pan;
    setX(baseX()*m_scale - m_pan.x());
    setY(baseY()*m_scale - m_pan.y());
    for(auto s: m_socketsInput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_socketOutput) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(auto s: m_additionalInputs) {
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    emit changePan(pan);
}

void Node::setResolution(QVector2D res) {
    m_resolution = res;
    emit changeResolution(res);
}

float Node::scaleView(){
    return m_scale;
}

bool Node::selected() {
    return m_selected;
}

void Node::setSelected(bool select) {
    m_selected = select;
    grNode->setProperty("selected", select);
    emit changeSelected(select);
}

bool Node::checkConnected(Node *node, socketType type) {
    if(type == OUTPUTS) {
        for(auto socket: m_socketsInput) {
            for(auto edge: socket->getEdges()) {
                if(qobject_cast<Node*>(edge->startSocket()->parentItem())) {
                    Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                    if(startNode == node) {
                        return true;
                    }
                    else {
                        bool connected = startNode->checkConnected(node, type);
                        if(connected) return true;
                    }
                }
            }
        }
        for(auto socket: m_additionalInputs) {
            for(auto edge: socket->getEdges()) {
                if(qobject_cast<Node*>(edge->startSocket()->parentItem())) {
                    Node *startNode = qobject_cast<Node*>(edge->startSocket()->parentItem());
                    if(startNode == node) {
                        return true;
                    }
                    else {
                        bool connected = startNode->checkConnected(node, type);
                        if(connected) return true;
                    }
                }
            }
        }
        return false;
    }
    else {
        for(auto socket: m_socketOutput) {
            for(auto edge: socket->getEdges()) {
                if(qobject_cast<Node*>(edge->endSocket()->parentItem())) {
                    Node *endNode = qobject_cast<Node*>(edge->endSocket()->parentItem());
                    if(endNode == node) {
                        return true;
                    }
                    else {
                        bool connected = endNode->checkConnected(node, type);
                        if(connected) return true;
                    }
                }
            }
        }        
        return false;
    }
}

QList<Edge*> Node::getEdges() const {
    QList<Edge*> edges;
    for(auto s: m_socketsInput) {
        edges.append(s->getEdges());
    }
    for(auto s: m_socketOutput) {
        edges.append(s->getEdges());
    }
    for(auto s: m_additionalInputs) {
        edges.append(s->getEdges());
    }
    return edges;
}

QQuickItem *Node::getPropertyPanel() {
    return propertiesPanel;
}

void Node::mousePressEvent(QMouseEvent *event) {

    if(event->pos().x() < 8*m_scale || event->pos().x() > 186*m_scale || event->pos().y() > 207*m_scale) {
        event->setAccepted(false);
        return;
    }
    moved = false;
    Scene *scene = reinterpret_cast<Scene*>(parentItem());
    QPointF point = mapToItem(scene, QPointF(event->pos().x(), event->pos().y())); 
    dragX = event->pos().x();
    dragY = event->pos().y();
    oldX = x();
    oldY = y();
    if(event->button() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
        setSelected(!m_selected);
        if(m_selected) {
            QList<QQuickItem*> selected = scene->selectedList();
            scene->addSelected(this);
            scene->selectedItems(selected);
        }
        else {
            QList<QQuickItem*> selected = scene->selectedList();
            scene->deleteSelected(this);
            scene->selectedItems(selected);
        }

    }
    else if(event->button() == Qt::LeftButton && !m_selected) {
        QList<QQuickItem*> selected = scene->selectedList();
        scene->clearSelected();
        setSelected(true);
        scene->addSelected(this);        
        scene->selectedItems(selected);
    }
    else if(event->button() != Qt::LeftButton) {
        event->setAccepted(false);
    }
}

void Node::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        moved = true;
        Scene* scene = reinterpret_cast<Scene*>(parentItem());
        QPointF point = mapToItem(scene, QPointF(event->pos().x(), event->pos().y()));
        setX(point.x() - dragX);
        setY(point.y() - dragY);
        int offsetBaseX = m_baseX - (x() + m_pan.x())/m_scale;
        int offsetBaseY = m_baseY - (y() + m_pan.y())/m_scale;
        for(int i = 0; i < scene->countSelected(); ++i) {
            QQuickItem *item = scene->atSelected(i);
            if(qobject_cast<Node*>(item)) {
                Node *n = qobject_cast<Node*>(item);
                n->setBaseX(n->baseX() - offsetBaseX);
                n->setBaseY(n->baseY() - offsetBaseY);
            }
        }
    }
}

void Node::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && event->modifiers() != Qt::ControlModifier) {
        QVector2D offset(x() - oldX, y() - oldY);

        Scene* scene = reinterpret_cast<Scene*>(parentItem());
        if(!moved && m_selected){
            QList<QQuickItem*> selected = scene->selectedList();
            if(selected.size() > 1) {
                scene->clearSelected();
                setSelected(true);
                scene->addSelected(this);
                scene->selectedItems(selected);
            }
        }
        else {
            QList<Node*> movedNodes;
            for(int i = 0; i < scene->countSelected(); ++i) {
                QQuickItem *item = scene->atSelected(i);
                if(qobject_cast<Node*>(item)) {
                    Node *n = qobject_cast<Node*>(item);
                    movedNodes.append(n);
                }
            }
            scene->movedNodes(movedNodes, offset);
        }
    }
}

void Node::hoverMoveEvent(QHoverEvent *event) {
    if(!grNode->property("hovered").toBool() && event->pos().y() < 207*m_scale) {
        grNode->setProperty("hovered", true);
    }
    else if(grNode->property("hovered").toBool() && event->pos().y() > 207*m_scale) {
        grNode->setProperty("hovered", false);
    }
}

void Node::hoverLeaveEvent(QHoverEvent *event) {
    grNode->setProperty("hovered", false);
}

void Node::serialize(QJsonObject &json) const {
    json["name"] = objectName();
    json["baseX"] = m_baseX;
    json["baseY"] = m_baseY;
}

void Node::deserialize(const QJsonObject &json) {
    if(json.contains("baseX")) {
        setBaseX(json["baseX"].toVariant().toFloat());
    }
    if(json.contains("baseY")) {
        setBaseY(json["baseY"].toVariant().toFloat());
    }    
    operation();
}

void Node::createSockets(int inputCount, int outputCount) {
    m_socketsInput.clear();
    m_socketOutput.clear();
    float inputStart = (grNode->height() - 30*m_scale)/2 + 22*m_scale - 42*m_scale*(inputCount - 1)/2;
    float outputStart = (grNode->height() - 30*m_scale)/2 + 22*m_scale - 42*m_scale*(outputCount - 1)/2;
    for(int i = 0; i < inputCount; ++i) {
        Socket *s = new Socket(this);
        s->setType(INPUTS);
        s->setY(inputStart + 42*m_scale*i);
        s->setX(2*m_scale);
        s->updateScale(m_scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
        m_socketsInput.append(s);
    }
    for(int i = 0; i < outputCount; ++i) {
        Socket *s = new Socket(this);
        s->setType(OUTPUTS);
        s->setTip("Output");
        s->setY(outputStart + 42*m_scale*i);
        s->setX(178*m_scale);
        s->updateScale(m_scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8, s->y() + 8));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
        m_socketOutput.append(s);
    }
}

void Node::createAdditionalInputs(int count) {
    m_additionalInputs.clear();
    float start = grNode->height() + 12*m_scale;
    for(int i = 0; i < count; ++i) {
        Socket *s = new Socket(this);
        s->setType(INPUTS);
        s->setAdditional(true);
        s->setY(start + 28*m_scale*i);
        s->setX(2*m_scale);
        s->updateScale(m_scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*m_scale, s->y() + 8*m_scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
        m_additionalInputs.append(s);
    }
}

void Node::setTitle(QString title) {
    grNode->setProperty("title", title);
}

void Node::setPropertyOnPanel(const char *name, QVariant value) {
    propertiesPanel->setProperty(name, value);
}

void Node::propertyChanged(QString propName, QVariant newValue, QVariant oldValue) {
    Scene* scene = reinterpret_cast<Scene*>(parentItem());
    if(scene) {
        std::string prop = propName.toStdString();
        char *name = new char[prop.size() + 1];
        std::copy(prop.begin(), prop.end(), name);
        name[prop.size()] = '\0';
        scene->nodePropertyChanged(this, name, newValue, oldValue);
    }
}

void Node::operation() {

}

unsigned int &Node::getPreviewTexture() {
    return previewTex;
}

void Node::saveTexture(QString fileName) {

}

void Node::scaleUpdate(float scale) {
    setWidth(196*static_cast<qreal>(scale));
    setHeight(207*static_cast<qreal>(scale));
    grNode->setHeight(207*scale);
    grNode->setProperty("scaleView", scale);    
    int inputCount = m_socketsInput.length();
    int outputCount = m_socketOutput.length();
    float inputStart = (grNode->height() - 30*scale)/2 + 22*scale - 42*scale*(inputCount - 1)/2;
    float outputStart = (grNode->height() - 30*scale)/2 + 22*scale - 42*scale*(outputCount - 1)/2;
    for(int i = 0; i < inputCount; ++i) {
        Socket *s = m_socketsInput[i];
        s->updateScale(scale);
        s->setY(inputStart + 42*scale*i);
        s->setX(2*scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*scale, s->y() + 8*scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(int i = 0; i < outputCount; ++i) {
        Socket *s = m_socketOutput[i];
        s->updateScale(scale);
        s->setY(outputStart + 42*scale*i);
        s->setX(178*scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*scale, s->y() + 8*scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    for(int i = 0; i < m_additionalInputs.length(); ++i) {
        Socket *s = m_additionalInputs[i];
        s->updateScale(scale);
        s->setY(grNode->height() + 12*scale + 28*i*scale);
        s->setX(2*scale);
        QPointF sPos = mapToItem(parentItem(), QPointF(s->x() + 8*scale, s->y() + 8*scale));
        s->setGlobalPos(QVector2D(sPos.x(), sPos.y()));
    }
    m_scale = scale;
    changeScaleView(scale);
}
