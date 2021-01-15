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

#include "blur.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>

BlurObject::BlurObject(QQuickItem *parent, QVector2D resolution, float intensity):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_intensity(intensity)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *BlurObject::createRenderer() const {
    return new BlurRenderer(m_resolution);
}

unsigned int &BlurObject::texture() {
    return m_texture;
}

void BlurObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int BlurObject::maskTexture() {
    return m_maskTexture;
}

void BlurObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    bluredTex = true;
    update();
}

unsigned int BlurObject::sourceTexture() {
    return m_sourceTexture;
}

void BlurObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    bluredTex = true;
    update();
}

float BlurObject::intensity() {
    return m_intensity;
}

void BlurObject::setIntensity(float intensity) {
    m_intensity = intensity;
    bluredTex = true;
    update();
}

QVector2D BlurObject::resolution() {
    return m_resolution;
}

void BlurObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

BlurRenderer::BlurRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    blurShader = new QOpenGLShaderProgram();
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blur.frag");
    blurShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    blurShader->bind();
    blurShader->setUniformValue(blurShader->uniformLocation("sourceTexture"), 0);
    blurShader->setUniformValue(blurShader->uniformLocation("maskTexture"), 1);
    blurShader->release();
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
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BlurRenderer::~BlurRenderer() {
    delete blurShader;
    delete checkerShader;
    delete textureShader;
}

QOpenGLFramebufferObject *BlurRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void BlurRenderer::synchronize(QQuickFramebufferObject *item) {
    BlurObject *blurItem = static_cast<BlurObject*>(item);
    if(blurItem->resUpdated) {
        blurItem->resUpdated = false;
        m_resolution = blurItem->resolution();
        updateTexResolution();
    }
    if(blurItem->bluredTex) {
        blurItem->bluredTex = false;
        m_sourceTexture = blurItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = blurItem->maskTexture();
            blurShader->bind();
            blurShader->setUniformValue(blurShader->uniformLocation("intensity"), blurItem->intensity());
            blurShader->release();
            createBlur();
            blurItem->setTexture(pingpongBuffer[1]);
            if(blurItem->selectedItem) {
                blurItem->updatePreview(pingpongBuffer[1], true);
            }
        }
    }
}

void BlurRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void BlurRenderer::createBlur() {
    bool first_iteration = true;
    QVector2D dir = QVector2D(1, 0);
    int amount = 10;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i%2]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D, first_iteration ? m_sourceTexture : pingpongBuffer[1 - (i%2)]
        );

        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(textureVAO);
        blurShader->bind();
        blurShader->setUniformValue(blurShader->uniformLocation("useMask"), false);
        blurShader->setUniformValue(blurShader->uniformLocation("direction"), dir);
        dir = QVector2D(1, 1) - dir;
        blurShader->setUniformValue(blurShader->uniformLocation("resolution"), m_resolution);
        if(i == amount - 1) blurShader->setUniformValue(blurShader->uniformLocation("useMask"), maskTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        blurShader->release();
        glBindVertexArray(0);
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BlurRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
