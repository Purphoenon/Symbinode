#include "bevel.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

BevelObject::BevelObject(QQuickItem *parent, QVector2D resolution, float distance, float smooth, bool useAlpha):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_dist(distance), m_smooth(smooth),
    m_alpha(useAlpha)
{

}

QQuickFramebufferObject::Renderer *BevelObject::createRenderer() const {
    return new BevelRenderer(m_resolution);
}

unsigned int &BevelObject::texture() {
    return m_texture;
}

void BevelObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

void BevelObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

unsigned int BevelObject::maskTexture() {
    return m_maskTexture;
}

void BevelObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    beveledTex = true;
    update();
}

unsigned int BevelObject::sourceTexture() {
    return m_sourceTexture;
}

void BevelObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    beveledTex = true;
    update();
}

float BevelObject::distance() {
    return m_dist;
}

void BevelObject::setDistance(float dist) {
    m_dist = dist;
    beveledTex = true;
    update();
}

float BevelObject::smooth() {
    return m_smooth;
}

void BevelObject::setSmooth(float smooth) {
    m_smooth = smooth;
    beveledTex = true;
    update();
}

bool BevelObject::useAlpha() {
    return m_alpha;
}

void BevelObject::setUseAlpha(bool use) {
    m_alpha = use;
    beveledTex = true;
    update();
}

QVector2D BevelObject::resolution() {
    return m_resolution;
}

void BevelObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

BevelRenderer::BevelRenderer(QVector2D res): m_resolution(res)
{
    initializeOpenGLFunctions();
    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    bevelShader = new QOpenGLShaderProgram();
    bevelShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    bevelShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/beveldisplay.frag");
    bevelShader->link();
    preparationShader = new QOpenGLShaderProgram();
    preparationShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    preparationShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fillforjfa.frag");
    preparationShader->link();
    jfaShader = new QOpenGLShaderProgram();
    jfaShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    jfaShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/jumpflooding.frag");
    jfaShader->link();
    blurShader = new QOpenGLShaderProgram();
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    blurShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/blur.frag");
    blurShader->link();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();
    bevelShader->bind();
    bevelShader->setUniformValue(bevelShader->uniformLocation("dfTexture"), 0);
    bevelShader->setUniformValue(bevelShader->uniformLocation("maskTexture"), 1);
    bevelShader->release();
    preparationShader->bind();
    preparationShader->setUniformValue(preparationShader->uniformLocation("sourceTexture"), 0);
    preparationShader->release();
    jfaShader->bind();
    jfaShader->setUniformValue(jfaShader->uniformLocation("sourceTexture"), 0);
    jfaShader->release();
    blurShader->bind();
    blurShader->setUniformValue(blurShader->uniformLocation("sourceTexture"), 0);
    blurShader->release();
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
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glGenTextures(1, &m_initTexture);
    glBindTexture(GL_TEXTURE_2D, m_initTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, m_initTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &bevelFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, bevelFBO);
    glGenTextures(1, &m_bevelTexture);
    glBindTexture(GL_TEXTURE_2D, m_bevelTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bevelTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, jfaFBO);
    glGenTextures(2, m_jfaTexture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, jfaFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_jfaTexture[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_jfaTexture[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, blurFBO);
    glGenTextures(2, m_blurTexture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_blurTexture[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurTexture[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

BevelRenderer::~BevelRenderer()
{
    delete checkerShader;
    delete textureShader;
    delete blurShader;
    delete bevelShader;
    delete preparationShader;
    delete jfaShader;
}

QOpenGLFramebufferObject *BevelRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void BevelRenderer::synchronize(QQuickFramebufferObject *item) {
    BevelObject *bevelItem = static_cast<BevelObject*>(item);

    if(bevelItem->beveledTex) {
        bevelItem->beveledTex = false;
        m_sourceTexture = bevelItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = bevelItem->maskTexture();
            float dist = bevelItem->distance();
            preparationShader->bind();
            preparationShader->setUniformValue(preparationShader->uniformLocation("outer"), dist > 0.0f);
            preparationShader->release();
            bevelShader->bind();
            bevelShader->setUniformValue(bevelShader->uniformLocation("dist"), dist);
            bevelShader->setUniformValue(bevelShader->uniformLocation("useAlpha"), bevelItem->useAlpha());
            bevelShader->release();
            blurShader->bind();
            blurShader->setUniformValue(blurShader->uniformLocation("intensity"), bevelItem->smooth());
            blurShader->release();
            jumpFlooding();
            bevelItem->setTexture(m_bevelTexture);
            bevelItem->updatePreview(m_bevelTexture);
        }
    }

    if(bevelItem->resUpdated) {
        bevelItem->resUpdated = false;
        m_resolution = bevelItem->resolution();
        updateTexResolution();
    }

    if(bevelItem->texSaving) {
        bevelItem->texSaving = false;
        saveTexture(bevelItem->saveName);
    }
}

void BevelRenderer::render() {
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
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_bevelTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
    }
}

void BevelRenderer::jumpFlooding() {
    //preparation of jfa
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    preparationShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    preparationShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //jfa
    bool first_iteration = true;
    int step = m_resolution.x();
    int stepsCount = ceil(log2(step));
    for(int i = 0; i < 10; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, jfaFBO[i%2]);
        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        jfaShader->bind();
        step /= 2;
        jfaShader->setUniformValue(jfaShader->uniformLocation("step"), step);
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_initTexture : m_jfaTexture[1 - i%2]);
        if(first_iteration) first_iteration = false;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        jfaShader->release();
    }

    //dt
    glBindFramebuffer(GL_FRAMEBUFFER, bevelFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    bevelShader->bind();
    bevelShader->setUniformValue(bevelShader->uniformLocation("useMask"), maskTexture);
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_jfaTexture[1 - stepsCount%2]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    bevelShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   

    //blur
    first_iteration = true;
    QVector2D dir = QVector2D(1, 0);
    for(int i = 0; i < 10; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i%2]);
        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        blurShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_bevelTexture : m_blurTexture[1 - i%2]);
        blurShader->setUniformValue(blurShader->uniformLocation("direction"), dir);
        dir = QVector2D(1, 1) - dir;
        blurShader->setUniformValue(blurShader->uniformLocation("resolution"), m_resolution);
        if(first_iteration) first_iteration = false;
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        blurShader->release();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, bevelFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    textureShader->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BevelRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_bevelTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_initTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_jfaTexture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_jfaTexture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, m_blurTexture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BevelRenderer::saveTexture(QString fileName) {
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glBindTexture(GL_TEXTURE_2D, m_bevelTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();

    BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
    FIBITMAP *result = FreeImage_ConvertToRGBA16(image);
    if (FreeImage_Save(FIF_PNG, result, fileName.toUtf8().constData(), 0))
        printf("Successfully saved!\n");
    else
        printf("Failed saving!\n");
    FreeImage_Unload(result);
    FreeImage_Unload(image);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
