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

#include "tile.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>
#include "FreeImage.h"

TileObject::TileObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float offsetX, float offsetY,
                       int columns, int rows, float scale, float scaleX, float scaleY, int rotation,
                       float randPosition, float randRotation, float randScale, float maskStrength,
                       int inputsCount, int seed, bool keepProportion, bool useAlpha, bool depthMask):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_offsetX(offsetX),
    m_offsetY(offsetY), m_columns(columns), m_rows(rows), m_scaleX(scaleX), m_scaleY(scaleY),
    m_rotationAngle(rotation), m_randPosition(randPosition), m_randRotation(randRotation),
    m_randScale(randScale), m_maskStrength(maskStrength), m_inputsCount(inputsCount), m_seed(seed),
    m_scale(scale), m_keepProportion(keepProportion), m_useAlpha(useAlpha), m_depthMask(depthMask)
{

}

QQuickFramebufferObject::Renderer *TileObject::createRenderer() const {
    return new TileRenderer(m_resolution, m_bpc);
}

unsigned int &TileObject::texture() {
    return m_texture;
}

void TileObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int TileObject::maskTexture() {
    return m_maskTexture;
}

void TileObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    tiledTex = true;
}

unsigned int TileObject::sourceTexture() {
    return m_sourceTexture;
}

void TileObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    tiledTex = true;
}

unsigned int TileObject::tile1() {
    return m_tile1;
}

void TileObject::setTile1(unsigned int texture) {
    m_tile1 = texture;
    tiledTex = true;
}

unsigned int TileObject::tile2() {
    return m_tile2;
}

void TileObject::setTile2(unsigned int texture) {
    m_tile2 = texture;
    tiledTex = true;
}

unsigned int TileObject::tile3() {
    return m_tile3;
}

void TileObject::setTile3(unsigned int texture) {
    m_tile3 = texture;
    tiledTex = true;
}

unsigned int TileObject::tile4() {
    return m_tile4;
}

void TileObject::setTile4(unsigned int texture) {
    m_tile4 = texture;
    tiledTex = true;
}

unsigned int TileObject::tile5() {
    return m_tile5;
}

void TileObject::setTile5(unsigned int texture) {
    m_tile5 = texture;
    tiledTex = true;
}

void TileObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float TileObject::offsetX() {
    return m_offsetX;
}

void TileObject::setOffsetX(float offset) {
    if(m_offsetX == offset) return;
    m_offsetX = offset;
    tiledTex = true;
}

float TileObject::offsetY() {
    return m_offsetY;
}

void TileObject::setOffsetY(float offset) {
    if(m_offsetY == offset) return;
    m_offsetY = offset;
    tiledTex = true;
}

int TileObject::columns() {
    return m_columns;
}

void TileObject::setColumns(int columns) {
    if(m_columns == columns) return;
    m_columns = columns;
    tiledTex = true;
}

int TileObject::rows() {
    return m_rows;
}

void TileObject::setRows(int rows) {
    if(m_rows == rows) return;
    m_rows = rows;
    tiledTex = true;
}

float TileObject::scaleX() {
    return m_scaleX;
}

void TileObject::setScaleX(float scale) {
    if(m_scaleX == scale) return;
    m_scaleX = scale;
    tiledTex = true;
}

float TileObject::scaleY() {
    return m_scaleY;
}

void TileObject::setScaleY(float scale) {
    if(m_scaleY == scale) return;
    m_scaleY = scale;
    tiledTex = true;
}

int TileObject::rotationAngle() {
    return m_rotationAngle;
}

void TileObject::setRotationAngle(int angle) {
    if(m_rotationAngle == angle) return;
    m_rotationAngle = angle;
    tiledTex = true;
}

float TileObject::randPosition() {
    return m_randPosition;
}

void TileObject::setRandPosition(float rand) {
    if(m_randPosition == rand) return;
    m_randPosition = rand;
    tiledTex = true;
}

