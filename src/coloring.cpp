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

#include "coloring.h"
#include <QOpenGLFramebufferObjectFormat>

ColoringObject::ColoringObject(QQuickItem *parent, QVector2D resolution, QVector3D color):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_color(color)
{
    setMirrorVertically(true);
}

unsigned int &ColoringObject::texture() {
    return m_texture;
}

void ColoringObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int ColoringObject::sourceTexture() {
    return m_sourceTexture;
}

void ColoringObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    colorizedTex = true;
    update();
}

QVector3D ColoringObject::color() {
    return m_color;
}

void ColoringObject::setColor(QVector3D color) {
    m_color = color;
    colorizedTex = true;
    update();
}

QVector2D ColoringObject::resolution() {
    return m_resolution;
}

void ColoringObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

QQuickFramebufferObject::Renderer *ColoringObject::createRenderer() const {
    return new ColoringRenderer(m_resolution);
}

ColoringRenderer::ColoringRenderer(QVector2D res):m_resolution(res) {
    initializeOpenGLFunctions();
    coloringShader = new QOpenGLShaderProgram();
    coloringShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    coloringShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/coloring.frag");
    coloringShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    coloringShader->bind();
    coloringShader->setUniformValue(coloringShader->uniformLocation("sourceTexture"), 0);
    coloringShader->release();
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

    glGenFramebuffers(1, &colorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ColoringRenderer::~ColoringRenderer() {
    delete coloringShader;
    delete textureShader;
}

QOpenGLFramebufferObject *ColoringRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void ColoringRenderer::synchronize(QQuickFramebufferObject *item) {
    ColoringObject *coloringItem = static_cast<ColoringObject*>(item);
    if(coloringItem->resUpdated) {
        coloringItem->resUpdated = false;
        m_resolution = coloringItem->resolution();
        updateTexResolution();
    }
    if(coloringItem->colorizedTex) {
        coloringItem->colorizedTex = false;
        m_sourceTexture = coloringItem->sourceTexture();
        if(m_sourceTexture) {
            coloringShader->bind();
            coloringShader->setUniformValue(coloringShader->uniformLocation("color"), coloringItem->color());
            coloringShader->release();
            colorize();
            if(coloringItem->selectedItem) coloringItem->updatePreview(m_colorTexture, true);
            coloringItem->setTexture(m_colorTexture);
        }
    }    
}

void ColoringRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_colorTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        textureShader->release();
    }
}

void ColoringRenderer::colorize() {
    glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    coloringShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    coloringShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ColoringRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
