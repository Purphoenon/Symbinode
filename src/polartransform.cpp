#include "polartransform.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

PolarTransformObject::PolarTransformObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                           float radius, bool clamp, int angle):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_radius(radius), m_clamp(clamp),
    m_angle(angle)
{

}

QQuickFramebufferObject::Renderer *PolarTransformObject::createRenderer() const {
    return new PolarTransformRenderer(m_resolution, m_bpc);
}

unsigned int &PolarTransformObject::texture() {
    return m_texture;
}

void PolarTransformObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int PolarTransformObject::maskTexture() {
    return m_maskTexture;
}

void PolarTransformObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
}

void PolarTransformObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

unsigned int PolarTransformObject::sourceTexture() {
    return m_sourceTexture;
}

void PolarTransformObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
}

float PolarTransformObject::radius() {
    return m_radius;
}

void PolarTransformObject::setRadius(float radius) {
    m_radius = radius;
    polaredTex = true;
    update();
}

bool PolarTransformObject::clamp() {
    return m_clamp;
}

void PolarTransformObject::setClamp(bool clamp) {
    m_clamp = clamp;
    polaredTex = true;
    update();
}

int PolarTransformObject::angle() {
    return m_angle;
}

void PolarTransformObject::setAngle(int angle) {
    m_angle = angle;
    polaredTex = true;
    update();
}

QVector2D PolarTransformObject::resolution() {
    return m_resolution;
}

void PolarTransformObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint PolarTransformObject::bpc() {
    return m_bpc;
}

void PolarTransformObject::setBPC(GLint bpc) {
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

PolarTransformRenderer::PolarTransformRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();

    polarShader = new QOpenGLShaderProgram();
    polarShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    polarShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/polartransform.frag");
    polarShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    polarShader->bind();
    polarShader->setUniformValue(polarShader->uniformLocation("sourceTexture"), 0);
    polarShader->setUniformValue(polarShader->uniformLocation("maskTexture"), 1);
    polarShader->release();
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
    glGenFramebuffers(1, &polarFBO);
    glGenTextures(1, &m_polarTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, polarFBO);
    glBindTexture(GL_TEXTURE_2D, m_polarTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_polarTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PolarTransformRenderer::~PolarTransformRenderer() {
    delete polarShader;
    delete textureShader;
    delete checkerShader;
    glDeleteTextures(1, &m_polarTexture);
    glDeleteFramebuffers(1, &polarFBO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *PolarTransformRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void PolarTransformRenderer::synchronize(QQuickFramebufferObject *item) {
    PolarTransformObject *polarItem = static_cast<PolarTransformObject*>(item);

    if(polarItem->polaredTex) {
        polarItem->polaredTex = false;
        m_sourceTexture = polarItem->sourceTexture();
        if(m_sourceTexture) {
            m_maskTexture = polarItem->maskTexture();
            polarShader->bind();
            polarShader->setUniformValue(polarShader->uniformLocation("radius"), polarItem->radius());
            polarShader->setUniformValue(polarShader->uniformLocation("useClamp"), polarItem->clamp());
            polarShader->setUniformValue(polarShader->uniformLocation("angle"), polarItem->angle());
            polarShader->setUniformValue(polarShader->uniformLocation("useMask"), m_maskTexture);
            transformToPolar();
            polarItem->setTexture(m_polarTexture);
            polarItem->updatePreview(m_polarTexture);
        }
    }
    if(polarItem->resUpdated) {
        polarItem->resUpdated = false;
        m_resolution = polarItem->resolution();
        updateTexResolution();
    }
    if(polarItem->bpcUpdated) {
        polarItem->bpcUpdated = false;
        m_bpc = polarItem->bpc();
        updateTexResolution();
        transformToPolar();
        polarItem->setTexture(m_polarTexture);
    }
    if(polarItem->texSaving) {
        polarItem->texSaving = false;
        saveTexture(polarItem->saveName);
    }
}

void PolarTransformRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_polarTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void PolarTransformRenderer::transformToPolar() {
    glBindFramebuffer(GL_FRAMEBUFFER, polarFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    polarShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    polarShader->release();
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

void PolarTransformRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_polarTexture);
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
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PolarTransformRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_polarTexture);
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
