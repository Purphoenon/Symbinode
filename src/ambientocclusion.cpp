#include <QOpenGLFramebufferObjectFormat>
#include "ambientocclusion.h"
#include "FreeImage.h"

AmbientOcclusionObject::AmbientOcclusionObject(QQuickItem *parent, QVector2D resolution, GLint bpc,
                                               float radius, int samples, float strength, float smooth):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_bpc(bpc), m_radius(radius),
    m_samples(samples), m_strength(strength), m_smooth(smooth)
{

}

unsigned int &AmbientOcclusionObject::texture() {
    return m_texture;
}

void AmbientOcclusionObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int AmbientOcclusionObject::sourceTexture() {
    return m_sourceTexture;
}

void AmbientOcclusionObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    occlusionTex = true;
}

unsigned int AmbientOcclusionObject::maskTexture() {
    return m_maskTexture;
}

void AmbientOcclusionObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    bluredTex = true;
}

void AmbientOcclusionObject::saveTexture(QString fileName) {
    texSaving = true;
    saveName = fileName;
    update();
}

QQuickFramebufferObject::Renderer *AmbientOcclusionObject::createRenderer() const {
    return new AmbientOcclusionRenderer(m_resolution, m_bpc);
}

float AmbientOcclusionObject::radius() {
    return m_radius;
}

void AmbientOcclusionObject::setRadius(float radius) {
    if(m_radius == radius) return;
    m_radius = radius;
    occlusionTex = true;
}

int AmbientOcclusionObject::samples() {
    return m_samples;
}

void AmbientOcclusionObject::setSamples(int samples) {
    if(m_samples == samples) return;
    m_samples = samples;
    occlusionTex = true;
}

float AmbientOcclusionObject::strength() {
    return m_strength;
}

void AmbientOcclusionObject::setStrength(float strength) {
    if(m_strength == strength) return;
    m_strength = strength;
    occlusionTex = true;
}

float AmbientOcclusionObject::ambientSmooth() {
    return m_smooth;
}

void AmbientOcclusionObject::setSmooth(float smooth) {
    if(m_smooth == smooth) return;
    m_smooth = smooth;
    bluredTex = true;
}

QVector2D AmbientOcclusionObject::resolution() {
    return m_resolution;
}

void AmbientOcclusionObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint AmbientOcclusionObject::bpc() {
    return m_bpc;
}

void AmbientOcclusionObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

AmbientOcclusionRenderer::AmbientOcclusionRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc)
{
    initializeOpenGLFunctions();

    blurShader = new QOpenGLShaderProgram();
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blurgauss.frag");
    blurShader->link();

    ambientShader = new QOpenGLShaderProgram();
    ambientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    ambientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/ambientocclusion.frag");
    ambientShader->link();

    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    blurShader->bind();
    blurShader->setUniformValue(blurShader->uniformLocation("sourceTexture"), 0);
    blurShader->setUniformValue(blurShader->uniformLocation("maskTexture"), 1);
    blurShader->release();
    ambientShader->bind();
    ambientShader->setUniformValue(ambientShader->uniformLocation("heightTexture"), 0);
    ambientShader->setUniformValue(ambientShader->uniformLocation("noiseTexture"), 1);
    ambientShader->release();
    randomShader->bind();
    randomShader->setUniformValue(randomShader->uniformLocation("seed"), 1);
    randomShader->release();
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
    glGenTextures(1, &m_texture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glBindTexture(GL_TEXTURE_2D, m_texture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &ambientFBO);
    glGenTextures(1, &m_ambientTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, ambientFBO);
    glBindTexture(GL_TEXTURE_2D, m_ambientTexture);
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
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ambientTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

    glGenFramebuffers(1, &randomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glGenTextures(1, &m_randomTexture);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

AmbientOcclusionRenderer::~AmbientOcclusionRenderer() {
    delete textureShader;
    delete ambientShader;
    delete blurShader;
    delete randomShader;
    delete checkerShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(1, &m_texture);
    glDeleteTextures(1, &m_ambientTexture);
    glDeleteTextures(2, &pingpongBuffer[0]);
    glDeleteTextures(1, &m_randomTexture);
    glDeleteFramebuffers(1, &FBO);
    glDeleteFramebuffers(1, &ambientFBO);
    glDeleteFramebuffers(2, &pingpongFBO[0]);
    glDeleteFramebuffers(1, &randomFBO);
}

QOpenGLFramebufferObject *AmbientOcclusionRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void AmbientOcclusionRenderer::synchronize(QQuickFramebufferObject *item) {
    AmbientOcclusionObject *ambientItem = static_cast<AmbientOcclusionObject*>(item);

    if(ambientItem->resUpdated) {
        ambientItem->resUpdated = false;
        m_resolution = ambientItem->resolution();
        updateTexResolution();
    }
    if(ambientItem->occlusionTex || ambientItem->bpcUpdated) {
        if(ambientItem->occlusionTex) {
            ambientItem->occlusionTex = false;
            m_sourceTexture = ambientItem->sourceTexture();
            ambientShader->bind();
            ambientShader->setUniformValue(ambientShader->uniformLocation("radius"), ambientItem->radius());
            ambientShader->setUniformValue(ambientShader->uniformLocation("samples"), ambientItem->samples());
            ambientShader->setUniformValue(ambientShader->uniformLocation("strength"), ambientItem->strength());
            blurShader->bind();
            blurShader->setUniformValue(blurShader->uniformLocation("radius"), 10.0f*ambientItem->ambientSmooth());
        }
        if(ambientItem->bpcUpdated) {
            ambientItem->bpcUpdated = false;
            m_bpc = ambientItem->bpc();
            updateTexResolution();
        }
        if(m_sourceTexture) {            
            ambientOcclusion();
            blur();
            ambientItem->setTexture(m_texture);
            ambientItem->updatePreview(m_texture);
        }
    }
    if(ambientItem->bluredTex) {
        ambientItem->bluredTex = false;
        m_maskTexture = ambientItem->maskTexture();
        blurShader->bind();
        blurShader->setUniformValue(blurShader->uniformLocation("radius"), 10.0f*ambientItem->ambientSmooth());
        blur();
        ambientItem->setTexture(m_texture);
        ambientItem->updatePreview(m_texture);
    }
    if(ambientItem->texSaving) {
        ambientItem->texSaving = false;
        saveTexture(ambientItem->saveName);
    }
}

void AmbientOcclusionRenderer::render() {
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
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void AmbientOcclusionRenderer::ambientOcclusion() {
    glBindFramebuffer(GL_FRAMEBUFFER, ambientFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    ambientShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ambientShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);    
}

void AmbientOcclusionRenderer::blur() {
    bool first_iteration = true;
    QVector2D dir = QVector2D(1, 0);
    int amount = 2;
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i%2]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D, first_iteration ? m_ambientTexture : pingpongBuffer[1 - (i%2)]
        );
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_maskTexture);

        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(textureVAO);
        blurShader->bind();
        blurShader->setUniformValue(blurShader->uniformLocation("useMask"), m_maskTexture && (i == amount - 1));
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

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFlush();
    glFinish();
}

void AmbientOcclusionRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_texture);
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

    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[0]);
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
    glBindTexture(GL_TEXTURE_2D, pingpongBuffer[1]);
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

    glBindTexture(GL_TEXTURE_2D, m_ambientTexture);
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

void AmbientOcclusionRenderer::saveTexture(QString fileName) {
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
    glBindTexture(GL_TEXTURE_2D, m_texture);
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
