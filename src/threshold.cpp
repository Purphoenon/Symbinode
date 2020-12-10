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

#include "threshold.h"
#include <QOpenGLFramebufferObjectFormat>

ThresholdObject::ThresholdObject(QQuickItem *parent, QVector2D resolution, float threshold):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_threshold(threshold)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *ThresholdObject::createRenderer() const {
    return new ThresholdRenderer(m_resolution);
}

unsigned int &ThresholdObject::texture() {
    return m_texture;
}

void ThresholdObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int ThresholdObject::maskTexture() {
    return m_maskTexture;
}

void ThresholdObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    created = true;
    update();
}

unsigned int ThresholdObject::sourceTexture() {
    return m_sourceTexture;
}

void ThresholdObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    created = true;
    update();
}

float ThresholdObject::threshold() {
    return m_threshold;
}

void ThresholdObject::setThreshold(float value) {
    m_threshold = value;
    created = true;
    update();
}

QVector2D ThresholdObject::resolution() {
    return m_resolution;
}

void ThresholdObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

ThresholdRenderer::ThresholdRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    thresholdShader = new QOpenGLShaderProgram();
    thresholdShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    thresholdShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/threshold.frag");
    thresholdShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    thresholdShader->bind();
    thresholdShader->setUniformValue(thresholdShader->uniformLocation("sourceTexture"), 0);
    thresholdShader->setUniformValue(thresholdShader->uniformLocation("maskTexture"), 1);
    thresholdShader->release();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();
    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                           -1.0f, 1.0f, 0.0f, 1.0f,
                           1.0f, -1.0f, 1.0f, 0.0f,
                           1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &textureVAO);
    glBindVertexArray(textureVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenFramebuffers(1, &thresholdFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, thresholdFBO);
    glGenTextures(1, &m_thresholdTexture);
    glBindTexture(GL_TEXTURE_2D, m_thresholdTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_thresholdTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ThresholdRenderer::~ThresholdRenderer() {
    delete thresholdShader;
    delete textureShader;
}

QOpenGLFramebufferObject *ThresholdRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void ThresholdRenderer::synchronize(QQuickFramebufferObject *item) {
    ThresholdObject *thresholdItem = static_cast<ThresholdObject*>(item);
    if(thresholdItem->resUpdated) {
        thresholdItem->resUpdated = false;
        m_resolution = thresholdItem->resolution();
        updateTexResolution();
    }
    if(thresholdItem->created) {
        thresholdItem->created = false;
        m_sourceTexture = thresholdItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = thresholdItem->maskTexture();
            thresholdShader->bind();
            thresholdShader->setUniformValue(thresholdShader->uniformLocation("threshold"), thresholdItem->threshold());
            thresholdShader->setUniformValue(thresholdShader->uniformLocation("useMask"), maskTexture);
            thresholdShader->release();
            create();
            if(thresholdItem->selectedItem) thresholdItem->updatePreview(m_thresholdTexture, true);
            thresholdItem->setTexture(m_thresholdTexture);
        }
    }    
}

void ThresholdRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_thresholdTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void ThresholdRenderer::create() {
    glBindFramebuffer(GL_FRAMEBUFFER, thresholdFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    thresholdShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    thresholdShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ThresholdRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_thresholdTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
