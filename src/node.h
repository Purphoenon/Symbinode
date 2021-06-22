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

#ifndef NODE_H
#define NODE_H
#include <QQuickItem>
#include <QQuickView>
#include <QJsonObject>
#include <QJsonArray>
#include "socket.h"

class Frame;

class Node: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(float baseX READ baseX WRITE setBaseX NOTIFY changeBaseX)
    Q_PROPERTY(float baseY READ baseY WRITE setBaseY NOTIFY changeBaseY)
    Q_PROPERTY(QVector2D pan READ pan WRITE setPan NOTIFY changePan)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY changeSelected)
public:
    Node(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8);
    Node(const Node &node);
    ~Node();
    float baseX();
    void setBaseX(float value);
    float baseY();
    void setBaseY(float value);
    QVector2D pan();
    void setPan(QVector2D pan);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    float scaleView();
    bool selected();
    void setSelected(bool select);
    bool checkConnected(Node* node, socketType type);
    QList<Edge*> getEdges() const;
    QQuickItem *getPropertyPanel();
    Frame *attachedFrame();
    void setAttachedFrame(Frame *frame);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void hoverMoveEvent(QHoverEvent *event);
    void hoverLeaveEvent(QHoverEvent *event);
    virtual void serialize(QJsonObject &json) const;
    virtual void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    void setPropertyOnPanel(const char* name, QVariant value);
    void createSockets(int inputCount, int outputCount);
    void createAdditionalInputs(int count);
    void setTitle(QString title);
    virtual void operation();
    virtual unsigned int &getPreviewTexture();
    virtual void saveTexture(QString fileName);
public slots:
    void scaleUpdate(float scale);
    void bpcUpdate(int bpcType);
    void propertyChanged(QString propName, QVariant newValue, QVariant oldValue);
signals:
    void changeBaseX(float value);
    void changeBaseY(float value);
    void changePan(QVector2D pan);
    void changeResolution(QVector2D res);
    void changeBPC(GLint bpc);
    void changeSelected(bool select);
    void changeScaleView(float scale);
    void updatePreview(unsigned int previewData);
    void dataChanged();
    void generatePreview();
protected:
    QQuickItem *grNode = nullptr;
    QQuickItem *propertiesPanel = nullptr;
    QQuickView *propView = nullptr;
    QVector<Socket *> m_socketsInput;
    QVector<Socket *> m_socketOutput;
    QVector<Socket *> m_additionalInputs;
    QVector2D m_resolution;
    GLint m_bpc;
private:
    QQuickView *view;
    Frame *m_attachedFrame = nullptr;
    float m_baseX = 0;
    float m_baseY = 0;
    float m_scale = 1.0f;
    QVector2D m_pan = QVector2D(0, 0);
    bool m_selected = false;
    float dragX = 0.0f;
    float dragY = 0.0f;
    float oldX;
    float oldY;
    bool moved = false;
    unsigned int previewTex = 0;
};


#endif // NODE_H
