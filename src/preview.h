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

#ifndef PREVIEW_H
#define PREVIEW_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class PreviewObject: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(bool pinned READ pinned WRITE setPinned NOTIFY pinnedChanged)
    Q_PROPERTY(bool canUpdatePreview READ canUpdatePreview WRITE setCanUpdatePreview)
public:
    Q_INVOKABLE void setPreviewData(QVariant previewData, bool useTexture);
    Q_INVOKABLE void resetView();
    QVariant &previewData();
    PreviewObject(QQuickItem *parent = nullptr);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    float previewScale();
    QVector2D previewPan();
    bool pinned();
    void setPinned(bool pin);
    bool canUpdatePreview();
    void setCanUpdatePreview(bool can);
    bool useTexture = false;
signals:
    void pinnedChanged(bool pin);
private:
    QVariant m_data = QVector3D(0.227f, 0.235f, 0.243f);
    float m_scale = 0.2f;
    float offsetX = 0, offsetY = 0;
    float lastX, lastY;
    float scaleStep = 0.0f;
    QVector2D m_pan = QVector2D(0.0f, 0.0f);
    bool m_pinned = false;
    bool m_canUpdatePreview = true;
};

class PreviewRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    PreviewRenderer();
    ~PreviewRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    int wWidth, wHeight;
    unsigned int texture = 0;
    unsigned int checkerTexture = 0;
    QVector3D color = QVector3D(0.227f, 0.235f, 0.243f);
    bool useTexture = false;
    unsigned int VAO;
    unsigned int checkerVAO;
    float scale = 1.0f;
    QVector2D pan = QVector2D(0.0f, 0.0f);
    QOpenGLShaderProgram *textureShader = nullptr;
    QOpenGLShaderProgram *checkerShader;
};

#endif // PREVIEW_H
