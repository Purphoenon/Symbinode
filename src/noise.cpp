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

#include "noise.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

NoiseObject::NoiseObject(QQuickItem *parent, QVector2D resolution, QString type, float noiseScale,
                         float scaleX, float scaleY, int layers, float persistence, float amplitude,
                         int seed): QQuickFramebufferObject (parent), m_noiseType(type),
    m_noiseScale(noiseScale), m_scaleX(scaleX), m_scaleY(scaleY), m_layers(layers),
    m_persistence(persistence), m_amplitude(amplitude), m_seed(seed), m_resolution(resolution)
{

}

QQuickFramebufferObject::Renderer *NoiseObject::createRenderer() const{
    return new NoiseRenderer(m_resolution);
}

unsigned int NoiseObject::maskTexture() {
    return m_maskTexture;
}

void NoiseObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedNoise = true;
    update();
}

void NoiseObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

QString NoiseObject::noiseType() {
    return m_noiseType;
}

void NoiseObject::setNoiseType(QString type) {
    m_noiseType = type;
    generatedNoise = true;
    update();
}

float NoiseObject::noiseScale() {
    return m_noiseScale;
}

void NoiseObject::setNoiseScale(float scale) {
    m_noiseScale = scale;
    generatedNoise = true;
    update();
}

float NoiseObject::scaleX() {
    return m_scaleX;
}

void NoiseObject::setScaleX(float scale) {
    m_scaleX = scale;
    generatedNoise = true;
    update();
}

float NoiseObject::scaleY() {
    return m_scaleY;
}

void NoiseObject::setScaleY(float scale) {
    m_scaleY = scale;
    generatedNoise = true;
    update();
}

int NoiseObject::layers() {
    return m_layers;
}

void NoiseObject::setLayers(int num) {
    m_layers = num;
    generatedNoise = true;
    update();
}

float NoiseObject::persistence() {
    return m_persistence;
}

void NoiseObject::setPersistence(float value) {
    m_persistence = value;
    generatedNoise = true;
    update();
}

float NoiseObject::amplitude() {
    return m_amplitude;
}

void NoiseObject::setAmplitude(float value) {
    m_amplitude = value;
    generatedNoise = true;
    update();
}

int NoiseObject::seed() {
    return m_seed;
}

void NoiseObject::setSeed(int seed) {
    m_seed = seed;
    generatedNoise = true;
    update();
}

QVector2D NoiseObject::resolution() {
    return m_resolution;
}

void NoiseObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

unsigned int &NoiseObject::texture() {
    return m_texture;
}

void NoiseObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

NoiseRenderer::NoiseRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();

    generateNoise = new QOpenGLShaderProgram();
    generateNoise->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    generateNoise->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/noise.frag");
    generateNoise->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();

    generateNoise->bind();
    generateNoise->setUniformValue(generateNoise->uniformLocation("maskTexture"), 0);
    generateNoise->release();

    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();

    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &noiseVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(noiseVAO);
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

    glGenFramebuffers(1, &noiseFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, noiseTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //createNoise();
}

NoiseRenderer::~NoiseRenderer() {
    delete generateNoise;
    delete checkerShader;
    delete renderTexture;
}

QOpenGLFramebufferObject *NoiseRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void NoiseRenderer::synchronize(QQuickFramebufferObject *item) {
    NoiseObject *noiseItem = static_cast<NoiseObject*>(item);
    if(noiseItem->resUpdated) {
        noiseItem->resUpdated = false;
        m_resolution = noiseItem->resolution();
        updateTexResolution();
        if(!m_maskTexture) {
            createNoise();
            noiseItem->setTexture(noiseTexture);
        }
    }
    if(noiseItem->generatedNoise) {
        noiseItem->generatedNoise = false;
        m_noiseType = noiseItem->noiseType();
        m_maskTexture = noiseItem->maskTexture();
        generateNoise->bind();
        generateNoise->setUniformValue(generateNoise->uniformLocation("scale"), noiseItem->noiseScale());
        generateNoise->setUniformValue(generateNoise->uniformLocation("scaleX"), noiseItem->scaleX());
        generateNoise->setUniformValue(generateNoise->uniformLocation("scaleY"), noiseItem->scaleY());
        generateNoise->setUniformValue(generateNoise->uniformLocation("octaves"), noiseItem->layers());
        generateNoise->setUniformValue(generateNoise->uniformLocation("persistence"), noiseItem->persistence());
        generateNoise->setUniformValue(generateNoise->uniformLocation("amplitude"), noiseItem->amplitude());
        generateNoise->setUniformValue(generateNoise->uniformLocation("seed"), noiseItem->seed());
        generateNoise->setUniformValue(generateNoise->uniformLocation("res"), m_resolution);
        generateNoise->setUniformValue(generateNoise->uniformLocation("useMask"), m_maskTexture);
        createNoise();
        noiseItem->setTexture(noiseTexture);
        noiseItem->updatePreview(noiseTexture);
    }
    if(noiseItem->texSaving) {
        noiseItem->texSaving = true;
        saveTexture(noiseItem->saveName);
    }
}

void NoiseRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(noiseVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderTexture->release();
}

void NoiseRenderer::createNoise() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, noiseFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    generateNoise->bind();
    generateNoise->setUniformValue(generateNoise->uniformLocation("res"), m_resolution);
    GLuint index = glGetSubroutineIndex(generateNoise->programId(), GL_FRAGMENT_SHADER, m_noiseType.toStdString().c_str());
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);
    glBindVertexArray(noiseVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);    
    generateNoise->release();
}

void NoiseRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void NoiseRenderer::saveTexture(QString fileName) {
    qDebug("texture save");
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderTexture->release();

    BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    if (FreeImage_Save(FIF_PNG, image, fileName.toUtf8().constData(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
