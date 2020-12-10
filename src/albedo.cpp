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

#include "albedo.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

AlbedoObject::AlbedoObject(QQuickItem *parent, QVector2D resolution): QQuickFramebufferObject (parent),
    m_resolution(resolution)
{
}

QQuickFramebufferObject::Renderer *AlbedoObject::createRenderer() const {
    return new AlbedoRenderer(m_resolution);
}

QVariant AlbedoObject::albedo() {
    return m_albedo;
}

void AlbedoObject::setAlbedo(QVariant albedo) {
    m_albedo = albedo;
}

QVector2D AlbedoObject::resolution() {
    return m_resolution;
}

void AlbedoObject::setResolution(QVector2D res) {
    m_resolution = res;
}

AlbedoRenderer::AlbedoRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();
    renderAlbedo = new QOpenGLShaderProgram();
    renderAlbedo->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderAlbedo->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/albedo.frag");
    renderAlbedo->link();

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 1.0f,
                    -1.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, -1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 0.0f};
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

    renderAlbedo->bind();
    renderAlbedo->setUniformValue(renderAlbedo->uniformLocation("albedoTex"), 0);
    renderAlbedo->release();
}

AlbedoRenderer::~AlbedoRenderer() {
    delete renderAlbedo;
}

QOpenGLFramebufferObject *AlbedoRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void AlbedoRenderer::synchronize(QQuickFramebufferObject *item) {
    AlbedoObject *albedoItem = static_cast<AlbedoObject*>(item);
    renderAlbedo->bind();
    renderAlbedo->setUniformValue(renderAlbedo->uniformLocation("useAlbedoTex"), albedoItem->useAlbedoTex);
    renderAlbedo->release();
    m_resolution = albedoItem->resolution();
    if(albedoItem->useAlbedoTex) {
        albedoTexture = albedoItem->albedo().toUInt();
        if(albedoItem->selectedItem) {
            albedoItem->updatePreview(albedoTexture, true);
        }
        albedoItem->updateAlbedo(albedoTexture, true);
    }
    else {
        albedoVal = qvariant_cast<QVector3D>(albedoItem->albedo());
        if(albedoItem->selectedItem) {
            albedoItem->updatePreview(albedoVal, false);
        }
        albedoItem->updateAlbedo(albedoVal, false);
    }
    if(albedoItem->texSaving) {
        albedoItem->texSaving = false;
        saveTexture(albedoItem->saveName);
    }
}

void AlbedoRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderAlbedo->bind();
    renderAlbedo->setUniformValue(renderAlbedo->uniformLocation("albedoVal"), albedoVal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderAlbedo->release();
}

void AlbedoRenderer::saveTexture(QString dir) {
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
    renderAlbedo->bind();
    renderAlbedo->setUniformValue(renderAlbedo->uniformLocation("albedoVal"), albedoVal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderAlbedo->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    BYTE *pixels = (BYTE*)malloc(3*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGR, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 3 * m_resolution.x(), 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    QString name = dir.append("/albedo.png");
    if (FreeImage_Save(FIF_PNG, image, name.toStdString().c_str(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
