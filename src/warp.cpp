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

#include "warp.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

WarpObject::WarpObject(QQuickItem *parent, QVector2D resolution, float intensity):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_intensity(intensity)
{

}

QQuickFramebufferObject::Renderer *WarpObject::createRenderer() const {
    return new WarpRenderer(m_resolution);
}

unsigned int &WarpObject::texture() {
    return m_texture;
}

void WarpObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int WarpObject::maskTexture() {
    return m_maskTexture;
}

void WarpObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    warpedTex = true;
    update();
}

unsigned int WarpObject::sourceTexture() {
    return m_sourceTexture;
}

void WarpObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    warpedTex = true;
    update();
}

unsigned int WarpObject::warpTexture() {
    return m_warpTexture;
}

void WarpObject::setWarpTexture(unsigned int texture) {
    m_warpTexture = texture;
    warpedTex = true;
    update();
}

void WarpObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float WarpObject::intensity() {
    return m_intensity;
}

void WarpObject::setIntensity(float intensity) {
    m_intensity = intensity;
    warpedTex = true;
    update();
}

QVector2D WarpObject::resolution() {
    return m_resolution;
}

void WarpObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

WarpRenderer::WarpRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    warpShader = new QOpenGLShaderProgram();
    warpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    warpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/warp.frag");
    warpShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    warpShader->bind();
    warpShader->setUniformValue(warpShader->uniformLocation("sourceTexture"), 0);
    warpShader->setUniformValue(warpShader->uniformLocation("warpTexture"), 1);
    warpShader->setUniformValue(warpShader->uniformLocation("maskTexture"), 2);
    warpShader->release();
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
    glGenFramebuffers(1, &warpFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glGenTextures(1, &m_warpedTexture);
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_warpedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

WarpRenderer::~WarpRenderer() {
    delete warpShader;
    delete checkerShader;
    delete textureShader;
}

QOpenGLFramebufferObject *WarpRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void WarpRenderer::synchronize(QQuickFramebufferObject *item) {
    WarpObject *warpItem = static_cast<WarpObject*>(item);
    if(warpItem->resUpdated) {
        warpItem->resUpdated = false;
        m_resolution = warpItem->resolution();
        updateTexResolution();
    }
    if(warpItem->warpedTex) {
        warpItem->warpedTex = false;
        m_sourceTexture = warpItem->sourceTexture();
        if(m_sourceTexture) {
            m_warpTexture = warpItem->warpTexture();
            maskTexture = warpItem->maskTexture();
            warpShader->bind();
            warpShader->setUniformValue(warpShader->uniformLocation("intensity"), warpItem->intensity());
            warpShader->setUniformValue(warpShader->uniformLocation("useMask"), maskTexture);
            warpShader->release();
            createWarp();
            warpItem->setTexture(m_warpedTexture);            
            warpItem->updatePreview(m_warpedTexture);
        }
    }
    if(warpItem->texSaving) {
        warpItem->texSaving = false;
        saveTexture(warpItem->saveName);
    }
}

void WarpRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void WarpRenderer::createWarp() {
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    warpShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_warpTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    warpShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WarpRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void WarpRenderer::saveTexture(QString fileName) {
    unsigned int fbo;
    unsigned int tex;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);

    BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    if (FreeImage_Save(FIF_PNG, image, fileName.toStdString().c_str(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
