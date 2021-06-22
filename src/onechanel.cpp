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

#include "onechanel.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

OneChanelObject::OneChanelObject(QQuickItem *parent, QVector2D resolution, GLint bpc): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_bpc(bpc)
{

}

QQuickFramebufferObject::Renderer *OneChanelObject::createRenderer() const {
    return new OneChanelRenderer(m_resolution, m_bpc);
}

QVariant OneChanelObject::value() {
    return m_value;
}

void OneChanelObject::setValue(QVariant val) {
    m_value = val;
}

void OneChanelObject::setColorTexture(unsigned int texture) {
    m_colorTexture = texture;
}

void OneChanelObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
}

unsigned int &OneChanelObject::texture() {
    if(useTex) return m_sourceTexture;
    else return m_colorTexture;
}

void OneChanelObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

QVector2D OneChanelObject::resolution() {
    return m_resolution;
}

void OneChanelObject::setResolution(QVector2D res) {
    m_resolution = res;
}

GLint OneChanelObject::bpc() {
    return m_bpc;
}

void OneChanelObject::setBPC(GLint bpc) {
    m_bpc = bpc;
}

OneChanelRenderer::OneChanelRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc) {
    initializeOpenGLFunctions();

    renderChanel = new QOpenGLShaderProgram();
    renderChanel->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderChanel->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/onechanel.frag");
    renderChanel->link();

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f, 1.0f};
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

    renderChanel->bind();
    renderChanel->setUniformValue(renderChanel->uniformLocation("useTex"), false);
    renderChanel->setUniformValue(renderChanel->uniformLocation("tex"), 0);
    renderChanel->setUniformValue(renderChanel->uniformLocation("val"), val);
    renderChanel->release();

    glGenFramebuffers(1, &m_colorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_colorFBO);
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    createColor();
}

OneChanelRenderer::~OneChanelRenderer() {
    delete renderChanel;
    glDeleteTextures(1, &m_colorTexture);
    glDeleteFramebuffers(1, &m_colorFBO);
    glDeleteVertexArrays(1, &VAO);
}

QOpenGLFramebufferObject *OneChanelRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void OneChanelRenderer::synchronize(QQuickFramebufferObject *item) {
    OneChanelObject *oneChanelItem = static_cast<OneChanelObject*>(item);
    renderChanel->bind();
    renderChanel->setUniformValue(renderChanel->uniformLocation("useTex"), oneChanelItem->useTex);
    m_resolution = oneChanelItem->resolution();
    m_bpc = oneChanelItem->bpc();
    if(oneChanelItem->useTex) {
        texture = oneChanelItem->value().toUInt();
        oneChanelItem->setSourceTexture(texture);
        oneChanelItem->updatePreview(texture);
        oneChanelItem->updateValue(texture, true);
    }
    else {
        val = oneChanelItem->value().toFloat();
        renderChanel->setUniformValue(renderChanel->uniformLocation("val"), val);
        createColor();
        oneChanelItem->setColorTexture(m_colorTexture);
        oneChanelItem->updatePreview(m_colorTexture);
        oneChanelItem->updateValue(val, false);
    }
    renderChanel->release();

    if(oneChanelItem->texSaving) {
        oneChanelItem->texSaving = false;
        saveTexture(oneChanelItem->saveName);
    }
}

void OneChanelRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderChanel->bind();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderChanel->release();
    glFlush();
}

void OneChanelRenderer::createColor() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_colorFBO);
    glViewport(0, 0, 8, 8);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderChanel->bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderChanel->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void OneChanelRenderer::saveTexture(QString fileName) {
    unsigned int fbo;
    unsigned int tex;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, m_resolution.x(), m_resolution.y(), 0, GL_RGB, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_resolution.x(), m_resolution.y(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderChanel->bind();
    renderChanel->setUniformValue(renderChanel->uniformLocation("albedoVal"), val);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderChanel->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(m_bpc == GL_RGBA16) {
        GLushort *pixels = (GLushort*)malloc(sizeof(GLushort)*3*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGR, GL_UNSIGNED_SHORT, pixels);
        FIBITMAP *image16 = FreeImage_AllocateT(FIT_RGB16, m_resolution.x(), m_resolution.y());
        int m_width = FreeImage_GetWidth(image16);
        int m_height = FreeImage_GetHeight(image16);
        for(int y = 0; y < m_height; ++y) {
            FIRGB16 *bits = (FIRGB16*)FreeImage_GetScanLine(image16, y);
            for(int x = 0; x < m_width; ++x) {
                bits[x].red = pixels[(m_width*(m_height - 1 - y) + x)*3 + 2];
                bits[x].green = pixels[(m_width*(m_height - 1 - y) + x)*3 + 1];
                bits[x].blue = pixels[(m_width*(m_height - 1 - y) + x)*3];
            }
        }
        if (FreeImage_Save(FIF_PNG, image16, fileName.toUtf8().constData(), 0))
            printf("Successfully saved!\n");
        else
            printf("Failed saving!\n");
        FreeImage_Unload(image16);
        delete [] pixels;
    }
    else if(m_bpc == GL_RGBA8) {
        BYTE *pixels = (BYTE*)malloc(3*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGR, GL_UNSIGNED_BYTE, pixels);
        FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 3 * m_resolution.x(), 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
        if (FreeImage_Save(FIF_PNG, image, fileName.toUtf8().constData(), 0))
            printf("Successfully saved!\n");
        else
            printf("Failed saving!\n");
        FreeImage_Unload(image);
        delete [] pixels;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &tex);
    glDeleteFramebuffers(1, &fbo);
}
