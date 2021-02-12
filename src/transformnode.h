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

#ifndef TRANSFORMNODE_H
#define TRANSFORMNODE_H

#include "node.h"
#include "transform.h"

class TransformNode: public Node
{
    Q_OBJECT
public:
    TransformNode(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float transX = 0.0f, float transY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, int angle = 0, bool clamp = false);
    ~TransformNode();
    void operation();
    unsigned int &getPreviewTexture();
    void saveTexture(QString fileName);
    void serialize(QJsonObject &json) const;
    void deserialize(const QJsonObject &json, QHash<QUuid, Socket*> &hash);
    float translationX();
    void setTranslationX(float x);
    float translationY();
    void setTranslationY(float y);
    float scaleX();
    void setScaleX(float x);
    float scaleY();
    void setScaleY(float y);
    int rotation();
    void setRotation(int angle);
    bool clampCoords();
    void setClampCoords(bool clamp);
    void setOutput();
signals:
    void translationXChanged(float x);
    void translationYChanged(float y);
    void scaleXChanged(float x);
    void scaleYChanged(float y);
    void rotationChanged(int angle);
    void clampCoordsChanged(bool clamp);
public slots:
    void updateScale(float scale);
    void previewGenerated();
    void updateTranslationX(qreal x);
    void updateTranslationY(qreal y);
    void updateScaleX(qreal x);
    void updateScaleY(qreal y);
    void updateRotation(int angle);
    void updateClampCoords(bool clamp);
private:
    TransformObject *preview;
    float m_transX = 0.0f;
    float m_transY = 0.0f;
    float m_scaleX = 1.0f;
    float m_scaleY = 1.0f;
    int m_angle = 0;
    bool m_clamp = false;
};

#endif // TRANSFORMNODE_H