float TileObject::randRotation() {
    return m_randRotation;
}

void TileObject::setRandRotation(float rand) {
    if(m_randRotation == rand) return;
    m_randRotation = rand;
    tiledTex = true;
}

float TileObject::randScale() {
    return m_randScale;
}

void TileObject::setRandScale(float rand) {
    if(m_randScale == rand) return;
    m_randScale = rand;
    tiledTex = true;
}

float TileObject::maskStrength() {
    return m_maskStrength;
}

void TileObject::setMaskStrength(float mask) {
    if(m_maskStrength == mask) return;
    m_maskStrength = mask;
    tiledTex = true;
}

int TileObject::inputsCount() {
    return m_inputsCount;
}

void TileObject::setInputsCount(int count) {
    if(m_inputsCount == count) return;
    m_inputsCount = count;
    tiledTex = true;
}

int TileObject::seed() {
    return m_seed;
}

void TileObject::setSeed(int seed) {
    if(m_seed == seed) return;
    m_seed = seed;
    randUpdated = true;
}

float TileObject::tileScale() {
    return m_scale;
}

void TileObject::setTileScale(float scale) {
    if(m_scale == scale) return;
    m_scale = scale;
    tiledTex = true;
}

bool TileObject::keepProportion() {
    return m_keepProportion;
}

void TileObject::setKeepProportion(bool keep) {
    if(m_keepProportion == keep) return;
    m_keepProportion = keep;
    tiledTex = true;
}

bool TileObject::useAlpha() {
    return m_useAlpha;
}

void TileObject::setUseAlpha(bool use) {
    if(m_useAlpha == use) return;
    m_useAlpha = use;
    tiledTex = true;
}

bool TileObject::depthMask() {
    return m_depthMask;
}

void TileObject::setDepthMask(bool depth) {
    if(m_depthMask == depth) return;
    m_depthMask = depth;
    tiledTex = true;
}

QVector2D TileObject::resolution() {
    return m_resolution;
}

void TileObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint TileObject::bpc() {
    return m_bpc;
}

void TileObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

