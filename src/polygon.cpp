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

#include "polygon.h"
#include "QOpenGLFramebufferObjectFormat"
#include <iostream>
#include "FreeImage.h"

PolygonObject::PolygonObject(QQuickItem *parent, QVector2D resolution, GLint bpc, int sides,
                             float polygonScale, float smooth, bool useAlpha):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_sides(sides),
    m_scale(polygonScale), m_smooth(smooth), m_useAlpha(useAlpha)
{

}

QQuickFramebufferObject::Renderer *PolygonObject::createRenderer() const {
    return new PolygonRenderer(m_resolution, m_bpc);
}

unsigned int PolygonObject::maskTexture() {
    return m_maskTexture;
}

void PolygonObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedPolygon = true;
    update();
}

unsigned int &PolygonObject::texture() {
    return m_texture;
}

void PolygonObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

void PolygonObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

int PolygonObject::sides() {
    return m_sides;
}

void PolygonObject::setSides(int sides) {
    m_sides = sides;
    generatedPolygon = true;
    update();
}

float PolygonObject::polygonScale() {
    return m_scale;
}

void PolygonObject::setPolygonScale(float scale) {
    m_scale = scale;
    generatedPolygon = true;
    update();
}

float PolygonObject::smooth() {
    return m_smooth;
}

void PolygonObject::setSmooth(float smooth) {
    m_smooth = smooth;
    generatedPolygon = true;
    update();
}

bool PolygonObject::useAlpha() {
    return m_useAlpha;
}

void PolygonObject::setUseAlpha(bool use) {
    m_useAlpha = use;
    generatedPolygon = true;
    update();
}

QVector2D PolygonObject::resolution() {
    return m_resolution;
}

void PolygonObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint PolygonObject::bpc() {
    return m_bpc;
}

void PolygonObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

PolygonRenderer::PolygonRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc) {
    initializeOpenGLFunctions();
    generatePolygon = new QOpenGLShaderProgram();
    generatePolygon->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    generatePolygon->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/polygon.frag");
    generatePolygon->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();
    generatePolygon->bind();
    generatePolygon->setUniformValue(generatePolygon->uniformLocation("maskTexture"), 0);
    generatePolygon->release();
    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();
    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &polygonVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(polygonVAO);
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

    glGenFramebuffers(1, &polygonFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, polygonFBO);
    glGenTextures(1, &polygonTexture);
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, polygonTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    createPolygon();
}

PolygonRenderer::~PolygonRenderer() {
    delete generatePolygon;
    delete checkerShader;
    delete renderTexture;
    glDeleteTextures(1, &polygonTexture);
    glDeleteFramebuffers(1, &polygonFBO);
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteVertexArrays(1, &polygonVAO);
}

QOpenGLFramebufferObject *PolygonRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void PolygonRenderer::synchronize(QQuickFramebufferObject *item) {
    PolygonObject *polygonItem = static_cast<PolygonObject*>(item);
    if(polygonItem->resUpdated) {
        polygonItem->resUpdated = false;
        m_resolution = polygonItem->resolution();
        updateTexResolution();
        if(!maskTexture) {
            createPolygon();
            polygonItem->setTexture(polygonTexture);
        }
    }
    if(polygonItem->bpcUpdated) {
        polygonItem->bpcUpdated = false;
        m_bpc = polygonItem->bpc();
        updateTexResolution();
        createPolygon();
    }
    if(polygonItem->generatedPolygon) {
        polygonItem->generatedPolygon = false;
        maskTexture = polygonItem->maskTexture();        
        generatePolygon->bind();
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("sides"), polygonItem->sides());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("scale"), polygonItem->polygonScale());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("smoothValue"), polygonItem->smooth());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("useAlpha"), polygonItem->useAlpha());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("useMask"), maskTexture);
        generatePolygon->release();
        createPolygon();
        polygonItem->setTexture(polygonTexture);
        polygonItem->updatePreview(polygonTexture);
    }
    if(polygonItem->texSaving) {
        polygonItem->texSaving = false;
        saveTexture(polygonItem->saveName);
    }
}

void PolygonRenderer::render() {
    glDisable(GL_DEPTH_TEST);    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(polygonVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    renderTexture->release();
    glFlush();
}

void PolygonRenderer::createPolygon() {
    glBindFramebuffer(GL_FRAMEBUFFER, polygonFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(polygonVAO);
    generatePolygon->bind();
    generatePolygon->setUniformValue(generatePolygon->uniformLocation("res"), m_resolution);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    generatePolygon->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void PolygonRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PolygonRenderer::saveTexture(QString fileName) {
    unsigned int fbo;
    unsigned int tex;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, tex);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    renderTexture->release();

    if(m_bpc == GL_RGBA16) {
        GLushort *pixels = (GLushort*)malloc(sizeof(GLushort)*4*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_SHORT, pixels);
        FIBITMAP *image16 = FreeImage_AllocateT(FIT_RGBA16, m_resolution.x(), m_resolution.y());
        int m_width = FreeImage_GetWidth(image16);
        int m_height = FreeImage_GetHeight(image16);
        for(int y = 0; y < m_height; ++y) {
            FIRGBA16 *bits = (FIRGBA16*)FreeImage_GetScanLine(image16, y);
            for(int x = 0; x < m_width; ++x) {
                bits[x].red = pixels[(m_width*(m_height - 1 - y) + x)*4 + 2];
                bits[x].green = pixels[(m_width*(m_height - 1 - y) + x)*4 + 1];
                bits[x].blue = pixels[(m_width*(m_height - 1 - y) + x)*4];
                bits[x].alpha = pixels[(m_width*(m_height - 1 - y) + x)*4 + 3];
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
        BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
        FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
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
