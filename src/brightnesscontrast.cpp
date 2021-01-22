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

#include "brightnesscontrast.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>

BrightnessContrastObject::BrightnessContrastObject(QQuickItem *parent, QVector2D resolution,
                                                   float brightness, float contrast):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_brightness(brightness),
    m_contrast(contrast)
{
}

QQuickFramebufferObject::Renderer *BrightnessContrastObject::createRenderer() const {
    return new BrightnessContrastRenderer(m_resolution);
}

unsigned int &BrightnessContrastObject::texture() {
    return m_texture;
}

void BrightnessContrastObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int BrightnessContrastObject::sourceTexture() {
    return m_sourceTexture;
}

void BrightnessContrastObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    created = true;
    update();
}

void BrightnessContrastObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float BrightnessContrastObject::brightness() {
    return m_brightness;
}

void BrightnessContrastObject::setBrightness(float value) {
    m_brightness = value;
    created = true;
    update();
}

float BrightnessContrastObject::contrast() {
    return m_contrast;
}

void BrightnessContrastObject::setContrast(float value) {
    m_contrast = value;
    created = true;
    update();
}

QVector2D BrightnessContrastObject::resolution() {
    return m_resolution;
}

void BrightnessContrastObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

BrightnessContrastRenderer::BrightnessContrastRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    brightnessContrastShader = new QOpenGLShaderProgram();
    brightnessContrastShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    brightnessContrastShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/brightnesscontrast.frag");
    brightnessContrastShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    brightnessContrastShader->bind();
    brightnessContrastShader->setUniformValue(brightnessContrastShader->uniformLocation("sourceTexture"), 0);
    brightnessContrastShader->release();
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

    glGenFramebuffers(1, &brightnessContrastFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, brightnessContrastFBO);
    glGenTextures(1, &m_brightnessContrastTexture);
    glBindTexture(GL_TEXTURE_2D, m_brightnessContrastTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brightnessContrastTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BrightnessContrastRenderer::~BrightnessContrastRenderer() {
    delete brightnessContrastShader;
    delete checkerShader;
    delete textureShader;
}

QOpenGLFramebufferObject *BrightnessContrastRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void BrightnessContrastRenderer::synchronize(QQuickFramebufferObject *item) {
    BrightnessContrastObject *brightnessContrastItem = static_cast<BrightnessContrastObject*>(item);
    if(brightnessContrastItem->resUpdated) {
        brightnessContrastItem->resUpdated = false;
        m_resolution = brightnessContrastItem->resolution();
        updateTexResolution();
    }
    if(brightnessContrastItem->created) {
        brightnessContrastItem->created = false;
        m_sourceTexture = brightnessContrastItem->sourceTexture();
        if(m_sourceTexture) {
            brightnessContrastShader->bind();
            brightnessContrastShader->setUniformValue(brightnessContrastShader->uniformLocation("brightness"), brightnessContrastItem->brightness());
            brightnessContrastShader->setUniformValue(brightnessContrastShader->uniformLocation("contrast"), brightnessContrastItem->contrast());
            brightnessContrastShader->release();
            create();
            brightnessContrastItem->setTexture(m_brightnessContrastTexture);
            brightnessContrastItem->updatePreview(m_brightnessContrastTexture);
        }
    }
    if(brightnessContrastItem->texSaving) {
        brightnessContrastItem->texSaving = false;
        saveTexture(brightnessContrastItem->saveName);
    }
}

void BrightnessContrastRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_brightnessContrastTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void BrightnessContrastRenderer::create() {
    glBindFramebuffer(GL_FRAMEBUFFER, brightnessContrastFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    brightnessContrastShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    brightnessContrastShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BrightnessContrastRenderer::updateTexResolution(){
    glBindTexture(GL_TEXTURE_2D, m_brightnessContrastTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BrightnessContrastRenderer::saveTexture(QString fileName) {
    qDebug("texture save");
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_resolution.x(), m_resolution.y(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightnessContrastTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);

    BYTE *pixels = (BYTE*)malloc(3*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGR, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 3 * m_resolution.x(), 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    if (FreeImage_Save(FIF_PNG, image, fileName.toUtf8().constData(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
