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

#include "preview.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>


PreviewObject::PreviewObject(QQuickItem *parent): QQuickFramebufferObject (parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
}

QQuickFramebufferObject::Renderer *PreviewObject::createRenderer() const{
    return new PreviewRenderer();
}

void PreviewObject::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        lastX = event->pos().x();
        lastY = event->pos().y();
    }
}

void PreviewObject::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() == Qt::MidButton) {
       offsetX = (event->pos().x() - lastX);
       offsetY = (event->pos().y() - lastY);
       m_pan.setX(m_pan.x() + offsetX);
       m_pan.setY(m_pan.y() + offsetY);
       lastX = event->pos().x();
       lastY = event->pos().y();
       update();
    }
}

void PreviewObject::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        offsetX = 0.0f;
        offsetY = 0.0f;
    }
}

void PreviewObject::wheelEvent(QWheelEvent *event) {
    if(event->angleDelta().y() > 0) {
       if((m_scale + 0.001f*event->angleDelta().y()) > 2.0f) return;
       scaleStep = 0.001f*event->angleDelta().y();
    }
    else {
        if((m_scale + 0.001f*event->angleDelta().y()) < 0.1f) return;
        scaleStep = 0.001f*event->angleDelta().y();
    }

    m_scale += scaleStep;
    lastX = event->pos().x();
    lastY = event->pos().y();
    m_scale = std::min(std::max(m_scale, 0.1f), 2.0f);
    float x = lastX - (width() - 1024.0f*(m_scale - scaleStep))*0.5f - m_pan.x();
    x = x/(m_scale - scaleStep);
    x = x*m_scale + (width() - 1024.0f*m_scale)*0.5f;
    x = lastX - x;
    m_pan.setX(x);
    float y = lastY - (height() - 1024.0f*(m_scale - scaleStep))*0.5f - m_pan.y();
    y = y/(m_scale - scaleStep);
    y = y*m_scale + (height() - 1024.0f*m_scale)*0.5f;
    y = lastY - y;
    m_pan.setY(y);
    update();
}

float PreviewObject::previewScale() {
    return m_scale;
}

QVector2D PreviewObject::previewPan() {
    return m_pan;
}

bool PreviewObject::pinned() {
    return m_pinned;
}

void PreviewObject::setPinned(bool pin) {
    m_pinned = pin;
    pinnedChanged(pin);
    update();
}

bool PreviewObject::canUpdatePreview() {
    return m_canUpdatePreview;
}

void PreviewObject::setCanUpdatePreview(bool can) {
    m_canUpdatePreview = can;
}

void PreviewObject::setPreviewData(QVariant previewData, bool useTexture) {
    m_data = previewData;
    this->useTexture = useTexture;
    update();
}

void PreviewObject::resetView() {
    m_scale = 0.2f;
    m_pan = QVector2D(0, 0);
}

QVariant &PreviewObject::previewData() {
    return m_data;
}

PreviewRenderer::PreviewRenderer() {
    initializeOpenGLFunctions();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texmatrix.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/albedo.frag");
    textureShader->link();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("useAlbedoTex"), false);
    textureShader->setUniformValue(textureShader->uniformLocation("albedoTex"), 0);
    textureShader->release();

    float vertQuadTex[] = {0.0f, 0.0f, 0.0f, 1.0f,
                    0.0f, 1024.0f, 0.0f, 0.0f,
                    1024.0f, 0.0f, 1.0f, 1.0f,
                    1024.0f, 1024.0f, 1.0f, 0.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

PreviewRenderer::~PreviewRenderer() {
    delete textureShader;
}

QOpenGLFramebufferObject *PreviewRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void PreviewRenderer::synchronize(QQuickFramebufferObject *item) {
    PreviewObject *previewItem = static_cast<PreviewObject*>(item);
    wWidth = previewItem->width();
    wHeight = previewItem->height();

    if(previewItem->canUpdatePreview()) {
        textureShader->bind();
        textureShader->setUniformValue(textureShader->uniformLocation("useAlbedoTex"), previewItem->useTexture);
        textureShader->release();
        if(previewItem->useTexture) {
            if(previewItem->previewData().toUInt() != texture) {
                texture = previewItem->previewData().toUInt();
                previewItem->resetView();
            }

        }
        else {
            if(color != qvariant_cast<QVector3D>(previewItem->previewData())) {
                color = qvariant_cast<QVector3D>(previewItem->previewData());
                previewItem->resetView();
            }
        }
    }
    scale = previewItem->previewScale();
    pan = previewItem->previewPan();
}

void PreviewRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.227f, 0.235f, 0.243f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    QMatrix4x4 matrix;
    matrix.ortho(0, wWidth, 0, wHeight, 0.1f, 100.0f);
    matrix.translate((wWidth - 1024*scale)*0.5f + pan.x(), (wHeight - 1024*scale)*0.5f + pan.y(), -5.0f);
    matrix.scale(scale);
    matrix.translate(0.0f, 0.0f, 0.0f);

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("matrix"), matrix);
    textureShader->setUniformValue(textureShader->uniformLocation("albedoVal"), color);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();

    glBindTexture(GL_TEXTURE_2D, 0);
}
