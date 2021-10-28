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

#include "mix.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

MixObject::MixObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float factor,
                     int foregroundOpacity, int backgroundOpacity, int mode, bool includingAlpha):
    QQuickFramebufferObject (parent), m_factor(factor), m_fOpacity(foregroundOpacity),
    m_bOpacity(backgroundOpacity), m_mode(mode), m_includingAlpha(includingAlpha), m_resolution(resolution),
    m_bpc(bpc)
{
}

QQuickFramebufferObject::Renderer *MixObject::createRenderer() const {
    return new MixRenderer(m_resolution, m_bpc);
}

unsigned int MixObject::maskTexture() {
    return  m_maskTexture;
}

void MixObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
}

unsigned int MixObject::firstTexture() {
    return m_firstTexture;
}

void MixObject::setFirstTexture(unsigned int texture) {
    m_firstTexture = texture;
}

unsigned int MixObject::secondTexture() {
    return m_secondTexture;
}

void MixObject::setSecondTexture(unsigned int texture) {
    m_secondTexture = texture;
}

void MixObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

QVariant MixObject::factor() {
    return m_factor;
}

void MixObject::setFactor(QVariant f) {
    if(m_factor == f) return;
    m_factor = f;
    mixedTex = true;
}

int MixObject::mode() {
    return m_mode;
}

void MixObject::setMode(int mode) {
    if(m_mode == mode) return;
    m_mode = mode;
    mixedTex = true;
}

bool MixObject::includingAlpha() {
    return m_includingAlpha;
}

void MixObject::setIncludingAlpha(bool including) {
    if(m_includingAlpha == including) return;
    m_includingAlpha = including;
    mixedTex = true;
}

int MixObject::foregroundOpacity() {
    return m_fOpacity;
}

void MixObject::setForegroundOpacity(int opacity) {
    if(m_fOpacity == opacity) return;
    m_fOpacity = opacity;
    mixedTex = true;
}

int MixObject::backgroundOpacity() {
    return m_bOpacity;
}

void MixObject::setBackgroundOpacity(int opacity) {
    if(m_bOpacity == opacity) return;
    m_bOpacity = opacity;
    mixedTex = true;
}

QVector2D MixObject::resolution() {
    return m_resolution;
}

void MixObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint MixObject::bpc() {
    return m_bpc;
}

void MixObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

unsigned int &MixObject::texture() {
    return m_texture;
}

void MixObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

MixRenderer::~MixRenderer() {
    delete mixShader;
    delete checkerShader;
    delete renderTexture;
    glDeleteTextures(1, &mixTexture);
    glDeleteFramebuffers(1, &mixFBO);
    glDeleteVertexArrays(1, &VAO);
}

MixRenderer::MixRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc) {
    initializeOpenGLFunctions();

    mixShader = new QOpenGLShaderProgram();
    mixShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    mixShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mix.frag");
    mixShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();

    mixShader->bind();
    mixShader->setUniformValue(mixShader->uniformLocation("firstTexture"), 0);
    mixShader->setUniformValue(mixShader->uniformLocation("secondTexture"), 1);
    mixShader->setUniformValue(mixShader->uniformLocation("factorTexture"), 2);
    mixShader->setUniformValue(mixShader->uniformLocation("maskTexture"), 3);
    mixShader->release();

    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->setUniformValue(renderTexture->uniformLocation("lod"), 2.0f);
    renderTexture->release();

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

    glGenFramebuffers(1, &mixFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mixFBO);
    glGenTextures(1, &mixTexture);
    glBindTexture(GL_TEXTURE_2D, mixTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mixTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLFramebufferObject *MixRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void MixRenderer::synchronize(QQuickFramebufferObject *item) {
    MixObject *mixItem = static_cast<MixObject*>(item);
    if(mixItem->resUpdated) {
        mixItem->resUpdated = false;
        m_resolution = mixItem->resolution();
        updateTextureRes();
    }
    if(mixItem->mixedTex || mixItem->bpcUpdated) {
        if(mixItem->bpcUpdated) {
            mixItem->bpcUpdated = false;
            m_bpc = mixItem->bpc();
            updateTextureRes();
        }
        if(mixItem->mixedTex) {
            mixItem->mixedTex = false;
            firstTexture = mixItem->firstTexture();
            secondTexture = mixItem->secondTexture();
            if((firstTexture && secondTexture) || (!firstTexture && !secondTexture)) {
                maskTexture = mixItem->maskTexture();
                currentMode = mixItem->mode();
                mixShader->bind();
                mixShader->setUniformValue(mixShader->uniformLocation("useFactorTex"), mixItem->useFactorTexture);
                mixShader->setUniformValue(mixShader->uniformLocation("includingAlpha"), mixItem->includingAlpha());
                mixShader->setUniformValue(mixShader->uniformLocation("useMask"), maskTexture);
                mixShader->setUniformValue(mixShader->uniformLocation("foregroundOpacity"), mixItem->foregroundOpacity()*0.01f);
                mixShader->setUniformValue(mixShader->uniformLocation("backgroundOpacity"), mixItem->backgroundOpacity()*0.01f);
                mixShader->release();
                if(mixItem->useFactorTexture) {
                    factorTexture = mixItem->factor().toUInt();
                }
                else {
                    mixFactor = mixItem->factor().toFloat();
                }
            }
        }
        if((firstTexture && secondTexture) || (!firstTexture && !secondTexture)) {
            if(firstTexture && secondTexture) {
                mix();
                mixItem->setTexture(mixTexture);
                mixItem->updatePreview(mixTexture);
            }
            else {
                mixItem->setTexture(0);
                mixItem->updatePreview(0);
            }
        }
        else if(firstTexture) {
            mixItem->setTexture(firstTexture);
            mixItem->updatePreview(firstTexture);
        }
        else if(secondTexture) {
            mixItem->setTexture(secondTexture);
            mixItem->updatePreview(secondTexture);
        }
    }
    if(mixItem->texSaving) {
        mixItem->texSaving = false;
        saveTexture(mixItem->saveName);
    }
}

void MixRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    if(firstTexture && secondTexture) {
        renderTexture->bind();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mixTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        renderTexture->release();
    }
    else if(firstTexture) {
        renderTexture->bind();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, firstTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        renderTexture->release();
    }
    else if(secondTexture) {
        renderTexture->bind();
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, secondTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        renderTexture->release();
    }
    glFlush();
}

void MixRenderer::mix() {
    std::cout << "mix" << std::endl;
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, mixFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    mixShader->bind();
    mixShader->setUniformValue(mixShader->uniformLocation("mixFactor"), mixFactor);
    GLuint indexMode = glGetSubroutineIndex(mixShader->programId(), GL_FRAGMENT_SHADER, blendFunc[currentMode].c_str());
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &indexMode);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, firstTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, secondTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, factorTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);    
    glBindVertexArray(0);
    mixShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, mixTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

void MixRenderer::updateTextureRes() {
    glBindTexture(GL_TEXTURE_2D, mixTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void MixRenderer::saveTexture(QString fileName) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("lod"), 0.0f);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    if(firstTexture && secondTexture) glBindTexture(GL_TEXTURE_2D, mixTexture);
    else if(firstTexture) glBindTexture(GL_TEXTURE_2D, firstTexture);
    else if(secondTexture) glBindTexture(GL_TEXTURE_2D, secondTexture);
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
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}
