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

#include "frame.h"
#include "scene.h"
#include <iostream>
#include "albedonode.h"
#include "metalnode.h"
#include "roughnode.h"
#include "noisenode.h"
#include "mixnode.h"
#include "normalmapnode.h"
#include "normalnode.h"
#include "voronoinode.h"
#include "polygonnode.h"
#include "circlenode.h"
#include "transformnode.h"
#include "tilenode.h"
#include "warpnode.h"
#include "blurnode.h"
#include "inversenode.h"
#include "colorrampnode.h"
#include "colornode.h"
#include "coloringnode.h"
#include "mappingnode.h"
#include "mirrornode.h"
#include "brightnesscontrastnode.h"
#include "thresholdnode.h"

Frame::Frame(QQuickItem *parent): QQuickItem (parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsDrops, true);
    setTransformOrigin(TopLeft);
    setWidth(200);
    setHeight(100);
    m_view = new QQuickView();
    m_view->setSource(QUrl(QStringLiteral("qrc:/qml/NodeFrame.qml")));
    m_grFrame = qobject_cast<QQuickItem *>(m_view->rootObject());
    m_grFrame->setParentItem(this);    
    connect(this, SIGNAL(nameInput()), m_grFrame, SLOT(createNameInput()));
    connect(m_grFrame, SIGNAL(titleChanged(QString, QString)), this, SLOT(titleChanged(QString, QString)));
    m_propView = new QQuickView();
    m_propView->setSource(QUrl(QStringLiteral("qrc:/qml/ColorProperty.qml")));
    m_propertiesPanel = qobject_cast<QQuickItem*>(m_propView->rootObject());
    connect(m_propertiesPanel, SIGNAL(colorChanged(QVector3D)), this, SLOT(updateColor(QVector3D)));
    connect(m_propertiesPanel, SIGNAL(propertyChangingFinished(QString, QVariant, QVariant)), this, SLOT(propertyChanged(QString, QVariant, QVariant)));
    m_propertiesPanel->setProperty("startColor", m_color);
}

Frame::~Frame() {
    delete m_grFrame;
    delete m_view;
    delete m_propertiesPanel;
    delete m_propView;
}

float Frame::baseX() {
    return m_baseX;
}

void Frame::setBaseX(float x) {
    float offsetBaseX = x - m_baseX;
    for(QQuickItem *item: m_content) {
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            if(n->selected()) continue;
            n->setBaseX(n->baseX() + offsetBaseX);
        }
        /*else if(qobject_cast<Frame*>(item)) {
            Frame *f = qobject_cast<Frame*>(item);
            if(f->selected()) continue;
            f->setBaseX(f->baseX() + offsetBaseX);
        }*/
    }
    m_baseX = x;
    setX(x*m_scale - m_pan.x());
}

float Frame::baseY() {
    return m_baseY;
}

void Frame::setBaseY(float y) {
    float offsetBaseY = y - m_baseY;
    for(QQuickItem *item: m_content) {
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            if(n->selected()) continue;
            n->setBaseY(n->baseY() + offsetBaseY);
        }
        /*else if(qobject_cast<Frame*>(item)) {
            Frame *f = qobject_cast<Frame*>(item);
            if(f->selected()) continue;
            f->setBaseY(f->baseY() + offsetBaseY);
        }*/
    }
    m_baseY = y;
    setY(y*m_scale - m_pan.y());
}

QString Frame::title() {
    return m_grFrame->property("frameName").toString();
}

void Frame::setTitle(QString title) {
    m_grFrame->setProperty("frameName", title);
}

QQuickItem *Frame::getPropertyPanel() {
    return m_propertiesPanel;
}

QVector3D Frame::color() {
    return m_color;
}

void Frame::setColor(QVector3D color) {
    m_color = color;
    m_grFrame->setProperty("startColor", m_color);
}

QVector2D Frame::pan() {
    return m_pan;
}

void Frame::setPan(QVector2D pan) {
    m_pan = pan;
    setX(m_baseX*m_scale - pan.x());
    setY(m_baseY*m_scale - pan.y());
}

void Frame::setScaleView(float scale) {
    m_scale = scale;
    setScale(scale);
    m_grFrame->setProperty("scaleView", scale);
}

