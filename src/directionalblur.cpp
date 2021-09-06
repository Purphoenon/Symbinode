#include "directionalblur.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"

DirectionalBlurObject::DirectionalBlurObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                             float intensity, int angle): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_bpc(bpc), m_intensity(intensity), m_angle(angle)
{

}

QQuickFramebufferObject::Renderer *DirectionalBlurObject::createRenderer() const {
    return new DirectionalBlurRenderer(m_resolution, m_bpc);
}

unsigned int &DirectionalBlurObject::texture() {
    return m_texture;
}

void DirectionalBlurObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int DirectionalBlurObject::maskTexture() {
    return m_maskTexture;
}

void DirectionalBlurObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
}

unsigned int DirectionalBlurObject::sourceTexture() {
    return m_sourceTexture;
}

void DirectionalBlurObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
}

void DirectionalBlurObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

float DirectionalBlurObject::intensity() {
    return m_intensity;
}

void DirectionalBlurObject::setIntensity(float intensity) {
    if(m_intensity == intensity) return;
    m_intensity = intensity;
    bluredTex = true;
}

int DirectionalBlurObject::angle() {
    return m_angle;
}

void DirectionalBlurObject::setAngle(int angle) {
    if(m_angle == angle) return;
    m_angle = angle;
    bluredTex = true;
}

QVector2D DirectionalBlurObject::resolution() {
    return m_resolution;
}

void DirectionalBlurObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint DirectionalBlurObject::bpc() {
    return m_bpc;
}

void DirectionalBlurObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
}

DirectionalBlurRenderer::DirectionalBlurRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc) {
    initializeOpenGLFunctions();
    dirBlurShader = new QOpenGLShaderProgram();
    dirBlurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    dirBlurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/directionalblur.frag");
    dirBlurShader->link();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    dirBlurShader->bind();
    dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("sourceTexture"), 0);
    dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("maskTexture"), 1);
    dirBlurShader->release();
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
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DirectionalBlurRenderer::~DirectionalBlurRenderer() {
    delete dirBlurShader;
    delete checkerShader;
    delete textureShader;
    glDeleteTextures(2, &pingpongBuffer[0]);
    glDeleteFramebuffers(2, &pingpongFBO[0]);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *DirectionalBlurRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void DirectionalBlurRenderer::synchronize(QQuickFramebufferObject *item) {
    DirectionalBlurObject *dirBlurItem = static_cast<DirectionalBlurObject*>(item);
    if(dirBlurItem->resUpdated) {
        dirBlurItem->resUpdated = false;
        m_resolution = dirBlurItem->resolution();
        updateTexResolution();
    }
    if(dirBlurItem->bluredTex || dirBlurItem->bpcUpdated) {
        if(dirBlurItem->bpcUpdated) {
            dirBlurItem->bpcUpdated = false;
            m_bpc = dirBlurItem->bpc();
            updateTexResolution();
        }
        if(dirBlurItem->bluredTex) {
            dirBlurItem->bluredTex = false;
            m_sourceTexture = dirBlurItem->sourceTexture();
            if(m_sourceTexture) {
                maskTexture = dirBlurItem->maskTexture();
                dirBlurShader->bind();
                dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("angle"), dirBlurItem->angle());
                dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("intensity"), dirBlurItem->intensity());
                dirBlurShader->release();
                m_intensity = dirBlurItem->intensity();
            }
        }
        if(m_sourceTexture) {
            createDirectionalBlur();
            dirBlurItem->setTexture(pingpongBuffer[1]);
            dirBlurItem->updatePreview(pingpongBuffer[1]);
        }
    }
    if(dirBlurItem->texSaving) {
        dirBlurItem->texSaving = false;
        saveTexture(dirBlurItem->saveName);
    }
}

void DirectionalBlurRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void DirectionalBlurRenderer::createDirectionalBlur() {
    bool first_iteration = true;
    QVector2D dir = QVector2D(1, 0);
    int amount = 50;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
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
        dirBlurShader->bind();
        dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("useMask"), false);
        dir = QVector2D(1, 1) - dir;
        dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("resolution"), m_resolution);
        if(i == amount - 1) dirBlurShader->setUniformValue(dirBlurShader->uniformLocation("useMask"), maskTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);        
        dirBlurShader->release();
        glBindVertexArray(0);
        if (first_iteration)
            first_iteration = false;
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i%2]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
    glFlush();
    glFinish();
}

void DirectionalBlurRenderer::updateTexResolution() {
    if(m_bpc == GL_RGBA16) {
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else if(m_bpc == GL_RGBA8) {
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void DirectionalBlurRenderer::saveTexture(QString fileName) {
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
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
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
