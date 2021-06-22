#include "slopeblur.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"


SlopeBlurObject::SlopeBlurObject(QQuickItem *parent, QVector2D resolution, GLint bpc, int mode,
                                 float intensity, int samples): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_bpc(bpc), m_mode(mode), m_intensity(intensity), m_samples(samples)
{

}

QQuickFramebufferObject::Renderer *SlopeBlurObject::createRenderer() const {
    return new SlopeBlurRenderer(m_resolution, m_bpc);
}

unsigned int &SlopeBlurObject::texture() {
    return m_texture;
}

void SlopeBlurObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int SlopeBlurObject::maskTexture() {
    return m_maskTexture;
}

void SlopeBlurObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
}

unsigned int SlopeBlurObject::sourceTexture() {
    return m_sourceTexture;
}

void SlopeBlurObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
}

unsigned int SlopeBlurObject::slopeTexture() {
    return m_slopeTexture;
}

void SlopeBlurObject::setSlopeTexture(unsigned int texture) {
    m_slopeTexture = texture;
}

void SlopeBlurObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

int SlopeBlurObject::mode() {
    return m_mode;
}

void SlopeBlurObject::setMode(int mode) {
    m_mode = mode;
    slopedTex = true;
    update();
}

float SlopeBlurObject::intensity() {
    return m_intensity;
}

void SlopeBlurObject::setIntensity(float intensity) {
    m_intensity = intensity;
    slopedTex = true;
    update();
}

int SlopeBlurObject::samples() {
    return m_samples;
}

void SlopeBlurObject::setSamples(int samples) {
    m_samples = samples;
    slopedTex = true;
    update();
}

QVector2D SlopeBlurObject::resolution() {
    return m_resolution;
}

void SlopeBlurObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint SlopeBlurObject::bpc() {
    return m_bpc;
}

void SlopeBlurObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

SlopeBlurRenderer::SlopeBlurRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc)
{
    initializeOpenGLFunctions();
    slopeBlurShader = new QOpenGLShaderProgram();
    slopeBlurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    slopeBlurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/slopeblur.frag");
    slopeBlurShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    slopeBlurShader->bind();
    slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("sourceTexture"), 0);
    slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("slopeTexture"), 1);
    slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("maskTexture"), 2);
    slopeBlurShader->release();
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
    glGenFramebuffers(1, &slopeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, slopeFBO);
    glGenTextures(1, &m_slopedTexture);
    glBindTexture(GL_TEXTURE_2D, m_slopedTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_slopedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SlopeBlurRenderer::~SlopeBlurRenderer()
{
    delete slopeBlurShader;
    delete checkerShader;
    delete textureShader;
    glDeleteTextures(1, &m_slopedTexture);
    glDeleteFramebuffers(1, &slopeFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *SlopeBlurRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void SlopeBlurRenderer::synchronize(QQuickFramebufferObject *item) {
    SlopeBlurObject *slopeBlurItem = static_cast<SlopeBlurObject*>(item);
    if(slopeBlurItem->slopedTex) {
        slopeBlurItem->slopedTex = false;
        m_sourceTexture = slopeBlurItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = slopeBlurItem->maskTexture();
            m_slopeTexture = slopeBlurItem->slopeTexture();
            slopeBlurShader->bind();
            slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("mode"), slopeBlurItem->mode());
            slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("intensity"), slopeBlurItem->intensity());
            slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("samples"), slopeBlurItem->samples());
            createSlopeBlur();
            slopeBlurShader->release();
            slopeBlurItem->setTexture(m_slopedTexture);
            slopeBlurItem->updatePreview(m_slopedTexture);
        }
    }
    if(slopeBlurItem->resUpdated) {
        slopeBlurItem->resUpdated = false;
        m_resolution = slopeBlurItem->resolution();
        updateTexResolution();
    }
    if(slopeBlurItem->bpcUpdated) {
        slopeBlurItem->bpcUpdated = false;
        m_bpc = slopeBlurItem->bpc();
        updateTexResolution();
        createSlopeBlur();
    }
    if(slopeBlurItem->texSaving) {
        slopeBlurItem->texSaving = false;
        saveTexture(slopeBlurItem->saveName);
    }
}

void SlopeBlurRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_slopedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void SlopeBlurRenderer::createSlopeBlur() {
    glBindFramebuffer(GL_FRAMEBUFFER, slopeFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    slopeBlurShader->bind();
    slopeBlurShader->setUniformValue(slopeBlurShader->uniformLocation("useMask"), maskTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_slopeTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    slopeBlurShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void SlopeBlurRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_slopedTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void SlopeBlurRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_slopedTexture);
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

