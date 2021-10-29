#include "floodfilltograyscale.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

FloodFillToGrayscaleObject::FloodFillToGrayscaleObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                                       int seed): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_bpc(bpc), m_seed(seed)
{

}

QQuickFramebufferObject::Renderer *FloodFillToGrayscaleObject::createRenderer() const {
    return new FloodFillToGrayscaleRenderer(m_resolution, m_bpc);
}

unsigned int &FloodFillToGrayscaleObject::texture() {
    return m_texture;
}

void FloodFillToGrayscaleObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int FloodFillToGrayscaleObject::sourceTexture() {
    return m_sourceTexture;
}

void FloodFillToGrayscaleObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    toGrayscaleTex = true;
}

void FloodFillToGrayscaleObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

int FloodFillToGrayscaleObject::seed() {
    return m_seed;
}

void FloodFillToGrayscaleObject::setSeed(int seed) {
    if(m_seed == seed) return;
    m_seed = seed;
    randUpdate = true;
}

QVector2D FloodFillToGrayscaleObject::resolution() {
    return m_resolution;
}

void FloodFillToGrayscaleObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdate = true;
    update();
}

GLint FloodFillToGrayscaleObject::bpc() {
    return m_bpc;
}

void FloodFillToGrayscaleObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

FloodFillToGrayscaleRenderer::FloodFillToGrayscaleRenderer(QVector2D resolution, GLint bpc):
    m_resolution(resolution), m_bpc(bpc)
{
    initializeOpenGLFunctions();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    grayscaleShader = new QOpenGLShaderProgram();
    grayscaleShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    grayscaleShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/floodfilltograyscale.frag");
    grayscaleShader->link();

    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();

    grayscaleShader->bind();
    grayscaleShader->setUniformValue(grayscaleShader->uniformLocation("floodFillTexture"), 0);
    grayscaleShader->setUniformValue(grayscaleShader->uniformLocation("randomTexture"), 1);
    grayscaleShader->release();

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

    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &m_grayscaleTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, m_grayscaleTexture);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_grayscaleTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &randomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glGenTextures(1, &m_randomTexture);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_randomTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    createRandom();
}

FloodFillToGrayscaleRenderer::~FloodFillToGrayscaleRenderer() {
    delete textureShader;
    delete randomShader;
    delete grayscaleShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(1, &m_randomTexture);
    glDeleteTextures(1, &m_grayscaleTexture);
    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &randomFBO);
}

QOpenGLFramebufferObject *FloodFillToGrayscaleRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void FloodFillToGrayscaleRenderer::synchronize(QQuickFramebufferObject *item) {
    FloodFillToGrayscaleObject *ffToGrayscaleItem = static_cast<FloodFillToGrayscaleObject*>(item);

    if(ffToGrayscaleItem->resUpdate) {
        ffToGrayscaleItem->resUpdate = false;
        m_resolution = ffToGrayscaleItem->resolution();
        updateTexResolution();
    }
    if(ffToGrayscaleItem->toGrayscaleTex || ffToGrayscaleItem->bpcUpdated) {
        if(ffToGrayscaleItem->bpcUpdated) {
            ffToGrayscaleItem->bpcUpdated = false;
            m_bpc = ffToGrayscaleItem->bpc();
            updateTexResolution();
        }
        if(ffToGrayscaleItem->toGrayscaleTex) {
            ffToGrayscaleItem->toGrayscaleTex = false;
            m_sourceTexture = ffToGrayscaleItem->sourceTexture();
        }
        if(m_sourceTexture) {
            toGrayscale();
            ffToGrayscaleItem->updatePreview(m_grayscaleTexture);
            ffToGrayscaleItem->setTexture(m_grayscaleTexture);
        }
    }
    if(ffToGrayscaleItem->randUpdate) {
        ffToGrayscaleItem->randUpdate = false;
        randomShader->bind();
        randomShader->setUniformValue(randomShader->uniformLocation("seed"), ffToGrayscaleItem->seed());
        randomShader->release();
        createRandom();
        toGrayscale();
        ffToGrayscaleItem->updatePreview(m_grayscaleTexture);
        ffToGrayscaleItem->setTexture(m_grayscaleTexture);
    }
    if(ffToGrayscaleItem->texSaving) {
        ffToGrayscaleItem->texSaving = false;
        saveTexture(ffToGrayscaleItem->saveName);
    }
}

void FloodFillToGrayscaleRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        textureShader->setUniformValue(textureShader->uniformLocation("lod"), 2.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_grayscaleTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void FloodFillToGrayscaleRenderer::toGrayscale() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    grayscaleShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    grayscaleShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glBindTexture(GL_TEXTURE_2D, m_grayscaleTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

void FloodFillToGrayscaleRenderer::createRandom() {
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

void FloodFillToGrayscaleRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_grayscaleTexture);
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
}

void FloodFillToGrayscaleRenderer::saveTexture(QString fileName) {
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
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_grayscaleTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}