void Frame::resizeByContent() {
    double minX = std::numeric_limits<double>::max();
    double minY = minX;
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = maxX;
    for(QQuickItem *item: m_content) {
        if(qobject_cast<Node*>(item)) {
            Node *node = qobject_cast<Node*>(item);
            minX = std::min(minX, static_cast<double>(node->baseX()));
            minY = std::min(minY, static_cast<double>(node->baseY()));
            maxX = std::max(maxX, static_cast<double>(node->baseX()) + node->width());
            maxY = std::max(maxY, static_cast<double>(node->baseY()) + node->height());
        }
    }
    setX((minX - 10)*m_scale - m_pan.x());
    setY((minY - 55)*m_scale - m_pan.y());
    m_baseX = minX - 10;
    m_baseY = minY - 55;
    setWidth(maxX - minX + 20);
    setHeight(maxY - minY + 75);
}

void Frame::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        setFocus(true);
        setFocus(false);
        lastX = event->pos().x()*m_scale;
        lastY = event->pos().y()*m_scale;
        m_oldX = m_baseX;
        m_oldY = m_baseY;
        m_moved = false;
        if(event->pos().y() < 45.0f && currentResize == resize::NOT) {
            Scene *scene = qobject_cast<Scene*>(parentItem());
            if(event->modifiers() == Qt::ControlModifier) {
               setSelected(!m_selected);
               QList<QQuickItem*> selectedList = scene->selectedList();
               if(m_selected) {
                   scene->addSelected(this);
               }
               else {
                   scene->deleteSelected(this);
               }
               scene->selectedItems(selectedList);
            }
            else if(!m_selected) {
                QList<QQuickItem*> selectedList = scene->selectedList();
                scene->clearSelected();
                scene->addSelected(this);
                setSelected(true);
                scene->selectedItems(selectedList);
            }
        }
        else if(event->pos().y() > 45.0f && currentResize == resize::NOT) {
            event->setAccepted(false);
        }
    }
    else {
        event->setAccepted(false);
    }
}

