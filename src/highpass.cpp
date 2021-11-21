#include "highpass.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>
#include "FreeImage.h"

HighPassObject::HighPassObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float radius,
                               float contrast): QQuickFramebufferObject(parent), m_resolution(resolution),
    m_bpc(bpc), m_radius(radius), m_contrast(contrast)
{

}

QQuickFramebufferObject::Renderer *HighPassObject::createRenderer() const
{
    return new HighPassRenderer(m_resolution, m_bpc);
}

unsigned int &HighPassObject::texture()
{
    return m_texture;
}

void HighPassObject::setTexture(unsigned int texture)
{
    m_texture = texture;
    textureChanged();
}

unsigned int HighPassObject::sourceTexture() {
    return m_sourceTexture;
}

void HighPassObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    highPassTex = true;
}

unsigned int HighPassObject::maskTexture() {
    return m_maskTexture;
}

void HighPassObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    contrastTex = true;
}

void HighPassObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

float HighPassObject::radius()
{
    return m_radius;
}

void HighPassObject::setRadius(float radius)
{
    if(m_radius == radius) return;
    m_radius = radius;
    highPassTex = true;
}

float HighPassObject::contrast()
{
    return m_contrast;
}

void HighPassObject::setContrast(float contrast)
{
    if(m_contrast == contrast) return;
    m_contrast = contrast;
    contrastTex = true;
}

QVector2D HighPassObject::resolution() {
    return m_resolution;
}

void HighPassObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint HighPassObject::bpc() {
    return m_bpc;
}

void HighPassObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

HighPassRenderer::HighPassRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc)
{
    initializeOpenGLFunctions();

    blurShader = new QOpenGLShaderProgram();
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blurgauss.frag");
    blurShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    highpassShader = new QOpenGLShaderProgram();
    highpassShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    highpassShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/highpass.frag");
    highpassShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    blurShader->bind();
    blurShader->setUniformValue(blurShader->uniformLocation("sourceTexture"), 0);
    blurShader->setUniformValue(blurShader->uniformLocation("maskTexture"), 1);
    blurShader->release();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    textureShader->release();
    highpassShader->bind();
    highpassShader->setUniformValue(highpassShader->uniformLocation("sourceTexture"), 0);
    highpassShader->setUniformValue(highpassShader->uniformLocation("blurTexture"), 1);
    highpassShader->setUniformValue(highpassShader->uniformLocation("maskTexture"), 2);
    highpassShader->setUniformValue(highpassShader->uniformLocation("useMask"), false);
    highpassShader->release();

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
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &highpassTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, highpassTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, highpassTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

HighPassRenderer::~HighPassRenderer() {
    delete textureShader;
    delete checkerShader;
    delete blurShader;
    delete highpassShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(2, &pingpongBuffer[0]);
    glDeleteTextures(1, &highpassTexture);
    glDeleteFramebuffers(2, &pingpongFBO[0]);
    glDeleteFramebuffers(1, &FBO);
}

QOpenGLFramebufferObject *HighPassRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void HighPassRenderer::synchronize(QQuickFramebufferObject *item) {
    HighPassObject *highPassItem = static_cast<HighPassObject*>(item);

    if(highPassItem->resUpdated) {
        highPassItem->resUpdated = false;
        m_resolution = highPassItem->resolution();
        updateTexResolution();
    }
    if(highPassItem->highPassTex || highPassItem->bpcUpdated) {
        if(highPassItem->highPassTex) {
            highPassItem->highPassTex = false;
            m_sourceTexture = highPassItem->sourceTexture();
            if(m_sourceTexture) {
                blurShader->bind();
                blurShader->setUniformValue(blurShader->uniformLocation("radius"), highPassItem->radius());
                blurShader->release();
            }
        }
        if(highPassItem->bpcUpdated) {
            highPassItem->bpcUpdated = false;
            m_bpc = highPassItem->bpc();
            updateTexResolution();
        }
        if(m_sourceTexture) {
            blur();
            highPass();
            highPassItem->setTexture(highpassTexture);
            highPassItem->updatePreview(highpassTexture);
        }
    }
    if(highPassItem->contrastTex) {
        highPassItem->contrastTex = false;
        m_maskTexture = highPassItem->maskTexture();
        highpassShader->bind();
        highpassShader->setUniformValue(highpassShader->uniformLocation("contrast"), highPassItem->contrast());
        highpassShader->setUniformValue(highpassShader->uniformLocation("useMask"), m_maskTexture);
        highpassShader->release();
        highPass();
        highPassItem->setTexture(highpassTexture);
        highPassItem->updatePreview(highpassTexture);
    }
    if(highPassItem->texSaving) {
        highPassItem->texSaving = false;
        saveTexture(highPassItem->saveName);
    }
}

void HighPassRenderer::render() {
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
        textureShader->setUniformValue(textureShader->uniformLocation("lod"), 2.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, highpassTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void HighPassRenderer::blur()
{
    bool first_iteration = true;
    QVector2D dir = QVector2D(1, 0);
    int amount = 2;
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i%2]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D, first_iteration ? m_sourceTexture : pingpongBuffer[1 - (i%2)]
        );

        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(textureVAO);
        blurShader->bind();
        blurShader->setUniformValue(blurShader->uniformLocation("useMask"), false);
        blurShader->setUniformValue(blurShader->uniformLocation("direction"), dir);
        dir = QVector2D(1, 1) - dir;
        blurShader->setUniformValue(blurShader->uniformLocation("resolution"), m_resolution);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        blurShader->release();
        glBindVertexArray(0);
        if (first_iteration)
            first_iteration = false;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void HighPassRenderer::highPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    highpassShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    highpassShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, highpassTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFlush();
    glFinish();
}

void HighPassRenderer::updateTexResolution() {
    if(m_bpc == GL_RGBA8) {
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, highpassTexture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if(m_bpc == GL_RGBA16) {
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, highpassTexture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void HighPassRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, highpassTexture);
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