TileRenderer::TileRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();

    tileShader = new QOpenGLShaderProgram();
    tileShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    tileShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tile.frag");
    tileShader->link();

    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    tileShader->bind();
    tileShader->setUniformValue(tileShader->uniformLocation("textureTile"), 0);
    tileShader->setUniformValue(tileShader->uniformLocation("maskTexture"), 1);
    tileShader->setUniformValue(tileShader->uniformLocation("randomTexture"), 2);
    tileShader->setUniformValue(tileShader->uniformLocation("tile1"), 3);
    tileShader->setUniformValue(tileShader->uniformLocation("tile2"), 4);
    tileShader->setUniformValue(tileShader->uniformLocation("tile3"), 5);
    tileShader->setUniformValue(tileShader->uniformLocation("tile4"), 6);
    tileShader->setUniformValue(tileShader->uniformLocation("tile5"), 7);
    tileShader->release();

    randomShader->bind();
    randomShader->setUniformValue(randomShader->uniformLocation("seed"), 1);
    randomShader->release();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 2.0f);
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

    glGenFramebuffers(1, &tileFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
    glGenTextures(1, &m_tiledTexture);
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tiledTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &randomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glGenTextures(1, &m_randomTexture);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_randomTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TileRenderer::~TileRenderer() {
    delete tileShader;
    delete checkerShader;
    delete textureShader;
    delete randomShader;
    glDeleteTextures(1, &m_tiledTexture);
    glDeleteTextures(1, &m_randomTexture);
    glDeleteFramebuffers(1, &tileFBO);
    glDeleteFramebuffers(1, &randomFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *TileRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void TileRenderer::synchronize(QQuickFramebufferObject *item) {
    TileObject *tileItem = static_cast<TileObject*>(item);
    if(tileItem->resUpdated) {
        tileItem->resUpdated = false;
        m_resolution = tileItem->resolution();
        updateTexResolution();
    }
    if(tileItem->tiledTex || tileItem->bpcUpdated) {
        if(tileItem->bpcUpdated) {
            tileItem->bpcUpdated = false;
            m_bpc = tileItem->bpc();
            updateTexResolution();
        }
        if(tileItem->tiledTex) {
            tileItem->tiledTex = false;
            m_sourceTexture = tileItem->sourceTexture();
            m_tile1 = tileItem->tile1();
            m_tile2 = tileItem->tile2();
            m_tile3 = tileItem->tile3();
            m_tile4 = tileItem->tile4();
            m_tile5 = tileItem->tile5();
            if(m_sourceTexture || m_tile1 || m_tile2 || m_tile3 || m_tile4 || m_tile5) {
                maskTexture = tileItem->maskTexture();
                tileShader->bind();
                tileShader->setUniformValue(tileShader->uniformLocation("offsetX"), tileItem->offsetX());
                tileShader->setUniformValue(tileShader->uniformLocation("offsetY"), tileItem->offsetY());
                tileShader->setUniformValue(tileShader->uniformLocation("columns"), tileItem->columns());
                tileShader->setUniformValue(tileShader->uniformLocation("rows"), tileItem->rows());
                tileShader->setUniformValue(tileShader->uniformLocation("scaleX"), tileItem->scaleX());
                tileShader->setUniformValue(tileShader->uniformLocation("scaleY"), tileItem->scaleY());
                tileShader->setUniformValue(tileShader->uniformLocation("scale"), tileItem->tileScale());
                tileShader->setUniformValue(tileShader->uniformLocation("rotationAngle"), tileItem->rotationAngle());
                tileShader->setUniformValue(tileShader->uniformLocation("randPosition"), tileItem->randPosition());
                tileShader->setUniformValue(tileShader->uniformLocation("randRotation"), tileItem->randRotation());
                tileShader->setUniformValue(tileShader->uniformLocation("randScale"), tileItem->randScale());
                tileShader->setUniformValue(tileShader->uniformLocation("maskStrength"), tileItem->maskStrength());
                tileShader->setUniformValue(tileShader->uniformLocation("inputCount"), tileItem->inputsCount());
                tileShader->setUniformValue(tileShader->uniformLocation("keepProportion"), tileItem->keepProportion());
                tileShader->setUniformValue(tileShader->uniformLocation("useAlpha"), tileItem->useAlpha());
                tileShader->setUniformValue(tileShader->uniformLocation("depthMask"), tileItem->depthMask());
                tileShader->setUniformValue(tileShader->uniformLocation("useMask"), maskTexture);
                tileShader->release();
            }
            else {
                tileItem->setTexture(0);
            }
        }
        if(m_sourceTexture || m_tile1 || m_tile2 || m_tile3 || m_tile4 || m_tile5) {
            createTile();
            tileItem->setTexture(m_tiledTexture);
            tileItem->updatePreview(m_tiledTexture);
        }
    }
    if(tileItem->randUpdated) {
        tileItem->randUpdated = false;
        randomShader->bind();
        randomShader->setUniformValue(randomShader->uniformLocation("seed"), tileItem->seed());
        randomShader->release();
        createRandom();
        createTile();
    }
    if(tileItem->texSaving) {
        tileItem->texSaving = false;
        saveTexture(tileItem->saveName);
    }
}

void TileRenderer::render() {
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

    if(m_sourceTexture || m_tile1 || m_tile2 || m_tile3 || m_tile4 || m_tile5) {
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        textureShader->release();
    }
    glFlush();
}

void TileRenderer::createTile() {
    glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    tileShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_tile1);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_tile2);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_tile3);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_tile4);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_tile5);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);    
    tileShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

void TileRenderer::createRandom() {
    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glViewport(0, 0, 512, 512);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    randomShader->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    randomShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TileRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TileRenderer::saveTexture(QString fileName) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    textureShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    textureShader->release();

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
