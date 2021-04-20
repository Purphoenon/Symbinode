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

#include "circle.h"
#include "QOpenGLFramebufferObjectFormat"
#include <iostream>

CircleObject::CircleObject(QQuickItem *parent, QVector2D resolution, int interpolation, float radius,
                           float smooth, bool useAlpha): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_interpolation(interpolation), m_radius(radius), m_smooth(smooth),
    m_useAlpha(useAlpha)
{

}

QQuickFramebufferObject::Renderer *CircleObject::createRenderer() const {
    return new CircleRenderer(m_resolution);
}

unsigned int CircleObject::maskTexture() {
    return m_maskTexture;
}

void CircleObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedCircle = true;
    update();
}

unsigned int &CircleObject::texture() {
    return m_texture;
}

void CircleObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

void CircleObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

int CircleObject::interpolation() {
    return m_interpolation;
}

void CircleObject::setInterpolation(int interpolation) {
    m_interpolation = interpolation;
    generatedCircle = true;
    update();
}

float CircleObject::radius() {
    return m_radius;
}

void CircleObject::setRadius(float radius) {
    m_radius = radius;
    generatedCircle = true;
    update();
}

float CircleObject::smooth() {
    return m_smooth;
}

void CircleObject::setSmooth(float smooth) {
    m_smooth = smooth;
    generatedCircle = true;
    update();
}

bool CircleObject::useAlpha() {
    return m_useAlpha;
}

void CircleObject::setUseAlpha(bool use) {
    m_useAlpha = use;
    generatedCircle = true;
    update();
}

QVector2D CircleObject::resolution() {
    return m_resolution;
}

void CircleObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

CircleRenderer::CircleRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();
    generateCircle = new QOpenGLShaderProgram();
    generateCircle->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    generateCircle->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/circle.frag");
    generateCircle->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    generateCircle->bind();
    generateCircle->setUniformValue(generateCircle->uniformLocation("maskTexture"), 0);
    generateCircle->release();
    renderTexture->link();
    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();
    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuad), vertQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int VBO2;
    glGenVertexArrays(1, &textureVAO);
    glBindVertexArray(textureVAO);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenFramebuffers(1, &circleFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, circleFBO);
    glGenTextures(1, &circleTexture);
    glBindTexture(GL_TEXTURE_2D, circleTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, circleTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    createCircle();
}

CircleRenderer::~CircleRenderer() {
    delete generateCircle;
    delete checkerShader;
    delete renderTexture;
}

QOpenGLFramebufferObject *CircleRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void CircleRenderer::synchronize(QQuickFramebufferObject *item) {
    CircleObject *circleItem = static_cast<CircleObject*>(item);
    if(circleItem->resUpdated) {
        circleItem->resUpdated = false;
        m_resolution = circleItem->resolution();
        updateTexResolution();
        if(!maskTexture) {
            createCircle();
            circleItem->setTexture(circleTexture);
        }
    }
    if(circleItem->generatedCircle) {
        circleItem->generatedCircle = false;
        maskTexture = circleItem->maskTexture();
        generateCircle->bind();
        generateCircle->setUniformValue(generateCircle->uniformLocation("interpolation"), circleItem->interpolation());
        generateCircle->setUniformValue(generateCircle->uniformLocation("radius"), circleItem->radius());
        generateCircle->setUniformValue(generateCircle->uniformLocation("smoothValue"), circleItem->smooth());
        generateCircle->setUniformValue(generateCircle->uniformLocation("useAlpha"), circleItem->useAlpha());
        generateCircle->setUniformValue(generateCircle->uniformLocation("useMask"), maskTexture);
        generateCircle->release();
        createCircle();
        circleItem->setTexture(circleTexture);
        circleItem->updatePreview(circleTexture);
    }
    if(circleItem->texSaving) {
        circleItem->texSaving = false;
        saveTexture(circleItem->saveName);
    }
}

void CircleRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(circleVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, circleTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    renderTexture->release();
}

void CircleRenderer::createCircle() {
    glBindFramebuffer(GL_FRAMEBUFFER, circleFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(circleVAO);
    generateCircle->bind();
    generateCircle->setUniformValue(generateCircle->uniformLocation("res"), m_resolution);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    generateCircle->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CircleRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, circleTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CircleRenderer::saveTexture(QString fileName) {
    qDebug("texture save");
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, circleTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    renderTexture->release();

    BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 64, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    FIBITMAP *result = FreeImage_ConvertToRGBA16(image);

    if (FreeImage_Save(FIF_PNG, result, fileName.toUtf8().constData(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
