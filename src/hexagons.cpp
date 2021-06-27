#include "hexagons.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

HexagonsObject::HexagonsObject(QQuickItem *parent, QVector2D resolution, GLint bpc, int columns, int rows,
                               float size,float smooth, float mask, int seed):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_columns(columns), m_rows(rows),
    m_size(size), m_smooth(smooth), m_mask(mask), m_seed(seed)
{

}

QQuickFramebufferObject::Renderer *HexagonsObject::createRenderer() const {
    return new HexagonsRenderer(m_resolution, m_bpc);
}

unsigned int &HexagonsObject::texture() {
    return m_texture;
}

void HexagonsObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int HexagonsObject::maskTexture() {
    return m_maskTexture;
}

void HexagonsObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedTex = true;
    update();
}

void HexagonsObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

int HexagonsObject::columns() {
    return m_columns;
}

void HexagonsObject::setColumns(int columns) {
    m_columns = columns;
    generatedTex = true;
    update();
}

int HexagonsObject::rows() {
    return m_rows;
}

void HexagonsObject::setRows(int rows) {
    m_rows = rows;
    generatedTex = true;
    update();
}

float HexagonsObject::hexSize() {
    return m_size;
}

void HexagonsObject::setHexSize(float size) {
    m_size = size;
    generatedTex = true;
    update();
}

float HexagonsObject::hexSmooth() {
    return m_smooth;
}

void HexagonsObject::setHexSmooth(float smooth) {
    m_smooth = smooth;
    generatedTex = true;
    update();
}

float HexagonsObject::mask() {
    return m_mask;
}

void HexagonsObject::setMask(float mask) {
    m_mask = mask;
    generatedTex = true;
    update();
}

int HexagonsObject::seed() {
    return m_seed;
}

void HexagonsObject::setSeed(int seed) {
    m_seed = seed;
    generatedTex = true;
    update();
}

QVector2D HexagonsObject::resolution() {
    return m_resolution;
}

void HexagonsObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint HexagonsObject::bpc() {
    return m_bpc;
}

void HexagonsObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

HexagonsRenderer::HexagonsRenderer(QVector2D res, GLint bpc): m_resolution(res) {
    initializeOpenGLFunctions();

    hexagonsShader = new QOpenGLShaderProgram();
    hexagonsShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    hexagonsShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/hexagons.frag");
    hexagonsShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    hexagonsShader->bind();
    hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("maskTexture"), 0);
    hexagonsShader->release();
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
    glGenFramebuffers(1, &hexagonsFBO);
    glGenTextures(1, &m_hexagonsTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, hexagonsFBO);
    glBindTexture(GL_TEXTURE_2D, m_hexagonsTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hexagonsTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

HexagonsRenderer::~HexagonsRenderer() {
    delete hexagonsShader;
    delete textureShader;
    delete checkerShader;
    glDeleteTextures(1, &m_hexagonsTexture);
    glDeleteFramebuffers(1, &hexagonsFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *HexagonsRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void HexagonsRenderer::synchronize(QQuickFramebufferObject *item) {
    HexagonsObject *hexagonsItem = static_cast<HexagonsObject*>(item);
    if(hexagonsItem->generatedTex) {
        hexagonsItem->generatedTex = false;
        m_maskTexture = hexagonsItem->maskTexture();
        hexagonsShader->bind();
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("columns"), hexagonsItem->columns());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("rows"), hexagonsItem->rows());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("size"), hexagonsItem->hexSize());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("hexSmooth"), hexagonsItem->hexSmooth());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("maskStrength"), hexagonsItem->mask());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("seed"), hexagonsItem->seed());
        hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("useMask"), m_maskTexture);
        createHexagons();
        hexagonsItem->setTexture(m_hexagonsTexture);
        hexagonsItem->updatePreview(m_hexagonsTexture);
    }
    if(hexagonsItem->resUpdated) {
        hexagonsItem->resUpdated = false;
        m_resolution = hexagonsItem->resolution();
        updateTexResolution();
        if(!m_maskTexture) {
            createHexagons();
            hexagonsItem->setTexture(m_hexagonsTexture);
        }
    }
    if(hexagonsItem->bpcUpdated) {
        hexagonsItem->bpcUpdated = false;
        m_bpc = hexagonsItem->bpc();
        updateTexResolution();
        createHexagons();
        hexagonsItem->setTexture(m_hexagonsTexture);
    }
    if(hexagonsItem->texSaving) {
        hexagonsItem->texSaving = false;
        saveTexture(hexagonsItem->saveName);
    }
}

void HexagonsRenderer::render() {
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
    glBindTexture(GL_TEXTURE_2D, m_hexagonsTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);
    glFlush();
}

void HexagonsRenderer::createHexagons() {
    glBindFramebuffer(GL_FRAMEBUFFER, hexagonsFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    hexagonsShader->bind();
    hexagonsShader->setUniformValue(hexagonsShader->uniformLocation("res"), m_resolution);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    hexagonsShader->release();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void HexagonsRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_hexagonsTexture);
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

void HexagonsRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_hexagonsTexture);
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
