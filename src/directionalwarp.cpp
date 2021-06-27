#include "directionalwarp.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

DirectionalWarpObject::DirectionalWarpObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                             float intensity, int angle): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_bpc(bpc), m_intensity(intensity), m_angle(angle)
{

}

QQuickFramebufferObject::Renderer *DirectionalWarpObject::createRenderer() const {
    return new DirectionalWarpRenderer(m_resolution, m_bpc);
}

unsigned int &DirectionalWarpObject::texture() {
    return m_texture;
}

void DirectionalWarpObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int DirectionalWarpObject::sourceTexture() {
    return m_sourceTexture;
}

void DirectionalWarpObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    warpedTex = true;
    update();
}

unsigned int DirectionalWarpObject::warpTexture() {
    return m_warpTexture;
}

void DirectionalWarpObject::setWarpTexture(unsigned int texture) {
    m_warpTexture = texture;
    warpedTex = true;
    update();
}

void DirectionalWarpObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

unsigned int DirectionalWarpObject::maskTexture() {
    return m_maskTexture;
}

void DirectionalWarpObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    warpedTex = true;
    update();
}

float DirectionalWarpObject::intensity() {
    return m_intensity;
}

void DirectionalWarpObject::setIntensity(float intensity) {
    m_intensity = intensity;
    warpedTex = true;
    update();
}

int DirectionalWarpObject::angle() {
    return m_angle;
}

void DirectionalWarpObject::setAngle(int angle) {
    m_angle = angle;
    warpedTex = true;
    update();
}

QVector2D DirectionalWarpObject::resolution() {
    return m_resolution;
}

void DirectionalWarpObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint DirectionalWarpObject::bpc() {
    return m_bpc;
}

void DirectionalWarpObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

DirectionalWarpRenderer::DirectionalWarpRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();
    dirWarpShader = new QOpenGLShaderProgram();
    dirWarpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    dirWarpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/directionalwarp.frag");
    dirWarpShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    dirWarpShader->bind();
    dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("sourceTexture"), 0);
    dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("warpTexture"), 1);
    dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("maskTexture"), 2);
    dirWarpShader->release();
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
    glGenFramebuffers(1, &warpFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glGenTextures(1, &m_warpedTexture);
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_warpedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalWarpRenderer::~DirectionalWarpRenderer() {
    delete dirWarpShader;
    delete checkerShader;
    delete textureShader;
    glDeleteTextures(1, &m_warpTexture);
    glDeleteTextures(1, &m_warpedTexture);
    glDeleteFramebuffers(1, &warpFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *DirectionalWarpRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void DirectionalWarpRenderer::synchronize(QQuickFramebufferObject *item) {
    DirectionalWarpObject *dirWarpItem = static_cast<DirectionalWarpObject*>(item);
    if(dirWarpItem->resUpdated) {
        dirWarpItem->resUpdated = false;
        m_resolution = dirWarpItem->resolution();
        updateTexResolution();
    }
    if(dirWarpItem->bpcUpdated) {
        dirWarpItem->bpcUpdated = false;
        m_bpc = dirWarpItem->bpc();
        updateTexResolution();
        createDirectionalWarp();
        dirWarpItem->setTexture(m_warpedTexture);
    }
    if(dirWarpItem->warpedTex) {
        dirWarpItem->warpedTex = false;
        m_sourceTexture = dirWarpItem->sourceTexture();
        glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
        if(m_sourceTexture) {
            m_warpTexture = dirWarpItem->warpTexture();
            maskTexture = dirWarpItem->maskTexture();
            dirWarpShader->bind();
            dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("useMask"), maskTexture);
            dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("intensity"), dirWarpItem->intensity());
            dirWarpShader->setUniformValue(dirWarpShader->uniformLocation("angle"), dirWarpItem->angle());
            dirWarpShader->release();
            createDirectionalWarp();
            dirWarpItem->setTexture(m_warpedTexture);
            dirWarpItem->updatePreview(m_warpedTexture);
        }
    }
    if(dirWarpItem->texSaving) {
        dirWarpItem->texSaving = false;
        saveTexture(dirWarpItem->saveName);
    }
}

void DirectionalWarpRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void DirectionalWarpRenderer::createDirectionalWarp() {
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    dirWarpShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_warpTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    dirWarpShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void DirectionalWarpRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    else if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DirectionalWarpRenderer::saveTexture(QString fileName) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
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
