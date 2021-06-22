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

#include "mapping.h"
#include <QOpenGLFramebufferObjectFormat>

MappingObject::MappingObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float inputMin,
                             float inputMax, float outputMin, float outputMax):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_inputMin(inputMin),
    m_inputMax(inputMax), m_outputMin(outputMin), m_outputMax(outputMax)
{

}

QQuickFramebufferObject::Renderer *MappingObject::createRenderer() const {
    return new MappingRenderer(m_resolution, m_bpc);
}

unsigned int &MappingObject::texture() {
    return m_texture;
}

void MappingObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int MappingObject::maskTexture() {
    return m_maskTexture;
}

void MappingObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    mappedTex = true;
    update();
}

unsigned int MappingObject::sourceTexture() {
    return m_sourceTexture;
}

void MappingObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    mappedTex = true;
    update();
}

void MappingObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float MappingObject::inputMin() {
    return m_inputMin;
}

void MappingObject::setInputMin(float value) {
    m_inputMin = value;
    mappedTex = true;
    update();
}

float MappingObject::inputMax() {
    return m_inputMax;
}

void MappingObject::setInputMax(float value) {
    m_inputMax = value;
    mappedTex = true;
    update();
}

float MappingObject::outputMin() {
    return m_outputMin;
}

void MappingObject::setOutputMin(float value) {
    m_outputMin = value;
    mappedTex = true;
    update();
}

float MappingObject::outputMax() {
    return  m_outputMax;
}

void MappingObject::setOutputMax(float value) {
    m_outputMax = value;
    mappedTex = true;
    update();
}

QVector2D MappingObject::resolution() {
    return m_resolution;
}

void MappingObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint MappingObject::bpc() {
    return m_bpc;
}

void MappingObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

MappingRenderer::~MappingRenderer() {
    delete mappingShader;
    delete checkerShader;
    delete textureShader;
    glDeleteTextures(1, &m_mappingTexture);
    glDeleteFramebuffers(1, &mappingFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

MappingRenderer::MappingRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();
    mappingShader = new QOpenGLShaderProgram();
    mappingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    mappingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mapping.frag");
    mappingShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    mappingShader->bind();
    mappingShader->setUniformValue(mappingShader->uniformLocation("sourceTexture"), 0);
    mappingShader->setUniformValue(mappingShader->uniformLocation("maskTexture"), 1);
    mappingShader->release();
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

    glGenFramebuffers(1, &mappingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mappingFBO);
    glGenTextures(1, &m_mappingTexture);
    glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mappingTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLFramebufferObject *MappingRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void MappingRenderer::synchronize(QQuickFramebufferObject *item) {
    MappingObject *mappingItem = static_cast<MappingObject*>(item);
    if(mappingItem->resUpdated) {
        mappingItem->resUpdated = false;
        m_resolution = mappingItem->resolution();
        updateTexResolution();
    }
    if(mappingItem->bpcUpdated) {
        mappingItem->bpcUpdated = false;
        m_bpc = mappingItem->bpc();
        updateTexResolution();
        map();
    }
    if(mappingItem->mappedTex) {
        mappingItem->mappedTex = false;
        m_sourceTexture = mappingItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = mappingItem->maskTexture();
            mappingShader->bind();
            mappingShader->setUniformValue(mappingShader->uniformLocation("inputMin"), mappingItem->inputMin());
            mappingShader->setUniformValue(mappingShader->uniformLocation("inputMax"), mappingItem->inputMax());
            mappingShader->setUniformValue(mappingShader->uniformLocation("outputMin"), mappingItem->outputMin());
            mappingShader->setUniformValue(mappingShader->uniformLocation("outputMax"), mappingItem->outputMax());
            mappingShader->setUniformValue(mappingShader->uniformLocation("useMask"), maskTexture);
            mappingShader->release();
            map();
            mappingItem->setTexture(m_mappingTexture);
            mappingItem->updatePreview(m_mappingTexture);
        }
    }
    if(mappingItem->texSaving) {
        mappingItem->texSaving = false;
        saveTexture(mappingItem->saveName);
    }
}

void MappingRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void MappingRenderer::map() {
    glBindFramebuffer(GL_FRAMEBUFFER, mappingFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    mappingShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    mappingShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void MappingRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MappingRenderer::saveTexture(QString fileName) {
    qDebug("texture save");
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);

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
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}
