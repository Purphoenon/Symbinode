#include "curvature.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

CurvatureObject::CurvatureObject(QQuickItem *parent, QVector2D resolution, GLint bpc, float intensity,
                                 int offset): QQuickFramebufferObject (parent), m_resolution(resolution),
    m_bpc(bpc), m_intensity(intensity), m_offset(offset)
{

}

QQuickFramebufferObject::Renderer *CurvatureObject::createRenderer() const {
    return new CurvatureRenderer(m_resolution, m_bpc);
}

unsigned int &CurvatureObject::texture() {
    return m_texture;
}

void CurvatureObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int CurvatureObject::maskTexture() {
    return m_maskTexture;
}

void CurvatureObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    curvatureTex = true;
}

unsigned int CurvatureObject::sourceTexture() {
    return m_sourceTexture;
}

void CurvatureObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    curvatureTex = true;
}

void CurvatureObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float CurvatureObject::intensity() {
    return m_intensity;
}

void CurvatureObject::setIntensity(float intensity) {
    if(m_intensity == intensity) return;
    m_intensity = intensity;
    curvatureTex = true;
}

int CurvatureObject::offset() {
    return m_offset;
}

void CurvatureObject::setOffset(int offset) {
    if(m_offset == offset) return;
    m_offset = offset;
    curvatureTex = true;
}

QVector2D CurvatureObject::resolution() {
    return m_resolution;
}

void CurvatureObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint CurvatureObject::bpc() {
    return m_bpc;
}

void CurvatureObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

CurvatureRenderer::CurvatureRenderer(QVector2D resolution, GLint bpc): m_resolution(resolution), m_bpc(bpc)
{
    initializeOpenGLFunctions();

    curvatureShader = new QOpenGLShaderProgram();
    curvatureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    curvatureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/curvature.frag");
    curvatureShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    curvatureShader->bind();
    curvatureShader->setUniformValue(curvatureShader->uniformLocation("normalTexture"), 0);
    curvatureShader->setUniformValue(curvatureShader->uniformLocation("maskTexture"), 1);
    curvatureShader->release();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
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

    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &m_curvatureTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, m_curvatureTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_curvatureTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CurvatureRenderer::~CurvatureRenderer() {
    delete curvatureShader;
    delete textureShader;
    delete checkerShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(1, &m_curvatureTexture);
    glDeleteFramebuffers(1, &FBO);
}

QOpenGLFramebufferObject *CurvatureRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void CurvatureRenderer::synchronize(QQuickFramebufferObject *item) {
    CurvatureObject *curvatureItem = static_cast<CurvatureObject*>(item);

    if(curvatureItem->resUpdated) {
        curvatureItem->resUpdated = false;
        m_resolution = curvatureItem->resolution();
        updateTexResolution();
    }
    if(curvatureItem->curvatureTex || curvatureItem->bpcUpdated) {
        if(curvatureItem->curvatureTex) {
            curvatureItem->curvatureTex = false;
            m_sourceTexture = curvatureItem->sourceTexture();
            m_maskTexture = curvatureItem->maskTexture();
            curvatureShader->bind();
            curvatureShader->setUniformValue(curvatureShader->uniformLocation("intensity"), curvatureItem->intensity());
            curvatureShader->setUniformValue(curvatureShader->uniformLocation("offset"), static_cast<float>(curvatureItem->offset()));
            curvatureShader->setUniformValue(curvatureShader->uniformLocation("useMask"), m_maskTexture);
        }
        if(curvatureItem->bpcUpdated) {
            curvatureItem->bpcUpdated = false;
            m_bpc = curvatureItem->bpc();
            updateTexResolution();
        }
        if(m_sourceTexture) {            
            curvature();
            curvatureItem->setTexture(m_curvatureTexture);
            curvatureItem->updatePreview(m_curvatureTexture);
        }
    }
    if(curvatureItem->texSaving) {
        curvatureItem->texSaving = false;
        saveTexture(curvatureItem->saveName);
    }
}

void CurvatureRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_curvatureTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void CurvatureRenderer::curvature() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    curvatureShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    curvatureShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_curvatureTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFlush();
    glFinish();
}

void CurvatureRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_curvatureTexture);
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

void CurvatureRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_curvatureTexture);
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
