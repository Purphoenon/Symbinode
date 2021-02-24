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

#ifndef FRAME_H
#define FRAME_H
#include <QQuickItem>
#include <QQuickView>
#include "node.h"

enum resize {LEFT, RIGHT, TOP, BOTTOM, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT, NOT};

class Frame: public QQuickItem
{
    Q_OBJECT
public:
    Frame(QQuickItem *parent = nullptr);
    ~Frame();
    float baseX();
    void setBaseX(float x);
    float baseY();
    void setBaseY(float y);
    float baseWidth();
    void setBaseWidth(float width);
    float baseHeight();
    void setBaseHeight(float height);
    QString title();
    void setTitle(QString title);
    QQuickItem *getPropertyPanel();
    QVector3D color();
    void setColor(QVector3D color);
    QVector2D pan();
    void setPan(QVector2D pan);
    void setScaleView(float scale);
    void resizeByContent();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void hoverEnterEvent(QHoverEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    void addNodes(QList<QQuickItem*> nodes);
    void removeItem(QQuickItem *item);
    bool selected();
    void setSelected(bool sel);
    void setBubbleVisible(bool visible);
    QList<QQuickItem*> contentList() const;
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
signals:
    void nameInput();
    void colorChanged(QVector3D color);
public slots:
    void titleChanged(QString newTitle, QString oldTitle);
    void updateColor(QVector3D color);
    void propertyChanged(QString propName, QVariant newValue, QVariant oldValue);
private:
    QQuickView *m_view = nullptr;
    QQuickItem *m_grFrame = nullptr;
    QQuickItem *m_propertiesPanel = nullptr;
    QQuickView *m_propView = nullptr;
    QList<QQuickItem*> m_content;
    float m_baseX;
    float m_baseY;
    float m_baseWidth = 200;
    float m_baseHeight = 100;
    float m_scale = 1.0f;
    QVector2D m_pan = QVector2D(0, 0);
    float lastX = 0;
    float lastY = 0;
    float m_oldX = 0;
    float m_oldY = 0;
    float m_minWidth = 70.0f;
    float m_minHeight = 50.0f;
    resize currentResize = LEFT;
    bool m_containsNodes = false;
    bool m_selected = false;
    bool m_moved = false;
    QVector3D m_color = QVector3D(0.13f, 0.13f, 0.13f);
};

#endif // FRAME_H
