#include "bricks.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

BricksObject::BricksObject(QQuickItem *parent, QVector2D resolution, GLint bpc, int columns, int rows,
                           float offset, float width, float height, float smoothX, float smoothY,
                           float mask, int seed): QQuickFramebufferObject (parent), m_resolution(resolution),
    m_bpc(bpc), m_columns(columns), m_rows(rows), m_offset(offset), m_width(width), m_height(height),
    m_mask(mask), m_smoothX(smoothX), m_smoothY(smoothY), m_seed(seed)
{

}

QQuickFramebufferObject::Renderer *BricksObject::createRenderer() const {
    return new BricksRenderer(m_resolution, m_bpc);
}

unsigned int &BricksObject::texture() {
    return m_texture;
}

void BricksObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

void BricksObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

unsigned int BricksObject::maskTexture() {
    return m_maskTexture;
}

void BricksObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedTex = true;
    update();
}

int BricksObject::columns() {
    return m_columns;
}

void BricksObject::setColumns(int columns) {
    m_columns = columns;
    generatedTex = true;
    update();
}

int BricksObject::rows() {
    return m_rows;
}

void BricksObject::setRows(int rows) {
    m_rows = rows;
    generatedTex = true;
    update();
}

float BricksObject::offset() {
    return m_offset;
}

void BricksObject::setOffset(float offset) {
    m_offset = offset;
    generatedTex = true;
    update();
}

float BricksObject::bricksWidth() {
    return m_width;
}

void BricksObject::setBricksWidth(float width) {
    m_width = width;
    generatedTex = true;
    update();
}

float BricksObject::bricksHeight() {
    return m_height;
}

void BricksObject::setBricksHeight(float height) {
    m_height = height;
    generatedTex = true;
    update();
}

float BricksObject::mask() {
    return m_mask;
}

void BricksObject::setMask(float mask) {
    m_mask = mask;
    generatedTex = true;
    update();
}

float BricksObject::smoothX() {
    return m_smoothX;
}

void BricksObject::setSmoothX(float smooth) {
    m_smoothX = smooth;
    generatedTex = true;
    update();
}

float BricksObject::smoothY() {
    return m_smoothY;
}

void BricksObject::setSmoothY(float smooth) {
    m_smoothY = smooth;
    generatedTex = true;
    update();
}

int BricksObject::seed() {
    return m_seed;
}

void BricksObject::setSeed(int seed) {
    m_seed = seed;
    generatedTex = true;
    update();
}

QVector2D BricksObject::resolution() {
    return m_resolution;
}

void BricksObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}
GLint BricksObject::bpc() {
    return m_bpc;
}

void BricksObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

BricksRenderer::BricksRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();
    bricksShader = new QOpenGLShaderProgram();
    bricksShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    bricksShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/bricks.frag");
    bricksShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    bricksShader->bind();
    bricksShader->setUniformValue(bricksShader->uniformLocation("maskTexture"), 0);
    bricksShader->release();
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
    glGenFramebuffers(1, &bricksFBO);
    glGenTextures(1, &m_bricksTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, bricksFBO);
    glBindTexture(GL_TEXTURE_2D, m_bricksTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bricksTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BricksRenderer::~BricksRenderer() {
    delete bricksShader;
    delete textureShader;
    delete checkerShader;
    glDeleteTextures(1, &m_bricksTexture);
    glDeleteFramebuffers(1, &bricksFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *BricksRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void BricksRenderer::synchronize(QQuickFramebufferObject *item) {
    BricksObject *bricksItem = static_cast<BricksObject*>(item);
    if(bricksItem->generatedTex) {
        bricksItem->generatedTex = false;
        m_maskTexture = bricksItem->maskTexture();
        bricksShader->bind();
        bricksShader->setUniformValue(bricksShader->uniformLocation("columns"), bricksItem->columns());
        bricksShader->setUniformValue(bricksShader->uniformLocation("rows"), bricksItem->rows());
        bricksShader->setUniformValue(bricksShader->uniformLocation("offset"), bricksItem->offset());
        bricksShader->setUniformValue(bricksShader->uniformLocation("width"), bricksItem->bricksWidth());
        bricksShader->setUniformValue(bricksShader->uniformLocation("height"), bricksItem->bricksHeight());
        bricksShader->setUniformValue(bricksShader->uniformLocation("maskStrength"), bricksItem->mask());
        bricksShader->setUniformValue(bricksShader->uniformLocation("smoothX"), bricksItem->smoothX());
        bricksShader->setUniformValue(bricksShader->uniformLocation("smoothY"), bricksItem->smoothY());
        bricksShader->setUniformValue(bricksShader->uniformLocation("seed"), bricksItem->seed());
        bricksShader->setUniformValue(bricksShader->uniformLocation("useMask"), m_maskTexture);
        createBricks();
        bricksItem->setTexture(m_bricksTexture);
        bricksItem->updatePreview(m_bricksTexture);
    }
    if(bricksItem->resUpdated) {
        bricksItem->resUpdated = false;
        m_resolution = bricksItem->resolution();
        updateTexResolution();
        if(!m_maskTexture) {
            createBricks();
            bricksItem->setTexture(m_bricksTexture);
        }
    }
    if(bricksItem->bpcUpdated) {
        bricksItem->bpcUpdated = false;
        m_bpc = bricksItem->bpc();
        updateTexResolution();
        createBricks();
        bricksItem->setTexture(m_bricksTexture);
    }
    if(bricksItem->texSaving) {
        bricksItem->texSaving = false;
        saveTexture(bricksItem->saveName);
    }
}

void BricksRenderer::render() {
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

    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_bricksTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);
    glFlush();
}

void BricksRenderer::createBricks() {
    glBindFramebuffer(GL_FRAMEBUFFER, bricksFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    bricksShader->bind();
    bricksShader->setUniformValue(bricksShader->uniformLocation("res"), m_resolution);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    bricksShader->release();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void BricksRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_bricksTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BricksRenderer::saveTexture(QString fileName) {
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
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_bricksTexture);
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