void Frame::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        QPointF point = mapToItem(parentItem(), QPointF(event->pos().x(), event->pos().y()));
        float offsetX = point.x() - x();
        float offsetY = point.y() - y();
        Scene *scene = qobject_cast<Scene*>(parentItem());
        float offX = 0;
        float offY = 0;
        float offW = 0;
        float offH = 0;
        switch (currentResize) {
        case LEFT:
            if(width() - offsetX/m_scale > m_minWidth) {
                offX = (point.x() + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            else {
                offsetX = (width() - m_minWidth)*m_scale;
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case RIGHT:
            if(offsetX/m_scale < m_minWidth) offsetX = m_minWidth*m_scale;
            offW = (offsetX/m_scale - width());
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOP:
            if(height() - offsetY/m_scale > m_minHeight) {
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            else {
                offsetY = (height() - m_minHeight)*m_scale;
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOM:
            if(offsetY/m_scale < m_minHeight) offsetY = m_minHeight*m_scale;
            offH = offsetY/m_scale - height();
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOPLEFT:
            if(width() - offsetX/m_scale > m_minWidth) {
                offX = (point.x() + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            else {
                offsetX = (width() - m_minWidth)*m_scale;
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            if(height() - offsetY/m_scale > m_minHeight) {
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            else {
                offsetY = (height() - m_minHeight)*m_scale;
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOPRIGHT:
            if(offsetX/m_scale < m_minWidth) offsetX = m_minWidth*m_scale;
            offW = offsetX/m_scale - width();
            if(height() - offsetY/m_scale > m_minHeight) {
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            else {
                offsetY = (height() - m_minHeight)*m_scale;
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = -offsetY/m_scale;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOMLEFT:
            if(width() - offsetX/m_scale > m_minWidth) {
                offX = (point.x() + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            else {
                offsetX = (width() - m_minWidth)*m_scale;
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = -offsetX/m_scale;
            }
            if(offsetY/m_scale < m_minHeight) offsetY = m_minHeight*m_scale;
            offH = offsetY/m_scale - height();
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOMRIGHT:
            if(offsetX/m_scale < m_minWidth) offsetX = m_minWidth*m_scale;
            offW = offsetX/m_scale - width();
            if(offsetY/m_scale < m_minHeight) offsetY = m_minHeight*m_scale;
            offH = offsetY/m_scale - height();
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case NOT:
            m_moved = true;
            Scene *scene = qobject_cast<Scene*>(parentItem());
            setX(point.x() - lastX);
            float offsetBaseX = (x() + m_pan.x())/m_scale - m_baseX;
            setY(point.y() - lastY);
            float offsetBaseY = (y() + m_pan.y())/m_scale - m_baseY;
            for(int i = 0; i < scene->countSelected(); ++i) {
                QQuickItem *item = scene->atSelected(i);
                if(qobject_cast<Node*>(item)) {
                    Node *n = qobject_cast<Node*>(item);
                    n->setBaseX(n->baseX() + offsetBaseX);
                    n->setBaseY(n->baseY() + offsetBaseY);
                }
                else if(qobject_cast<Frame*>(item)) {
                    Frame *f = qobject_cast<Frame*>(item);
                    f->setBaseX(f->baseX() + offsetBaseX);
                    f->setBaseY(f->baseY() + offsetBaseY);
                }
            }
        }
    }
}

void Frame::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        Scene *scene = qobject_cast<Scene*>(parentItem());
        if(m_selected && !m_moved) {            
            if(scene->countSelected() == 1) return;
            QList<QQuickItem*> selectedList = scene->selectedList();
            scene->clearSelected();
            setSelected(true);
            scene->addSelected(this);
            scene->selectedItems(selectedList);
        }
        else {
            scene->movedNodes(scene->selectedList(), QVector2D(m_baseX - m_oldX, m_baseY - m_oldY));
        }
    }
}

void Frame::mouseDoubleClickEvent(QMouseEvent *event) {
    int eventX = event->pos().x();
    int eventY = event->pos().y();
    if(eventX >= 5*m_scale && eventX <= (width() - 5*m_scale) && eventY >= 10*m_scale && eventY <= 35*m_scale){
        nameInput();
    }
}

void Frame::hoverEnterEvent(QHoverEvent *event) {
    setBubbleVisible(true);
}

void Frame::hoverMoveEvent(QHoverEvent *event) {
    if(event->pos().y() < 45.0f) {
        m_grFrame->setProperty("hovered", true);
    }
    else {
        m_grFrame->setProperty("hovered", false);
    }

    if(m_content.size() > 0) {
        currentResize = NOT;
        setCursor(QCursor(Qt::ArrowCursor));
        return;
    }
    float mouseAreaSize = 3.0f;
    if(event->pos().x() < mouseAreaSize/m_scale && (event->pos().y() > mouseAreaSize/m_scale && event->pos().y() < height() - mouseAreaSize/m_scale)) {
        currentResize = LEFT;
        window()->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize/m_scale && (event->pos().y() > mouseAreaSize/m_scale && event->pos().y() < height() - mouseAreaSize/m_scale)) {
        currentResize = RIGHT;
        window()->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if((event->pos().x() > mouseAreaSize/m_scale && event->pos().x() < width() - mouseAreaSize/m_scale) && event->pos().y() < mouseAreaSize/m_scale) {
        currentResize = TOP;
        window()->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if((event->pos().x() > mouseAreaSize/m_scale && event->pos().x() < width() - mouseAreaSize/m_scale) && event->pos().y() > height() - mouseAreaSize/m_scale) {
        currentResize = BOTTOM;
        window()->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(event->pos().x() < mouseAreaSize/m_scale && event->pos().y() < mouseAreaSize/m_scale) {
        currentResize = TOPLEFT;
        window()->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize/m_scale && event->pos().y() < mouseAreaSize/m_scale) {
        currentResize = TOPRIGHT;
        window()->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(event->pos().x() < mouseAreaSize/m_scale && event->pos().y() > height() - mouseAreaSize/m_scale) {
        currentResize = BOTTOMLEFT;
        window()->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize/m_scale && event->pos().y() > height() - mouseAreaSize/m_scale) {
        currentResize = BOTTOMRIGHT;
        window()->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else {
        currentResize = NOT;
        window()->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void Frame::hoverLeaveEvent(QHoverEvent *event) {
    m_grFrame->setProperty("hovered", false);
    window()->setCursor(QCursor(Qt::ArrowCursor));
    setBubbleVisible(false);
}

void Frame::addNodes(QList<QQuickItem *> nodes) {
    for(QQuickItem *n: nodes) {
        if(m_content.contains(n)) continue;
        m_content.push_back(n);        
        if(qobject_cast<Node*>(n)) {
            Node *node = qobject_cast<Node*>(n);
            node->setAttachedFrame(this);
        }
    }
    resizeByContent();
}

void Frame::removeItem(QQuickItem *item) {
    m_content.removeOne(item);
    if(m_content.size() > 0) resizeByContent();
}

bool Frame::selected() {
    return m_selected;
}

void Frame::setSelected(bool sel) {
    m_selected = sel;
    m_grFrame->setProperty("selected", sel);
}

void Frame::setBubbleVisible(bool visible) {
    m_grFrame->setProperty("bubbleVisible", visible);
}

QList<QQuickItem*> Frame::contentList() const {
    return m_content;
}

void Frame::serialize(QJsonObject &json) const {
    QJsonArray color;
    color.append(m_color.x());
    color.append(m_color.y());
    color.append(m_color.z());
    json["color"] = color;
    json["title"] = m_grFrame->property("frameName").toString();
    json["baseX"] = m_baseX;
    json["baseY"] = m_baseY;
    json["width"] = width();
    json["height"] = height();
    QJsonArray nodesArray;
    for(auto item: m_content) {
        if(qobject_cast<Node*>(item)) {
            Node *n = qobject_cast<Node*>(item);
            QJsonObject nodeObject;
            n->serialize(nodeObject);
            nodesArray.append(nodeObject);
        }
    }
    json["nodes"] = nodesArray;
}

void Frame::deserialize(const QJsonObject &json, QHash<QUuid, Socket *> &hash) {
    if(json.contains("color")) {
        QJsonArray color = json["color"].toVariant().toJsonArray();
        QVector3D colorValue = QVector3D(color[0].toVariant().toFloat(), color[1].toVariant().toFloat(), color[2].toVariant().toFloat());
        updateColor(colorValue);
        m_propertiesPanel->setProperty("startColor", colorValue);
    }
    if(json.contains("baseX")) {
        setBaseX(json["baseX"].toVariant().toFloat());
    }
    if(json.contains("baseY")) {
        setBaseY(json["baseY"].toVariant().toFloat());
    }
    if(json.contains("width")) {
        setWidth(json["width"].toVariant().toFloat());
    }
    if(json.contains("height")) {
        setHeight(json["height"].toVariant().toFloat());
    }
    if(json.contains("title")) {
        m_grFrame->setProperty("frameName", json["title"].toString());
    }
    if(json.contains("nodes") && json["nodes"].isArray()) {
        m_content.clear();
        Scene *scene = qobject_cast<Scene*>(parentItem());
        QJsonArray nodes = json["nodes"].toArray();
        for(int i = 0; i < nodes.size(); ++i) {
            QJsonObject nodesObject = nodes[i].toObject();
            if(nodesObject.contains("type")) {
                Node *node = scene->deserializeNode(nodesObject);
                if(node) {
                    scene->addNode(node);
                    node->deserialize(nodesObject, hash);
                    m_content.push_back(node);
                    node->setAttachedFrame(this);
                }
            }
        }
    }
    resizeByContent();
}

void Frame::titleChanged(QString newTitle, QString oldTitle) {
    Scene *scene = qobject_cast<Scene*>(parentItem());
    scene->changedTitle(this, newTitle, oldTitle);
}

void Frame::updateColor(QVector3D color) {
    setColor(color);
}
void Frame::propertyChanged(QString propName, QVariant newValue, QVariant oldValue) {
    Scene* scene = reinterpret_cast<Scene*>(parentItem());
    if(scene) {
        std::string prop = propName.toStdString();
        char *name = new char[prop.size() + 1];
        std::copy(prop.begin(), prop.end(), name);
        name[prop.size()] = '\0';
        scene->itemPropertyChanged(this, name, newValue, oldValue);
    }
}
