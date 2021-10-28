#include "floodfilltogradient.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

FloodFillToGradientObject::FloodFillToGradientObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                                     int rotation, float randomizing, int seed):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_rotation(rotation),
    m_randomizing(randomizing), m_seed(seed)
{

}

QQuickFramebufferObject::Renderer *FloodFillToGradientObject::createRenderer() const {
    return new FloodFillToGradientRenderer(m_resolution, m_bpc);
}

unsigned int &FloodFillToGradientObject::texture() {
    return m_texture;
}

void FloodFillToGradientObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int FloodFillToGradientObject::sourceTexture() {
    return m_sourceTexture;
}

void FloodFillToGradientObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    toGradientTex = true;
}

void FloodFillToGradientObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

int FloodFillToGradientObject::rotation() {
    return m_rotation;
}

void FloodFillToGradientObject::setRotation(int angle) {
    if(m_rotation == angle) return;
    m_rotation = angle;
    toGradientTex = true;
}

float FloodFillToGradientObject::randomizing() {
    return m_randomizing;
}

void FloodFillToGradientObject::setRandomizing(float rand) {
    if(m_randomizing == rand) return;
    m_randomizing = rand;
    toGradientTex = true;
}

int FloodFillToGradientObject::seed() {
    return m_seed;
}

void FloodFillToGradientObject::setSeed(int seed) {
    if(m_seed == seed) return;
    m_seed = seed;
    randUpdate = true;
}

QVector2D FloodFillToGradientObject::resolution() {
    return m_resolution;
}

void FloodFillToGradientObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdate = true;
    update();
}

GLint FloodFillToGradientObject::bpc() {
    return m_bpc;
}

void FloodFillToGradientObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

FloodFillToGradientRenderer::FloodFillToGradientRenderer(QVector2D res, GLint bpc): m_resolution(res),
                                                                                    m_bpc(bpc) {
    initializeOpenGLFunctions();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    gradientShader = new QOpenGLShaderProgram();
    gradientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    gradientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/floodfilltogradient.frag");
    gradientShader->link();

    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();

    gradientShader->bind();
    gradientShader->setUniformValue(gradientShader->uniformLocation("floodFillTexture"), 0);
    gradientShader->setUniformValue(gradientShader->uniformLocation("randomTexture"), 1);
    gradientShader->release();

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
    glGenTextures(1, &m_gradientTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, m_gradientTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gradientTexture, 0
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

FloodFillToGradientRenderer::~FloodFillToGradientRenderer() {
    delete textureShader;
    delete randomShader;
    delete gradientShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(1, &m_gradientTexture);
    glDeleteTextures(1, &m_randomTexture);
    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &randomFBO);
}

QOpenGLFramebufferObject *FloodFillToGradientRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void FloodFillToGradientRenderer::synchronize(QQuickFramebufferObject *item) {
    FloodFillToGradientObject *ffToGradientItem = static_cast<FloodFillToGradientObject*>(item);

    if(ffToGradientItem->resUpdate) {
        ffToGradientItem->resUpdate = false;
        m_resolution = ffToGradientItem->resolution();
        updateTexResolution();
    }
    if(ffToGradientItem->toGradientTex || ffToGradientItem->bpcUpdated) {
        if(ffToGradientItem->bpcUpdated) {
            ffToGradientItem->bpcUpdated = false;
            m_bpc = ffToGradientItem->bpc();
            updateTexResolution();
        }
        if(ffToGradientItem->toGradientTex) {
            ffToGradientItem->toGradientTex = false;
            m_sourceTexture = ffToGradientItem->sourceTexture();
            if(m_sourceTexture) {
                gradientShader->bind();
                gradientShader->setUniformValue(gradientShader->uniformLocation("rotation"), ffToGradientItem->rotation());
                gradientShader->setUniformValue(gradientShader->uniformLocation("randomizing"), ffToGradientItem->randomizing());
                gradientShader->release();
            }
        }
        if(m_sourceTexture) {
            toGradient();
            ffToGradientItem->setTexture(m_gradientTexture);
            ffToGradientItem->updatePreview(m_gradientTexture);
        }
    }
    if(ffToGradientItem->randUpdate) {
        ffToGradientItem->randUpdate = false;
        randomShader->bind();
        randomShader->setUniformValue(randomShader->uniformLocation("seed"), ffToGradientItem->seed());
        randomShader->release();
        createRandom();
        toGradient();
        ffToGradientItem->setTexture(m_gradientTexture);
        ffToGradientItem->updatePreview(m_gradientTexture);
    }
    if(ffToGradientItem->texSaving) {
        ffToGradientItem->texSaving = false;
        saveTexture(ffToGradientItem->saveName);
    }
}

void FloodFillToGradientRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_gradientTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void FloodFillToGradientRenderer::toGradient() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    gradientShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    gradientShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glBindTexture(GL_TEXTURE_2D, m_gradientTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

void FloodFillToGradientRenderer::createRandom() {
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

void FloodFillToGradientRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_gradientTexture);
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

void FloodFillToGradientRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_gradientTexture);
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
