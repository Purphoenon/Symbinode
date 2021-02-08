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
    setWidth(200);
    setHeight(100);
    m_view = new QQuickView();
    m_view->setSource(QUrl(QStringLiteral("qrc:/qml/NodeFrame.qml")));
    m_grFrame = qobject_cast<QQuickItem *>(m_view->rootObject());
    m_grFrame->setParentItem(this);
    connect(this, SIGNAL(nameInput()), m_grFrame, SLOT(createNameInput()));
    connect(m_grFrame, SIGNAL(titleChanged(QString, QString)), this, SLOT(titleChanged(QString, QString)));
}

Frame::~Frame() {
    delete m_grFrame;
    delete m_view;
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

float Frame::baseWidth() {
    return m_baseWidth;
}

void Frame::setBaseWidth(float width) {
    m_baseWidth = width;
    setWidth(m_baseWidth*m_scale);
}

float Frame::baseHeight() {
    return m_baseHeight;
}

void Frame::setBaseHeight(float height) {
    m_baseHeight = height;
    setHeight(m_baseHeight*m_scale);
}

QString Frame::title() {
    return m_grFrame->property("frameName").toString();
}

void Frame::setTitle(QString title) {
    m_grFrame->setProperty("frameName", title);
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
    setWidth(m_baseWidth*scale);
    setHeight(m_baseHeight*scale);
    m_grFrame->setProperty("scaleView", scale);
}

void Frame::resizeByContent() {
    double minX = std::numeric_limits<double>::max();
    double minY = minX;
    double maxX = std::numeric_limits<double>::min();
    double maxY = maxX;
    for(QQuickItem *item: m_content) {
        minX = std::min(minX, item->x());
        minY = std::min(minY, item->y());
        maxX = std::max(maxX, item->x() + item->width());
        maxY = std::max(maxY, item->y() + item->height());
    }
    setX(minX - 10*m_scale);
    setY(minY - 35*m_scale);
    m_baseX = (x() + m_pan.x())/m_scale;
    m_baseY = (y() + m_pan.y())/m_scale;
    setWidth(maxX - minX + 20*m_scale);
    setHeight(maxY - minY + 55*m_scale);
    m_baseWidth = width()/m_scale;
    m_baseHeight = height()/m_scale;
}

void Frame::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        setFocus(true);
        setFocus(false);
        lastX = event->pos().x();
        lastY = event->pos().y();
        m_oldX = m_baseX;
        m_oldY = m_baseY;
        m_moved = false;
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
            if(width() - offsetX > m_minWidth*m_scale) {
                /*setX(point.x());
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (point.x() - m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            else {
                offsetX = width() - m_minWidth*m_scale;
                /*setX(x() + offsetX);
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case RIGHT:
            if(offsetX < m_minWidth*m_scale) offsetX = m_minWidth*m_scale;
            /*setWidth(offsetX);
            m_baseWidth = width()/m_scale;*/
            offW = offsetX/m_scale - m_baseWidth;
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOP:
            if(height() - offsetY > m_minHeight*m_scale) {
                /*setY(point.y());
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            else {
                offsetY = height() - m_minHeight*m_scale;
                /*setY(y() + offsetY);
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOM:
            if(offsetY < m_minHeight*m_scale) offsetY = m_minHeight*m_scale;
            /*setHeight(offsetY);
            m_baseHeight = height()/m_scale;*/
            offH = offsetY/m_scale - m_baseHeight;
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOPLEFT:
            if(width() - offsetX > m_minWidth*m_scale) {
                /*setX(point.x());
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (point.x() + m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            else {
                offsetX = width() - m_minWidth*m_scale;
                /*setX(x() + offsetX);
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            if(height() - offsetY > m_minHeight*m_scale) {
                /*setY(point.y());
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            else {
                offsetY = height() - m_minHeight*m_scale;
                /*setY(y() + offsetY);
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case TOPRIGHT:
            if(offsetX < m_minWidth*m_scale) offsetX = m_minWidth*m_scale;
            /*setWidth(offsetX);
            m_baseWidth = width()/m_scale;*/
            offW = offsetX/m_scale - m_baseWidth;
            if(height() - offsetY > m_minHeight*m_scale) {
                /*setY(point.y());
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (point.y() + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            else {
                offsetY = height() - m_minHeight*m_scale;
                /*setY(y() + offsetY);
                m_baseY = (y() + m_pan.y())/m_scale;
                setHeight(height() - offsetY);
                m_baseHeight = height()/m_scale;*/
                offY = (y() + offsetY + m_pan.y())/m_scale - m_baseY;
                offH = (height() - offsetY)/m_scale - m_baseHeight;
            }
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOMLEFT:
            if(width() - offsetX > m_minWidth*m_scale) {
                /*setX(point.x());
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (point.x() + m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            else {
                offsetX = width() - m_minWidth*m_scale;
                /*setX(x() + offsetX);
                m_baseX = (x() + m_pan.x())/m_scale;
                setWidth(width() - offsetX);
                m_baseWidth = width()/m_scale;*/
                offX = (x() + offsetX + m_pan.x())/m_scale - m_baseX;
                offW = (width() - offsetX)/m_scale - m_baseWidth;
            }
            if(offsetY < m_minHeight*m_scale) offsetY = m_minHeight*m_scale;
            /*setHeight(offsetY);
            m_baseHeight = height()/m_scale;*/
            offH = offsetY/m_scale - m_baseHeight;
            scene->resizedFrame(this, offX, offY, offW, offH);
            break;
        case BOTTOMRIGHT:
            if(offsetX < m_minWidth*m_scale) offsetX = m_minWidth*m_scale;
            /*setWidth(offsetX);
            m_baseWidth = width()/m_scale;*/
            offW = offsetX/m_scale - m_baseWidth;
            if(offsetY < m_minHeight*m_scale) offsetY = m_minHeight*m_scale;
            /*setHeight(offsetY);
            m_baseHeight = height()/m_scale;*/
            offH = offsetY/m_scale - m_baseHeight;
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
    m_grFrame->setProperty("hovered", true);
}

void Frame::hoverMoveEvent(QHoverEvent *event) {
    if(m_content.size() > 0) {
        currentResize = NOT;
        setCursor(QCursor(Qt::ArrowCursor));
        return;
    }
    float mouseAreaSize = 3.0f;
    if(event->pos().x() < mouseAreaSize && (event->pos().y() > mouseAreaSize && event->pos().y() < height() - mouseAreaSize)) {
        currentResize = LEFT;
        setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize && (event->pos().y() > mouseAreaSize && event->pos().y() < height() - mouseAreaSize)) {
        currentResize = RIGHT;
        setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if((event->pos().x() > mouseAreaSize && event->pos().x() < width() - mouseAreaSize) && event->pos().y() < mouseAreaSize) {
        currentResize = TOP;
        setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if((event->pos().x() > mouseAreaSize && event->pos().x() < width() - mouseAreaSize) && event->pos().y() > height() - mouseAreaSize) {
        currentResize = BOTTOM;
        setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(event->pos().x() < mouseAreaSize && event->pos().y() < mouseAreaSize) {
        currentResize = TOPLEFT;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize && event->pos().y() < mouseAreaSize) {
        currentResize = TOPRIGHT;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(event->pos().x() < mouseAreaSize && event->pos().y() > height() - mouseAreaSize) {
        currentResize = BOTTOMLEFT;
        setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(event->pos().x() > width() - mouseAreaSize && event->pos().y() > height() - mouseAreaSize) {
        currentResize = BOTTOMRIGHT;
        setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else {
        currentResize = NOT;
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void Frame::hoverLeaveEvent(QHoverEvent *event) {
    m_grFrame->setProperty("hovered", false);
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
    if(qobject_cast<Node*>(item)) {
        Node *node = qobject_cast<Node*>(item);
        node->setAttachedFrame(nullptr);
    }
    if(m_content.size() > 0) resizeByContent();
}

bool Frame::selected() {
    return m_selected;
}

void Frame::setSelected(bool sel) {
    m_selected = sel;
    m_grFrame->setProperty("selected", sel);
}

QList<QQuickItem*> Frame::contentList() const {
    return m_content;
}

void Frame::serialize(QJsonObject &json) const {
    json["title"] = m_grFrame->property("frameName").toString();
    json["baseX"] = m_baseX;
    json["baseY"] = m_baseY;
    json["baseWidth"] = m_baseWidth;
    json["baseHeight"] = m_baseHeight;
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

void Frame::deserialize(const QJsonObject &json) {
    if(json.contains("baseX")) {
        setBaseX(json["baseX"].toVariant().toFloat());
    }
    if(json.contains("baseY")) {
        setBaseY(json["baseY"].toVariant().toFloat());
    }
    if(json.contains("baseWidth")) {
        m_baseWidth = json["baseWidth"].toVariant().toFloat();
        setWidth(m_baseWidth*m_scale);
    }
    if(json.contains("baseHeight")) {
        m_baseHeight = json["baseHeight"].toVariant().toFloat();
        setHeight(m_baseHeight*m_scale);
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
                int nodeType = nodesObject["type"].toInt();
                Node *node = nullptr;
                switch (nodeType) {
                case 0:
                    node = new ColorRampNode(scene, scene->resolution());
                    break;
                case 1:
                    node = new ColorNode(scene, scene->resolution());
                    break;
                case 2:
                    node = new ColoringNode(scene, scene->resolution());
                    break;
                case 3:
                    node = new MappingNode(scene, scene->resolution());
                    break;
                case 5:
                    node = new MirrorNode(scene, scene->resolution());
                    break;
                case 6:
                    node = new NoiseNode(scene, scene->resolution());
                    break;
                case 7:
                    node = new MixNode(scene, scene->resolution());
                    break;
                case 8:
                    node = new AlbedoNode(scene, scene->resolution());
                    break;
                case 9:
                    node = new MetalNode(scene, scene->resolution());
                    break;
                case 10:
                    node = new RoughNode(scene, scene->resolution());
                    break;
                case 11:
                    node = new NormalMapNode(scene, scene->resolution());
                    break;
                case 12:
                    node = new NormalNode(scene, scene->resolution());
                    break;
                case 13:
                    node = new VoronoiNode(scene, scene->resolution());
                    break;
                case 14:
                    node = new PolygonNode(scene, scene->resolution());
                    break;
                case 15:
                    node = new CircleNode(scene, scene->resolution());
                    break;
                case 16:
                    node = new TransformNode(scene, scene->resolution());
                    break;
                case 17:
                    node = new TileNode(scene, scene->resolution());
                    break;
                case 18:
                    node = new WarpNode(scene, scene->resolution());
                    break;
                case 19:
                    node = new BlurNode(scene, scene->resolution());
                    break;
                case 20:
                    node = new InverseNode(scene, scene->resolution());
                    break;
                case 21:
                    node = new BrightnessContrastNode(scene, scene->resolution());
                    break;
                case 22:
                    node = new ThresholdNode(scene, scene->resolution());
                    break;
                default:
                    std::cout << "nonexistent type" << std::endl;
                }
                if(node) {                    
                    scene->addNode(node);
                    node->deserialize(nodesObject);
                    m_content.push_back(node);
                    node->setAttachedFrame(this);
                }
            }
        }
    }
}

void Frame::titleChanged(QString newTitle, QString oldTitle) {
    Scene *scene = qobject_cast<Scene*>(parentItem());
    scene->changedTitle(this, newTitle, oldTitle);
}
