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

#include "transform.h"
#include "QOpenGLFramebufferObjectFormat"
#include "FreeImage.h"

TransformObject::TransformObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float transX,
                                 float transY, float scaleX, float scaleY, int angle, bool clamp):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_translateX(transX),
    m_translateY(transY), m_scaleX(scaleX), m_scaleY(scaleY), m_angle(angle), m_clamp(clamp)
{

}

QQuickFramebufferObject::Renderer *TransformObject::createRenderer() const {
    return new TransformRenderer(m_resolution, m_bpc);
}

unsigned int &TransformObject::texture() {
    return m_texture;
}

void TransformObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int TransformObject::maskTexture() {
    return m_maskTexture;
}

void TransformObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    transformedTex = true;
    update();
}

unsigned int TransformObject::sourceTexture() {
    return m_sourceTexture;
}

void TransformObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    transformedTex = true;
    update();
}

void TransformObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float TransformObject::translateX() {
    return m_translateX;
}

void TransformObject::setTranslateX(float transX) {
    m_translateX = transX;
    transformedTex = true;
    update();
}

float TransformObject::translateY() {
    return m_translateY;
}

void TransformObject::setTranslateY(float transY) {
    m_translateY = transY;
    transformedTex = true;
    update();
}

float TransformObject::scaleX() {
    return m_scaleX;
}

void TransformObject::setScaleX(float scaleX) {
    m_scaleX = scaleX;
    transformedTex = true;
    update();
}

float TransformObject::scaleY() {
    return m_scaleY;
}

void TransformObject::setScaleY(float scaleY) {
    m_scaleY = scaleY;
    transformedTex = true;
    update();
}

int TransformObject::rotation() {
    return m_angle;
}

void TransformObject::setRotation(int angle) {
    m_angle = angle;
    transformedTex = true;
    update();
}

bool TransformObject::clampCoords() {
    return m_clamp;
}

void TransformObject::setClampCoords(bool clamp) {
    m_clamp = clamp;
    transformedTex = true;
    update();
}

QVector2D TransformObject::resolution() {
    return m_resolution;
}

void TransformObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint TransformObject::bpc() {
    return m_bpc;
}

void TransformObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

TransformRenderer::TransformRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc) {
    initializeOpenGLFunctions();
    transformShader = new QOpenGLShaderProgram();
    transformShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    transformShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/transform.frag");
    transformShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    transformShader->bind();
    transformShader->setUniformValue(transformShader->uniformLocation("transTexture"), 0);
    transformShader->setUniformValue(transformShader->uniformLocation("maskTexture"), 1);
    transformShader->release();
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
    glGenFramebuffers(1, &transformFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, transformFBO);
    glGenTextures(1, &m_transformedTexture);
    glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_transformedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TransformRenderer::~TransformRenderer() {
    delete transformShader;
    delete checkerShader;
    delete textureShader;
    glDeleteTextures(1, &m_transformedTexture);
    glDeleteFramebuffers(1, &transformFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *TransformRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void TransformRenderer::synchronize(QQuickFramebufferObject *item) {
    TransformObject *transformItem = static_cast<TransformObject*>(item);
    if(transformItem->resUpdated) {
        transformItem->resUpdated;
        m_resolution = transformItem->resolution();
        updateTexResolution();
    }
    if(transformItem->bpcUpdated) {
        transformItem->bpcUpdated = false;
        m_bpc = transformItem->bpc();
        updateTexResolution();
        transformateTexture();
    }
    if(transformItem->transformedTex) {
        transformItem->transformedTex = false;
        m_sourceTexture = transformItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = transformItem->maskTexture();
            transformShader->bind();
            transformShader->setUniformValue(transformShader->uniformLocation("translate"), QVector2D(transformItem->translateX(), transformItem->translateY()));
            transformShader->setUniformValue(transformShader->uniformLocation("scale"), QVector2D(transformItem->scaleX(), transformItem->scaleY()));
            transformShader->setUniformValue(transformShader->uniformLocation("angle"), transformItem->rotation());
            transformShader->setUniformValue(transformShader->uniformLocation("clampTrans"), transformItem->clampCoords());
            transformShader->setUniformValue(transformShader->uniformLocation("useMask"), maskTexture);
            transformShader->release();
            transformateTexture();
            transformItem->setTexture(m_transformedTexture);
            transformItem->updatePreview(m_transformedTexture);
        }
    }
    if(transformItem->texSaving) {
        transformItem->texSaving = false;
        saveTexture(transformItem->saveName);
    }
}

void TransformRenderer::render() {
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

    if (m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void TransformRenderer::transformateTexture() {
    glBindFramebuffer(GL_FRAMEBUFFER, transformFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f ,0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    transformShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    transformShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void TransformRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TransformRenderer::saveTexture(QString fileName) {
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
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
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
    glDeleteTextures(1, &tex);
    glDeleteFramebuffers(1, &fbo);
}
