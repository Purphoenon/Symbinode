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

#include <iostream>
#include "voronoi.h"
#include <QOpenGLFramebufferObjectFormat>

VoronoiObject::VoronoiObject(QQuickItem *parent, QVector2D resolution, QString voronoiType, int scale,
                             int scaleX, int scaleY, float jitter, bool inverse, float intensity,
                             float bordersSize, int seed): QQuickFramebufferObject(parent),
    m_resolution(resolution), m_voronoiType(voronoiType), m_scale(scale), m_scaleX(scaleX),
    m_scaleY(scaleY), m_jitter(jitter), m_inverse(inverse), m_intensity(intensity),
    m_borders(bordersSize), m_seed(seed)
{

}

QQuickFramebufferObject::Renderer *VoronoiObject::createRenderer() const {
    return new VoronoiRenderer(m_resolution);
}

unsigned int VoronoiObject::maskTexture() {
    return m_maskTexture;
}

void VoronoiObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedVoronoi = true;
    update();
}

unsigned int &VoronoiObject::texture() {
    return m_texture;
}

void VoronoiObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

QString VoronoiObject::voronoiType() {
    return m_voronoiType;
}

void VoronoiObject::setVoronoiType(QString type) {
    m_voronoiType = type;
    generatedVoronoi = true;
    update();
}

int VoronoiObject::voronoiScale() {
    return m_scale;
}

void VoronoiObject::setVoronoiScale(int scale) {
    m_scale = scale;
    generatedVoronoi = true;
    update();
}

int VoronoiObject::scaleX() {
    return m_scaleX;
}

void VoronoiObject::setScaleX(int scale) {
    m_scaleX = scale;
    generatedVoronoi = true;
    update();
}

int VoronoiObject::scaleY() {
    return m_scaleY;
}

void VoronoiObject::setScaleY(int scale) {
    m_scaleY = scale;
    generatedVoronoi = true;
    update();
}

float VoronoiObject::jitter() {
    return m_jitter;
}

void VoronoiObject::setJitter(float jitter) {
    m_jitter = jitter;
    generatedVoronoi = true;
    update();
}

bool VoronoiObject::inverse() {
    return m_inverse;
}

void VoronoiObject::setInverse(bool inverse) {
    m_inverse = inverse;
    generatedVoronoi = true;
    update();
}

float VoronoiObject::intensity() {
    return m_intensity;
}

void VoronoiObject::setIntensity(float intensity) {
    m_intensity = intensity;
    generatedVoronoi = true;
    update();
}

float VoronoiObject::bordersSize() {
    return m_borders;
}

void VoronoiObject::setBordersSize(float size) {
    m_borders = size;
    generatedVoronoi = true;
    update();
}

int VoronoiObject::seed() {
    return m_seed;
}

void VoronoiObject::setSeed(int seed) {
    m_seed = seed;
    generatedVoronoi = true;
    update();
}

QVector2D VoronoiObject::resolution() {
    return m_resolution;
}

void VoronoiObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

VoronoiRenderer::VoronoiRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    generateVoronoi = new QOpenGLShaderProgram();
    generateVoronoi->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    generateVoronoi->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/voronoi.frag");
    generateVoronoi->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();
    generateVoronoi->bind();
    generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("maskTexture"), 0);
    generateVoronoi->release();
    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();
    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &voronoiVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(voronoiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuad), vertQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 1.0f,
                    -1.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, -1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 0.0f};
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

    glGenFramebuffers(1, &voronoiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, voronoiFBO);
    glGenTextures(1, &voronoiTexture);
    glBindTexture(GL_TEXTURE_2D, voronoiTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, voronoiTexture, 0);    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    createVoronoi();
}

VoronoiRenderer::~VoronoiRenderer() {
    delete generateVoronoi;
    delete  checkerShader;
    delete renderTexture;
}

QOpenGLFramebufferObject *VoronoiRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void VoronoiRenderer::synchronize(QQuickFramebufferObject *item) {
    VoronoiObject *voronoiItem = static_cast<VoronoiObject*>(item);
    if(voronoiItem->resUpdated) {
        voronoiItem->resUpdated = false;
        m_resolution = voronoiItem->resolution();
        updateTexResolution();
        if(!maskTexture) {
            createVoronoi();
            voronoiItem->setTexture(voronoiTexture);
        }
    }
    if(voronoiItem->generatedVoronoi) {
        voronoiItem->generatedVoronoi = false;
        m_voronoiType = voronoiItem->voronoiType();
        maskTexture = voronoiItem->maskTexture();
        generateVoronoi->bind();
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("scale"), voronoiItem->voronoiScale());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("scaleX"), voronoiItem->scaleX());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("scaleY"), voronoiItem->scaleY());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("jitter"), voronoiItem->jitter());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("inverse"), voronoiItem->inverse());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("intensity"), voronoiItem->intensity());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("bordersSize"), voronoiItem->bordersSize());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("seed"), voronoiItem->seed());
        generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("useMask"), maskTexture);
        generateVoronoi->release();
        createVoronoi();
        voronoiItem->setTexture(voronoiTexture);
        voronoiItem->updatePreview(voronoiTexture);
    }    
}

void VoronoiRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(voronoiVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    glBindVertexArray(textureVAO);
    renderTexture->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, voronoiTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderTexture->release();
    glBindVertexArray(0);
}

void VoronoiRenderer::createVoronoi() {
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, voronoiFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(voronoiVAO);
    generateVoronoi->bind();
    GLuint index = glGetSubroutineIndex(generateVoronoi->programId(), GL_FRAGMENT_SHADER, m_voronoiType.toStdString().c_str());
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);
    generateVoronoi->setUniformValue(generateVoronoi->uniformLocation("res"), m_resolution);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    generateVoronoi->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void VoronoiRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, voronoiTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
