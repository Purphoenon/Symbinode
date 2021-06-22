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
    setAcceptHoverEvents(true);
}

QQuickFramebufferObject::Renderer *PreviewObject::createRenderer() const{
    return new PreviewRenderer();
}

void PreviewObject::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton || event->button() == Qt::RightButton) {
        lastX = event->pos().x();
        lastY = event->pos().y();
        startX = lastX;
        startY = lastY;
    }
    else {
        event->setAccepted(false);
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
    else if(event->buttons() == Qt::RightButton && event->modifiers() == Qt::AltModifier) {
        if(event->pos().x() > 0) {
           if((m_scale + 0.01f*(event->pos().x() - lastX)) > 2.0f) scaleStep = 2.0f - m_scale;
           else scaleStep = 0.01f*(event->pos().x() - lastX);
        }
        else {
            if((m_scale + 0.01f*(event->pos().x() - lastX)) < 0.1f) scaleStep = 0.1f - m_scale;
            else scaleStep = 0.01f*(event->pos().x() - lastX);
        }

        if(scaleStep != 0.0f) {
            float oldScale = m_scale;
            m_scale += scaleStep;
            lastX = event->pos().x();
            lastY = event->pos().y();
            m_scale = std::min(std::max(m_scale, 0.1f), 2.0f);
            float x = startX - (width() - 1024.0f*(oldScale))*0.5f - m_pan.x();
            x = x/(oldScale);
            x = x*m_scale + (width() - 1024.0f*m_scale)*0.5f;
            x = startX - x;
            m_pan.setX(x);
            float y = startY - (height() - 1024.0f*(oldScale))*0.5f - m_pan.y();
            y = y/(oldScale);
            y = y*m_scale + (height() - 1024.0f*m_scale)*0.5f;
            y = startY - y;
            m_pan.setY(y);
            update();
        }

    }
}

void PreviewObject::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        offsetX = 0.0f;
        offsetY = 0.0f;
    }
}

void PreviewObject::hoverEnterEvent(QHoverEvent *event) {
    setFocus(true);
}

void PreviewObject::hoverLeaveEvent(QHoverEvent *event) {
    setFocus(false);
}

void PreviewObject::wheelEvent(QWheelEvent *event) {
    if(event->angleDelta().y() > 0) {
       if((m_scale + 0.001f*event->angleDelta().y()) > 2.0f) scaleStep = 2.0f - m_scale;
       else scaleStep = 0.001f*event->angleDelta().y();
    }
    else {
        if((m_scale + 0.001f*event->angleDelta().y()) < 0.1f) scaleStep = 0.1f - m_scale;
        else scaleStep = 0.001f*event->angleDelta().y();
    }

    if(scaleStep != 0.0f) {
        float oldScale = m_scale;
        m_scale += scaleStep;
        lastX = event->pos().x();
        lastY = event->pos().y();
        m_scale = std::min(std::max(m_scale, 0.1f), 2.0f);
        float x = lastX - (width() - 1024.0f*(oldScale))*0.5f - m_pan.x();
        x = x/(oldScale);
        x = x*m_scale + (width() - 1024.0f*m_scale)*0.5f;
        x = lastX - x;
        m_pan.setX(x);
        float y = lastY - (height() - 1024.0f*(oldScale))*0.5f - m_pan.y();
        y = y/(oldScale);
        y = y*m_scale + (height() - 1024.0f*m_scale)*0.5f;
        y = lastY - y;
        m_pan.setY(y);
        update();
    }
}

void PreviewObject::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_F) {
        resetView();
        update();
    }
}

float PreviewObject::previewScale() {
    return m_scale;
}

QVector2D PreviewObject::previewPan() {
    return m_pan;
}

void PreviewObject::setPreviewData(unsigned int previewData) {
    m_previewTexture = previewData;
    update();
}

void PreviewObject::resetView() {
    m_scale = 0.2f;
    m_pan = QVector2D(0, 0);
}

unsigned int &PreviewObject::previewData() {
    return m_previewTexture;
}

PreviewRenderer::PreviewRenderer() {
    initializeOpenGLFunctions();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texmatrix.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();

    float vertQuadTex[] = {0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1024.0f, 0.0f, 1.0f,
                    1024.0f, 0.0f, 1.0f, 0.0f,
                    1024.0f, 1024.0f, 1.0f, 1.0f};
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

    float vertQuad[] = {-1.0f, -1.0f,
                        -1.0f, 1.0f,
                        1.0f, -1.0f,
                        1.0f, 1.0f};

    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuad), vertQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertChecker[] = {0.0f, 0.0f, 0.0f, 1.0f,
                           0.0f, 1024.0f, 0.0f, 0.0f,
                           1024.0f, 0.0f, 1.0f, 1.0f,
                           1024.0f, 1024.0f, 1.0f, 0.0f};

    unsigned int checkerVBO;
    glGenVertexArrays(1, &checkerVAO);
    glBindVertexArray(checkerVAO);
    glGenBuffers(1, &checkerVBO);
    glBindBuffer(GL_ARRAY_BUFFER, checkerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertChecker), vertChecker, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int checkerFBO;
    glGenFramebuffers(1, &checkerFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, checkerFBO);
    glGenTextures(1, &checkerTexture);
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, checkerTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, checkerFBO);
    glViewport(0, 0, 32, 32);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    checkerShader->bind();
    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, checkerTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);    
    glDeleteFramebuffers(1, &checkerFBO);
    glDeleteVertexArrays(1, &VAO2);
}

PreviewRenderer::~PreviewRenderer() {
    delete textureShader;
    delete checkerShader;
    glDeleteTextures(1, &checkerTexture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &checkerVAO);
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
    if(previewItem->previewData() != texture) {
        texture = previewItem->previewData();
        previewItem->resetView();
    }
    scale = previewItem->previewScale();
    pan = previewItem->previewPan();
}

void PreviewRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.227f, 0.235f, 0.243f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    QMatrix4x4 matrix;
    matrix.ortho(0, wWidth, 0, wHeight, 0.1f, 100.0f);
    matrix.translate((wWidth - 1024*scale)*0.5f + pan.x(), (wHeight - 1024*scale)*0.5f + pan.y(), -5.0f);
    matrix.scale(scale);
    matrix.translate(0.0f, 0.0f, 0.0f);

    if(texture) {
        textureShader->bind();
        textureShader->setUniformValue(textureShader->uniformLocation("matrix"), matrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, checkerTexture);
        glBindVertexArray(checkerVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();

        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
